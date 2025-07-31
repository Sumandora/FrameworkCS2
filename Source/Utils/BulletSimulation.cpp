#include "BulletSimulation.hpp"

#include "../SDK/ConVar/ConVar.hpp"
#include "../SDK/ConVar/EngineCvar.hpp"
#include "../SDK/EngineTrace/EngineTrace.hpp"
#include "../SDK/EngineTrace/GameTrace.hpp"
#include "../SDK/EngineTrace/TraceFilter.hpp"
#include "../SDK/Entities/BaseEntity.hpp"
#include "../SDK/Entities/BasePlayerWeapon.hpp"
#include "../SDK/Entities/CSPlayerPawn.hpp"
#include "../SDK/Entities/Services/PlayerWeaponServices.hpp"
#include "../SDK/Entities/VData/CSWeaponBaseVData.hpp"
#include "../SDK/EntityHandle.hpp"
#include "../SDK/Padding.hpp"

#include "../Memory.hpp"

#include "../Interfaces.hpp"

#include "BCRL/SearchConstraints.hpp"
#include "BCRL/Session.hpp"
#include "SignatureScanner/PatternSignature.hpp"

#include "glm/ext/vector_float2.hpp"
#include "glm/ext/vector_float3.hpp"
#include "glm/geometric.hpp"

#include <cstddef>
#include <cstdint>
#include <cstring>

// Reenable for debugging
#ifdef DEBUG_BULLET_SIMULATION
#include "../SDK/Entities/CSPlayerController.hpp"
#include "magic_enum/magic_enum.hpp"
// NOLINTNEXTLINE(cppcoreguidelines-macro-usage)
#define BULLETSIM_DBG(...) Logging::debug(__VA_ARGS__)
#else
#define BULLETSIM_DBG(...)
#endif

struct TraceElement {
	float unk;
	float distance;
	float damage;
	std::uint32_t penetration_count;
	std::uint16_t handle_1;
	std::uint16_t handle_1_2;
	std::uint32_t handle_2;
};
static_assert(sizeof(TraceElement) == 0x18);

struct OtherElem {
	PADDING(0x30);
};
static_assert(sizeof(OtherElem) == 0x30);

struct TraceData {
	PADDING(1024 * 10);

	OFFSET(int, count, 6176);
	OFFSET(TraceElement*, elements, 6184);
	OFFSET(OtherElem*, other_elements, 8);

	OFFSET(glm::vec3, range_vec, 6404);
};

static_assert(sizeof(TraceData) >= 0x1850); // I don't know the real size

struct WeaponData {
	float damage;
	float penetration;
	float range_modifier;
	float range;
	int pen_count;
	bool failed;
};

// The following names are the ones that the Windows community seems to have agreed upon,
// I don't fully agree with them, but in the spirit of comparability I will use them as well.
static glm::vec2 (*create_trace)(TraceData*, glm::vec3*, glm::vec3*, TraceFilter*, int) = nullptr; // Interesting, but non-essential return type.
static bool (*handle_bullet_penetration)(TraceData*, WeaponData*, void*, TeamID, void*) = nullptr;
static void (*get_trace)(TraceData*, GameTrace*, void*, float) = nullptr;

static ConVar* mp_damage_scale_ct_body = nullptr;
static ConVar* mp_damage_scale_t_body = nullptr;
static ConVar* mp_damage_scale_ct_head = nullptr;
static ConVar* mp_damage_scale_t_head = nullptr;

void BulletSimulation::resolve_signatures()
{
	// TODO Find better signatures, this may be hard because all of these functions are in the penetration of FX_FireBullets
	create_trace = BCRL::signature(
		Memory::mem_mgr,
		SignatureScanner::PatternSignature::for_array_of_bytes<"55 48 89 E5 41 57 49 89 CF 41 56 49 89 F6 41 55 41 54">(),
		BCRL::everything(Memory::mem_mgr).thats_readable().thats_executable().with_name("libclient.so"))
					   .BCRL_EXPECT(decltype(create_trace), create_trace);
	handle_bullet_penetration = BCRL::signature(
		Memory::mem_mgr,
		SignatureScanner::PatternSignature::for_array_of_bytes<"55 66 0F EF D2 48 89 E5 41 57 41 56 41 55 49 89 F5">(),
		BCRL::everything(Memory::mem_mgr).thats_readable().thats_executable().with_name("libclient.so"))
									.BCRL_EXPECT(decltype(handle_bullet_penetration), handle_bullet_penetration);
	get_trace = BCRL::signature(
		Memory::mem_mgr,
		SignatureScanner::PatternSignature::for_array_of_bytes<"55 48 89 E5 41 57 66 41 0F 7E C7 41 56 41 55 49 89 FD 48 89 F7">(),
		BCRL::everything(Memory::mem_mgr).thats_readable().thats_executable().with_name("libclient.so"))
					.BCRL_EXPECT(decltype(get_trace), get_trace);

	mp_damage_scale_ct_body = Interfaces::engineCvar->findByName("mp_damage_scale_ct_body");
	MEM_ACCEPT(mp_damage_scale_ct_body);
	mp_damage_scale_t_body = Interfaces::engineCvar->findByName("mp_damage_scale_t_body");
	MEM_ACCEPT(mp_damage_scale_t_body);
	mp_damage_scale_ct_head = Interfaces::engineCvar->findByName("mp_damage_scale_ct_head");
	MEM_ACCEPT(mp_damage_scale_ct_head);
	mp_damage_scale_t_head = Interfaces::engineCvar->findByName("mp_damage_scale_t_head");
	MEM_ACCEPT(mp_damage_scale_t_head);
}

static void scale_damage(CSPlayerPawn* entity, CSWeaponBaseVData* weapon_data, BulletSimulation::Results& results)
{
	// @see CCSPlayer::TraceAttack

	// TODO If the game brings back riot shields proper their support must be reflected here.
	// 		However one should question if never shooting a riot shield is a good tactical decision,
	// 			perhaps "wasting" a shot on destroying it would be justified
	const bool hit_shield = false; // IsHittingShield(vecDir, ptr);

	// NOTE: Game considers non-ct to be t here. Shouldn't matter, but should be noted.
	const bool is_ct = entity->team_id() == TeamID::TEAM_COUNTER_TERRORIST;
	const float body_damage_scale = is_ct ? mp_damage_scale_ct_body->get_float() : mp_damage_scale_t_body->get_float();
	float head_damage_scale = is_ct ? mp_damage_scale_ct_head->get_float() : mp_damage_scale_t_head->get_float();

	float damage = results.raw_damage;

	if (entity->has_heavy_armor())
		head_damage_scale = head_damage_scale * 0.5F;

	if (entity->gun_game_immunity() || hit_shield) {
		results.scaled_damage = 0;
		return;
	}

	switch (results.hit_group) {
	case HitGroup::GENERIC:
		break;
	case HitGroup::HEAD:
		damage *= weapon_data->headshot_multiplier();
		damage *= head_damage_scale;
		break;
	case HitGroup::CHEST:
		damage *= 1.0F;
		damage *= body_damage_scale;
		break;
	case HitGroup::STOMACH:
		damage *= 1.25F;
		damage *= body_damage_scale;
		break;
	case HitGroup::LEFTARM:
	case HitGroup::RIGHTARM:
		damage *= 1.0F;
		damage *= body_damage_scale;
		break;
	case HitGroup::LEFTLEG:
	case HitGroup::RIGHTLEG:
		damage *= 0.75F;
		damage *= body_damage_scale;
		break;
	default:
		break;
	}

	results.scaled_damage = entity->scale_damage_with_armor(
		damage,
		weapon_data->armor_ratio(),
		results.hit_group,
		&results.lost_armor);
}

BulletSimulation::Results BulletSimulation::simulate_bullet(const glm::vec3& from, const glm::vec3& to, BaseEntity* entity)
{
	if (!Memory::local_player
		|| !Memory::local_player->weapon_services()->active_weapon().has_entity()
		|| !Memory::local_player->weapon_services()->active_weapon().get())
		return {}; // leck eier

	TraceData data{};
	const auto indexer = reinterpret_cast<std::uintptr_t>(&data);
	*reinterpret_cast<std::uint64_t*>(indexer + 16) = 0x8000000000000080;
	*reinterpret_cast<void**>(indexer + 8) = reinterpret_cast<void*>(indexer + 0x18);
	*reinterpret_cast<std::uint64_t*>(indexer + 6192) = 0x8000000000000008;
	*reinterpret_cast<void**>(indexer + 6184) = reinterpret_cast<void*>(indexer + 0x1838);

	TraceFilter filter{ 0x1c300b };
	// filter.unk3 = 0x30f;
	// filter.unk4 = 3;

	filter.add_skip(Memory::local_player);

	auto* vdata = static_cast<CSWeaponBaseVData*>(Memory::local_player->weapon_services()->active_weapon().get()->get_vdata());

	glm::vec3 the_from = from;
	glm::vec3 direction = to - from;

	BULLETSIM_DBG("from: {}; to: {}; dir: {}", from, to, direction);

	static constexpr int MAX_PENETRATION_COUNT = 4;

	[[maybe_unused]] const glm::vec2 something = create_trace(&data, &the_from, &direction, &filter, MAX_PENETRATION_COUNT);

	if (data.count() == 0)
		return {};

	WeaponData wep_data{
		.damage = static_cast<float>(vdata->damage()),
		.penetration = vdata->penetration(),
		.range_modifier = vdata->range_modifier(),
		// .range = glm::distance(from, to),
		.range = glm::length(data.range_vec()),
		.pen_count = MAX_PENETRATION_COUNT,
		.failed = false
	};

	BULLETSIM_DBG("wpn dmg: {}", vdata->damage());

	auto damage = static_cast<float>(vdata->damage());
	BULLETSIM_DBG("count: {}", data.count());

	for (int i = 0; i < data.count(); i++) {
		TraceElement* elem = &data.elements()[i];
		// if ((elem->handle_2 & 1) != 0)
		// 	continue; // ???

		const bool failed = handle_bullet_penetration(&data, &wep_data, &data.elements()[i], Memory::local_player->team_id(), nullptr);
		BULLETSIM_DBG("intermediate dmg: {}", wep_data.damage);

		damage = wep_data.damage;

		if (failed)
			break;

		GameTrace game_trace = GameTrace::initialized();

		void* arg3 = reinterpret_cast<void*>(reinterpret_cast<std::uintptr_t>(data.other_elements()) + static_cast<std::uintptr_t>(0x30 * (elem->handle_1_2 & ENTITY_LIST_INDEX_MASK)));
		get_trace(&data, &game_trace, arg3, 0.0F);

		BULLETSIM_DBG("intermediate fraction: {}", game_trace.fraction);
		BULLETSIM_DBG("trace from: {}, trace to: {}", game_trace.from, the_from + game_trace.to);

		if (game_trace.hit_entity != nullptr && (!entity || game_trace.hit_entity == entity)) {
			if (damage < 1.0F)
				return {};

			auto* pawn = game_trace.hit_entity->entity_cast<CSPlayerPawn*>(); // TODO don't recompute

			if (pawn) {
				BULLETSIM_DBG("player_name: {}", pawn->original_controller().get()->sanitized_name());

				BULLETSIM_DBG("hitgroup: {}", magic_enum::enum_name(game_trace.hitbox_data->hitgroup));

				Results results{
					.raw_damage = damage,
					.scaled_damage = damage,
					.hit_group = game_trace.hitbox_data->hitgroup,
					.lost_armor = false,
					.penetration_count = i,
					.hit_entity = game_trace.hit_entity,
				};

				scale_damage(pawn, vdata, results);

				BULLETSIM_DBG("scaled damage: {}", results.scaled_damage);

				// TODO Finalize properly.
				// MemAlloc::the()->deallocate(data.other_elements());
				// MemAlloc::the()->deallocate(data.elements());
				return results;
			}
		}
	}

	// MemAlloc::the()->deallocate(data.other_elements());
	// MemAlloc::the()->deallocate(data.elements());
	return {};
}
