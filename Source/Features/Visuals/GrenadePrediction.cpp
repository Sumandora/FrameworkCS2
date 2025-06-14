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

GrenadePrediction::GrenadePrediction()
	: Feature("Visuals", "Grenade prediction")
{
}

static std::vector<ImVec2> points;

void GrenadePrediction::cache_initial_state()
{
	points.clear();
	if (!enabled.get())
		return;

	static auto* get_player = BCRL::signature(
		Memory::mem_mgr,
		SignatureScanner::PatternSignature::for_array_of_bytes<"55 31 FF 48 89 E5 41 54 48 83 EC 08 E8 ? ? ? ? 48 85 C0 74 ? 49 89 C4 48 8B 00 4C 89 E7 FF 90 ? ? ? ? 84 C0 74 ? 4C 89 E0">(),
		BCRL::everything(Memory::mem_mgr).thats_readable().thats_executable().with_name("libclient.so"))
								  .expect<void* (*)()>("");
	auto* player = static_cast<CSPlayerPawn*>(get_player());
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
	static auto* calculate_initial_state = BCRL::signature(
		Memory::mem_mgr,
		SignatureScanner::PatternSignature::for_array_of_bytes<"55 48 89 E5 41 57 49 89 FF 41 56 4C 8D 8D">(),
		BCRL::everything(Memory::mem_mgr).thats_readable().thats_executable().with_name("libclient.so"))
											   .expect<void* (*)(void* weapon, void* player, glm::vec3* position_out, glm::vec3* velocity_out, bool jumpthrow)>("");

	static auto* make_grenade_predictor = BCRL::signature(
		Memory::mem_mgr,
		SignatureScanner::PatternSignature::for_array_of_bytes<"55 48 8D 05 ? ? ? ? 48 89 E5 41 57 41 56 49 89 F6 41 55 4C 63 EF">(),
		BCRL::everything(Memory::mem_mgr).thats_readable().thats_executable().with_name("libclient.so"))
											  .expect<void* (*)(float time_to_render, int grenade_type, void* unk)>("");
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

	const int grenade_type = static_cast<BaseCSGrenade*>(weapon)->get_grenade_type();
	void* predictor = make_grenade_predictor(30.0F, grenade_type, nullptr);
	char position[512] = { 0 };
	char velocity[512] = { 0 };
	calculate_initial_state(weapon, player, reinterpret_cast<glm::vec3*>(position), reinterpret_cast<glm::vec3*>(velocity), predict_jumpthrow.get());
	predict_grenade(predictor, reinterpret_cast<glm::vec3*>(position), reinterpret_cast<glm::vec3*>(velocity));

	const int count = *(int*)((char*)predictor + 0xce8);
	for (int i = 0; i < count; i++) {
		struct Elem {
			int count;
			PADDING(4);
			glm::vec3* vecs;
			PADDING(4);
		};
		static_assert(sizeof(Elem) == 0x18);
		const Elem& elem = ((Elem*)(*(char**)((char*)predictor + 0xcf0)))[i];

		for (int j = 0; j < elem.count; j++) {
			const glm::vec3& v = elem.vecs[j];

			ImVec2 screen;
			if (!Projection::project(v, screen))
				continue;

			points.emplace_back(screen);
		}
	}
	remove_entity(predictor);
}

void GrenadePrediction::draw(ImDrawList* draw_list) const
{
	if (!enabled.get())
		return;

	static auto* get_player = BCRL::signature(
		Memory::mem_mgr,
		SignatureScanner::PatternSignature::for_array_of_bytes<"55 31 FF 48 89 E5 41 54 48 83 EC 08 E8 ? ? ? ? 48 85 C0 74 ? 49 89 C4 48 8B 00 4C 89 E7 FF 90 ? ? ? ? 84 C0 74 ? 4C 89 E0">(),
		BCRL::everything(Memory::mem_mgr).thats_readable().thats_executable().with_name("libclient.so"))
								  .expect<void* (*)()>("");
	auto* player = static_cast<CSPlayerPawn*>(get_player());
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

	// SchemaClassInfo* schema_type = player->getSchemaType();
	// if (schema_type != BaseCSGrenade::classInfo())
	// 	return;

	static auto* calculate_initial_state = BCRL::signature(
		Memory::mem_mgr,
		SignatureScanner::PatternSignature::for_array_of_bytes<"55 48 89 E5 41 57 49 89 FF 41 56 4C 8D 8D">(),
		BCRL::everything(Memory::mem_mgr).thats_readable().thats_executable().with_name("libclient.so"))
											   .expect<void (*)(void* weapon, void* player, glm::vec3* position_out, glm::vec3* velocity_out)>("");

	draw_list->AddPolyline(points.data(), (int)points.size(), -1, 0, 2.0F);
}
