#include "BulletTracers.hpp"

#include "../Feature.hpp"

#include "../../SDK/CUtl/Vector.hpp"
#include "../../SDK/EngineTrace/EngineTrace.hpp"
#include "../../SDK/EngineTrace/Ray.hpp"
#include "../../SDK/Entities/BasePlayerWeapon.hpp"
#include "../../SDK/Entities/CSPlayerPawn.hpp"
#include "../../SDK/Entities/Services/CSPlayerWeaponServices.hpp"
#include "../../SDK/Entities/VData/CSWeaponBaseVData.hpp"
#include "../../SDK/GameClass/GameEvent.hpp"
#include "../../SDK/GameClass/GameSceneNode.hpp"
#include "../../SDK/Padding.hpp"
#include "../../SDK/VirtualMethod.hpp"

#include "BCRL/SearchConstraints.hpp"
#include "BCRL/Session.hpp"
#include "SignatureScanner/PatternSignature.hpp"

#include "../../Memory.hpp"

#include "glm/ext/vector_float3.hpp"
#include "glm/gtx/compatibility.hpp"
#include "glm/trigonometric.hpp"

#include "imgui.h"

#include <cstddef>
#include <cstdint>
#include <cstring>
#include <string_view>
#include <vector>

struct ParticleEffect {
	const char* name;
	int unk = 8;
	char pad[0x3c];

	explicit ParticleEffect(const char* name)
		: name(name)
	{
		std::memset(pad, 0, sizeof(pad));
	}
};
static_assert(sizeof(ParticleEffect) == 0x48);

struct Particle;

struct ParticleManager { // TODO check the actual name
	VIRTUAL_METHOD(4, show_particle, void, (Particle * particle), (this, particle));
};

struct ParticleSnapshot {
};

static ParticleManager* (*get_particles)() = nullptr;
static Particle* (*create_particle)(ParticleManager* thisptr, ParticleEffect* effect) = nullptr;
static void (*modify_particle)(float unk, ParticleManager* thisptr, Particle* particle, int control_point_id, void* data) = nullptr;
static void (*attach_particle_to_snapshot)(ParticleManager* thisptr, Particle* particle, int unk, ParticleSnapshot** snapshot) = nullptr;

struct ParticleManager2 { // TODO check actual name
	STATIC_VIRTUAL_METHOD(41, create_particle_snapshot, void,
		(ParticleSnapshot * *snapshot, ParticleManager2* manager, std::uint64_t* unk),
		(this, snapshot, manager, unk));

	VIRTUAL_METHOD(42, show_particle, void,
		(ParticleSnapshot * *snapshot, int count, void* data),
		(this, snapshot, count, data));
};

static ParticleManager2** particle_manager2 = nullptr;

BulletTracers::BulletTracers()
	: Feature("Visuals", "Bullet tracers")
{
	// TODO Better sigs
	get_particles
		= BCRL::signature(
			Memory::mem_mgr,
			SignatureScanner::PatternSignature::for_array_of_bytes<"E8 ? ? ? ? 66 0F EF C0 0F 29 45 ? 0F 28 05 ? ? ? ? 48 89 C7 4C 89 F6 C7 85 ? ? ? ? 02 00 00 00">(),
			BCRL::everything(Memory::mem_mgr).with_flags("r-x").with_name("libclient.so"))
			  .add(1)
			  .relative_to_absolute()
			  .BCRL_EXPECT(decltype(get_particles), get_particles);
	create_particle
		= BCRL::signature(
			Memory::mem_mgr,
			SignatureScanner::PatternSignature::for_array_of_bytes<"E8 ? ? ? ? 41 89 85 ? ? ? ? E8">(),
			BCRL::everything(Memory::mem_mgr).with_flags("r-x").with_name("libclient.so"))
			  .add(1)
			  .relative_to_absolute()
			  .BCRL_EXPECT(decltype(create_particle), create_particle);
	modify_particle
		= BCRL::signature(
			Memory::mem_mgr,
			SignatureScanner::PatternSignature::for_array_of_bytes<"E8 ? ? ? ? E8 ? ? ? ? 66 0F EF C0 49 89 C7">(),
			BCRL::everything(Memory::mem_mgr).with_flags("r-x").with_name("libclient.so"))
			  .add(1)
			  .relative_to_absolute()
			  .BCRL_EXPECT(decltype(modify_particle), modify_particle);
	attach_particle_to_snapshot
		= BCRL::signature(
			Memory::mem_mgr,
			SignatureScanner::PatternSignature::for_array_of_bytes<"E8 ? ? ? ? E9 ? ? ? ? ? ? ? ? ? ? ? ? ? 41 0F B6 85">(),
			BCRL::everything(Memory::mem_mgr).with_flags("r-x").with_name("libclient.so"))
			  .add(1)
			  .relative_to_absolute()
			  .BCRL_EXPECT(decltype(attach_particle_to_snapshot), attach_particle_to_snapshot);
	particle_manager2
		= BCRL::signature(
			Memory::mem_mgr,
			SignatureScanner::PatternSignature::for_array_of_bytes<"4C 8D 3D ? ? ? ? 0F 84 ? ? ? ? 48 8D 85">(),

			BCRL::everything(Memory::mem_mgr).with_flags("r-x").with_name("libclient.so"))
			  .add(3)
			  .relative_to_absolute()
			  .BCRL_EXPECT(ParticleManager2**, particle_manager2);
}

void BulletTracers::update_viewangles(const glm::vec3& view_angles)
{
	if (!Memory::local_player) {
		this->view_angles = view_angles;
		return;
	}
	this->view_angles = view_angles + glm::vec3{ Memory::local_player->get_aim_punch(), 0.0F };
}

void BulletTracers::event_handler(GameEvent* event)
{
	if (!enabled.get())
		return;

	if (std::string_view{ event->GetName() } != "weapon_fire")
		return;

	BaseEntity* shooter = event->get_entity("userid");
	if (!shooter)
		return;

	if (shooter != Memory::local_player)
		return;

	CSPlayerWeaponServices* weapon_services = Memory::local_player->weapon_services();
	if (!weapon_services)
		return;

	BasePlayerWeapon* weapon = weapon_services->active_weapon().get();
	if (!weapon)
		return;

	auto* vdata = static_cast<CSWeaponBaseVData*>(weapon->get_vdata());
	if (!vdata)
		return;

	if (!vdata->is_gun())
		return;

	ParticleManager* mgr = get_particles();

	const glm::vec3 from = Memory::local_player->gameSceneNode()->transform().m_Position + Memory::local_player->view_offset();
	const float pitch = glm::radians(view_angles.x);
	const float yaw = glm::radians(view_angles.y);
	const glm::vec3 shoot_dir{
		glm::cos(yaw) * glm::cos(pitch),
		glm::sin(yaw) * glm::cos(pitch),
		-glm::sin(pitch),
	};
	const glm::vec3 furthest_to = from + shoot_dir * vdata->range();

	GameTrace trace = GameTrace::initialized();
	TraceFilter filter{ TraceFilter::MASK1 };
	filter.add_skip(Memory::local_player);
	Ray ray{};

	EngineTrace::the()->trace_shape(&ray, from, furthest_to, &filter, &trace);

	const glm::vec3 to = from + (furthest_to - from) * trace.fraction;

	ParticleEffect effect{ "particles/entity/spectator_utility_trail.vpcf" };
	Particle* particle = create_particle(mgr, &effect);

	// glm::vec3 zero{
	// 	0.0F,
	// 	0.0F,
	// 	0.0F,
	// };
	// modify_particle(0.0F, mgr, particle, 0, &zero);
	const ImColor color = this->color.get();

	glm::vec3 info{
		lifetime.get(),
		width.get(),
		color.Value.w,
	};
	modify_particle(0.0F, mgr, particle, 3, &info);

	glm::vec3 color_vec{
		color.Value.x * 255.0F,
		color.Value.y * 255.0F,
		color.Value.z * 255.0F,
	};
	modify_particle(0.0F, mgr, particle, 16, &color_vec);

	ParticleSnapshot* snapshot = nullptr;

	struct ParticleData {
		glm::vec3* positions{};
		PADDING(0x1024); // :thumbsup:
	};

	// Since the spectator utility trail shape increases towards the middle, these two extra positions can be used to make it increase immediately.
	std::vector<glm::vec3> positions{
		from,
		glm::lerp(from, to, 0.05F),
		glm::lerp(from, to, 0.50F),
		glm::lerp(from, to, 0.95F),
		to,
	};

	ParticleData data{};
	data.positions = positions.data();

	std::uint64_t unk = 0;
	(*particle_manager2)->create_particle_snapshot(&snapshot, *particle_manager2, &unk);

	attach_particle_to_snapshot(mgr, particle, 0, &snapshot);

	(*particle_manager2)->show_particle(&snapshot, static_cast<int>(positions.size()), &data);
}
