#include "GrenadePrediction.hpp"

#include "../Feature.hpp"

#include "BCRL/SearchConstraints.hpp"
#include "BCRL/Session.hpp"

#include "../../Memory.hpp"

#include "../../Utils/Projection.hpp"

#include "glm/ext/vector_float3.hpp"

#include "imgui.h"

#include "SignatureScanner/PatternSignature.hpp"

#include "../../SDK/Entities/BaseCSGrenade.hpp"
#include "../../SDK/Entities/BasePlayerWeapon.hpp"
#include "../../SDK/Entities/CSPlayerPawn.hpp"
#include "../../SDK/Entities/Services/PlayerWeaponServices.hpp"
#include "../../SDK/Padding.hpp"

#include <cstddef>
#include <vector>

GrenadePrediction::GrenadePrediction()
	: Feature("Visuals", "Grenade prediction")
{
}

static std::vector<glm::vec3> points;

void GrenadePrediction::calculate_grenade_prediction()
{
	points.clear();
	if (!enabled.get())
		return;

	auto* player = Memory::local_player;
	if (!player)
		return;
	static auto* get_weapon = BCRL::signature(
		Memory::mem_mgr,
		SignatureScanner::PatternSignature::for_array_of_bytes<"55 48 89 E5 41 54 49 89 FC 48 83 EC 08 48 8B BF ? ? ? ? 48 85 FF 74 ? 4C 8B 65">(),
		BCRL::everything(Memory::mem_mgr).thats_readable().thats_executable().with_name("libclient.so"))
								  .expect<void* (*)(void* player)>("");
	auto* weapon = static_cast<BasePlayerWeapon*>(get_weapon(player));
	if (!weapon)
		return;

	auto* grenade = weapon->entity_cast<BaseCSGrenade*>();
	if(!grenade || !grenade->pin_pulled())
		return;

	static auto* calculate_initial_state = BCRL::signature(
		Memory::mem_mgr,
		SignatureScanner::PatternSignature::for_array_of_bytes<"55 48 89 E5 41 57 49 89 FF 41 56 4C 8D 8D">(),
		BCRL::everything(Memory::mem_mgr).thats_readable().thats_executable().with_name("libclient.so"))
											   .expect<void* (*)(void* weapon, void* player, glm::vec3* position_out, glm::vec3* velocity_out, bool jumpthrow)>("");

	static auto* make_grenade_predictor = BCRL::signature(
		Memory::mem_mgr,
		SignatureScanner::PatternSignature::for_array_of_bytes<"55 48 8D 05 ? ? ? ? 48 89 E5 41 57 41 56 49 89 F6 41 55 4C 63 EF">(),
		BCRL::everything(Memory::mem_mgr).thats_readable().thats_executable().with_name("libclient.so"))
											  .expect<struct GrenadePredictor* (*)(float time_to_render, int grenade_type, void* unk)>("");
	static auto* predict_grenade = BCRL::signature(
		Memory::mem_mgr,
		SignatureScanner::PatternSignature::for_array_of_bytes<"55 48 89 E5 41 57 41 56 41 55 49 89 D5 41 54 49 89 FC 53 48 83 EC 78">(),
		BCRL::everything(Memory::mem_mgr).thats_readable().thats_executable().with_name("libclient.so"))
									   .expect<void (*)(void* predictor, glm::vec3* position, glm::vec3* velocity)>("");
	static auto* remove_entity = BCRL::signature(
		Memory::mem_mgr,
		SignatureScanner::PatternSignature::for_array_of_bytes<"48 85 FF 0F 84 ? ? ? ? 55 48 89 E5 41 55 41 54 48 8B 47">(),
		BCRL::everything(Memory::mem_mgr).thats_readable().thats_executable().with_name("libclient.so"))
									 .expect<void (*)(void* entity)>("");

	const int grenade_type = grenade->get_grenade_type();

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

	GrenadePredictor* predictor = make_grenade_predictor(0.0F, grenade_type, nullptr);

	glm::vec3 position;
	glm::vec3 velocity;

	calculate_initial_state(weapon, player, &position, &velocity, predict_jumpthrow.get());
	predict_grenade(predictor, &position, &velocity);

	for (int i = 0; i < predictor->count; i++) {
		const GrenadePredictor::Element& elem = predictor->elements[i];

		for (int j = 0; j < elem.count; j++) {
			const glm::vec3& v = elem.vecs[j];
			points.emplace_back(v);
		}
	}
	remove_entity(predictor);
}

void GrenadePrediction::draw(ImDrawList* draw_list) const
{
	if (!enabled.get())
		return;

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
