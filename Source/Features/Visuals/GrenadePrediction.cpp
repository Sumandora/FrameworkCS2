#include "GrenadePrediction.hpp"

#include "../Feature.hpp"

#include "BCRL/SearchConstraints.hpp"
#include "BCRL/Session.hpp"

#include "SignatureScanner/PatternSignature.hpp"
#include "SignatureScanner/XRefSignature.hpp"

#include "../../Memory.hpp"

#include "../../Utils/Projection.hpp"

#include "glm/ext/vector_float3.hpp"

#include "RetAddrSpoofer.hpp"

#include "imgui.h"

#include "../../SDK/Entities/BaseCSGrenade.hpp"
#include "../../SDK/Entities/BasePlayerWeapon.hpp"
#include "../../SDK/Entities/CSPlayerPawn.hpp"
#include "../../SDK/Entities/Services/CSPlayerWeaponServices.hpp"
#include "../../SDK/Entities/Services/PlayerWeaponServices.hpp"
#include "../../SDK/Padding.hpp"

#include <cstddef>
#include <mutex>
#include <vector>

struct GrenadePredictor {
	struct Element {
		int count;
		PADDING(4);
		glm::vec3* vecs;
		PADDING(4);
	};
	static_assert(sizeof(Element) == 0x18);
	PADDING(0xe90);
	int count;
	PADDING(0x4);
	Element* elements;
};

static void (*calculate_initial_state)(BasePlayerWeapon* weapon, BaseEntity* player, glm::vec3* pos, glm::vec3* rot, bool jump_throw);
static GrenadePredictor* (*make_grenade_predictor)(float time_to_render, int grenade_type, BaseCSGrenade* grenade);
static void (*predict_grenade)(GrenadePredictor* predictor, glm::vec3* pos, glm::vec3* rot);
static void (*remove_entity)(GrenadePredictor* predictor);

GrenadePrediction::GrenadePrediction()
	: Feature("Visuals", "Grenade prediction")
{
	auto convar_callback
		= BCRL::signature(
			Memory::mem_mgr,
			SignatureScanner::PatternSignature::for_literal_string<"cl_sim_grenade_trajectory">(),
			BCRL::everything(Memory::mem_mgr).thats_readable().with_name("libclient.so"))
			  .find_xrefs(SignatureScanner::XRefTypes::relative(),
				  BCRL::everything(Memory::mem_mgr).thats_readable().with_name("libclient.so"))
			  .sub(9)
			  .relative_to_absolute();
	calculate_initial_state
		= convar_callback
			  .clone()
			  .repeater([](auto& ptr) {
				  ptr.next_instruction();
				  static int i = 0;
				  if (ptr.does_match(SignatureScanner::PatternSignature::for_array_of_bytes<"e8">()))
					  i++;
				  return i != 5;
			  })
			  .add(1)
			  .relative_to_absolute()
			  .next_instruction()
			  .next_instruction()
			  .next_instruction()
			  .add(1)
			  .relative_to_absolute()
			  .BCRL_EXPECT(decltype(calculate_initial_state), calculate_initial_state);
	make_grenade_predictor
		= convar_callback
			  .clone()
			  .repeater([](auto& ptr) {
				  ptr.next_instruction();
				  static int i = 0;
				  if (ptr.does_match(SignatureScanner::PatternSignature::for_array_of_bytes<"e8">()))
					  i++;
				  return i != 6;
			  })
			  .add(1)
			  .relative_to_absolute()
			  .BCRL_EXPECT(decltype(make_grenade_predictor), make_grenade_predictor);
	predict_grenade
		= convar_callback
			  .clone()
			  .repeater([](auto& ptr) {
				  ptr.next_instruction();
				  static int i = 0;
				  if (ptr.does_match(SignatureScanner::PatternSignature::for_array_of_bytes<"e8">()))
					  i++;
				  return i != 7;
			  })
			  .add(1)
			  .relative_to_absolute()
			  .BCRL_EXPECT(decltype(predict_grenade), predict_grenade);
	remove_entity
		= convar_callback
			  .clone()
			  .repeater([](auto& ptr) {
				  ptr.next_instruction();
				  static int i = 0;
				  if (ptr.does_match(SignatureScanner::PatternSignature::for_array_of_bytes<"e8">()))
					  i++;
				  return i != 8;
			  })
			  .add(1)
			  .relative_to_absolute()
			  .BCRL_EXPECT(decltype(remove_entity), remove_entity);
}

void GrenadePrediction::calculate_grenade_prediction()
{
	const std::lock_guard lock{ mutex };
	points.clear();
	if (!enabled.get())
		return;

	CSPlayerPawn* player = Memory::local_player;
	if (!player)
		return;

	CSPlayerWeaponServices* weapon_services = player->weapon_services();
	if (!weapon_services)
		return;

	BasePlayerWeapon* weapon = weapon_services->active_weapon().get();
	if (!weapon)
		return;

	auto* grenade = weapon->entity_cast<BaseCSGrenade*>();
	if (!grenade || !grenade->pin_pulled())
		return;

	const int grenade_type = grenade->get_grenade_type();

	auto* predictor = RetAddrSpoofer::invoke(make_grenade_predictor, 30.0F, grenade_type, grenade);

	glm::vec3 position;
	glm::vec3 velocity;

	RetAddrSpoofer::invoke(calculate_initial_state, weapon, static_cast<BaseEntity*>(player), &position, &velocity, predict_jumpthrow.get());
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
	if (!enabled.get())
		return;

	const std::lock_guard lock{ mutex };
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
