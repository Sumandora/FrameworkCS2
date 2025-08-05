#include "BulletTracers.hpp"

#include "../Feature.hpp"

#include "../../SDK/EngineTrace/EngineTrace.hpp"
#include "../../SDK/EngineTrace/Ray.hpp"
#include "../../SDK/Entities/BasePlayerWeapon.hpp"
#include "../../SDK/Entities/CSPlayerPawn.hpp"
#include "../../SDK/Entities/Services/CSPlayerWeaponServices.hpp"
#include "../../SDK/Entities/VData/CSWeaponBaseVData.hpp"
#include "../../SDK/GameClass/GameEvent.hpp"
#include "../../SDK/GameClass/GameSceneNode.hpp"
#include "../../SDK/Padding.hpp"
#include "../../SDK/Particles/GameParticleManager.hpp"
#include "../../SDK/Particles/ParticleEffect.hpp"
#include "../../SDK/Particles/ParticleSnapshot.hpp"
#include "../../SDK/Particles/ParticleSystemMgr.hpp"

#include "../../Interfaces.hpp"
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

struct Particle;

BulletTracers::BulletTracers()
	: Feature("Visuals", "Bullet tracers")
{
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

	GameParticleManager* game_particle_mgr = GameParticleManager::get();

	ParticleEffect effect{ "particles/entity/spectator_utility_trail.vpcf" };
	Particle* particle = game_particle_mgr->create_particle(&effect);

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
	game_particle_mgr->modify_particle(0.0F, particle, 3, &info);

	glm::vec3 color_vec{
		color.Value.x * 255.0F,
		color.Value.y * 255.0F,
		color.Value.z * 255.0F,
	};
	game_particle_mgr->modify_particle(0.0F, particle, 16, &color_vec);

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
	Interfaces::particle_system_mgr->create_particle_snapshot(&snapshot, &unk);

	game_particle_mgr->attach_particle_to_snapshot(particle, 0, &snapshot);

	Interfaces::particle_system_mgr->show_particle(&snapshot, static_cast<int>(positions.size()), &data);
}
