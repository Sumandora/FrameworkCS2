#include "GrenadePrediction.hpp"

#include "../Feature.hpp"

#include "BCRL/SearchConstraints.hpp"
#include "BCRL/Session.hpp"

#include "../../Memory.hpp"

#include "../../Utils/Projection.hpp"

#include "glm/ext/vector_float3.hpp"

#include "RetAddrSpoofer.hpp"

#include "imgui.h"

#include "SignatureScanner/PatternSignature.hpp"

#include "../../SDK/Entities/BaseCSGrenade.hpp"
#include "../../SDK/Entities/BasePlayerWeapon.hpp"
#include "../../SDK/Entities/CSPlayerPawn.hpp"
#include "../../SDK/Entities/Services/PlayerWeaponServices.hpp"
#include "../../SDK/Padding.hpp"

#include <cstddef>
#include <vector>

struct GrenadePredictor {
	struct Element {
		int count;
		PADDING(4);
		glm::vec3* vecs;
		PADDING(4);
	};
	static_assert(sizeof(Element) == 0x18);
	PADDING(0xce8);
	int count;
	PADDING(4);
	struct Element* elements;
};

static_assert(offsetof(GrenadePredictor, count) == 0xce8);
static_assert(offsetof(GrenadePredictor, elements) == 0xcf0);

static BasePlayerWeapon* (*get_weapon)(BaseEntity* player);
static void (*calculate_initial_state)(BasePlayerWeapon* weapon, BaseEntity* player, glm::vec3* pos, glm::vec3* rot, bool jump_throw);
static GrenadePredictor* (*make_grenade_predictor)(float time_to_render, int grenade_type, BaseCSGrenade* grenade);
static void(*predict_grenade)(GrenadePredictor* predictor, glm::vec3* pos, glm::vec3* rot);
static void(*remove_entity)(GrenadePredictor* predictor);

GrenadePrediction::GrenadePrediction()
	: Feature("Visuals", "Grenade prediction")
{
	get_weapon = BCRL::signature(
		Memory::mem_mgr,
		SignatureScanner::PatternSignature::for_array_of_bytes<"55 48 89 E5 41 54 49 89 FC 48 83 EC 08 48 8B BF ? ? ? ? 48 85 FF 74 ? 4C 8B 65">(),
		BCRL::everything(Memory::mem_mgr).thats_readable().thats_executable().with_name("libclient.so"))
					 .expect<decltype(get_weapon)>("Failed to find get_weapon");
	calculate_initial_state = BCRL::signature(
		Memory::mem_mgr,
		SignatureScanner::PatternSignature::for_array_of_bytes<"55 48 89 E5 41 57 49 89 FF 41 56 4C 8D 8D">(),
		BCRL::everything(Memory::mem_mgr).thats_readable().thats_executable().with_name("libclient.so"))
								  .expect<decltype(calculate_initial_state)>("Failed to find calculate_initial_state");
	make_grenade_predictor = BCRL::signature(
		Memory::mem_mgr,
		SignatureScanner::PatternSignature::for_array_of_bytes<"55 48 8D 05 ? ? ? ? 48 89 E5 41 57 41 56 49 89 F6 41 55 4C 63 EF">(),
		BCRL::everything(Memory::mem_mgr).thats_readable().thats_executable().with_name("libclient.so"))
								 .expect<decltype(make_grenade_predictor)>("Failed to find make_grenade_predictor");
	predict_grenade = BCRL::signature(
		Memory::mem_mgr,
		SignatureScanner::PatternSignature::for_array_of_bytes<"55 48 89 E5 41 57 41 56 41 55 49 89 D5 41 54 49 89 FC 53 48 83 EC 78">(),
		BCRL::everything(Memory::mem_mgr).thats_readable().thats_executable().with_name("libclient.so"))
						  .expect<decltype(predict_grenade)>("Failed to find predict_grenade");
	remove_entity = BCRL::signature(
		Memory::mem_mgr,
		SignatureScanner::PatternSignature::for_array_of_bytes<"48 85 FF 0F 84 ? ? ? ? 55 48 89 E5 41 55 41 54 48 8B 47">(),
		BCRL::everything(Memory::mem_mgr).thats_readable().thats_executable().with_name("libclient.so"))
						.expect<decltype(remove_entity)>("Failed to find remove_entity");
}

static std::vector<glm::vec3> points;

void GrenadePrediction::calculate_grenade_prediction()
{
	BaseEntity* player = Memory::local_player;
	if (!player)
		return;

	BasePlayerWeapon* weapon = RetAddrSpoofer::invoke(get_weapon, player);
	if (!weapon)
		return;

	auto* grenade = weapon->entity_cast<BaseCSGrenade*>();
	if (!grenade || !grenade->pin_pulled())
		return;

	const int grenade_type = grenade->get_grenade_type();

	auto* predictor = RetAddrSpoofer::invoke(make_grenade_predictor, 30.0F, grenade_type, grenade);

	glm::vec3 position;
	glm::vec3 velocity;

	RetAddrSpoofer::invoke(calculate_initial_state, weapon, player, &position, &velocity, predict_jumpthrow.get());
	RetAddrSpoofer::invoke(predict_grenade, predictor, &position, &velocity);

	for (int i = 0; i < predictor->count; i++) {
		const GrenadePredictor::Element& elem = predictor->elements[i];

		for (int j = 0; j < elem.count; j++) {
			const glm::vec3& v = elem.vecs[j];
			points.emplace_back(v);
		}
	}

	RetAddrSpoofer::invoke(remove_entity, predictor);
}

void GrenadePrediction::draw(ImDrawList* draw_list)
{
	points.clear();
	if (!enabled.get())
		return;

	calculate_grenade_prediction();

	std::vector<ImVec2> screen_points;
	screen_points.reserve(points.size());

	for (const glm::vec3& point : points) {
		ImVec2 screen;
		if (!Projection::project(point, screen))
			continue;

		screen_points.emplace_back(screen);
	}

	line.draw(draw_list, nullptr, screen_points);
}
