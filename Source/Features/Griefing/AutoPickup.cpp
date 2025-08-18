#include "AutoPickup.hpp"

#include "../Feature.hpp"

#include "../../SDK/CUtl/Vector.hpp"
#include "../../SDK/Econ/EconItemView.hpp"
#include "../../SDK/Econ/ItemDefinition.hpp"
#include "../../SDK/Entities/BaseEntity.hpp"
#include "../../SDK/Entities/BasePlayerWeapon.hpp"
#include "../../SDK/Entities/CSPlayerPawn.hpp"
#include "../../SDK/Entities/CSWeaponBase.hpp"
#include "../../SDK/Entities/EntityIdentity.hpp"
#include "../../SDK/Entities/GameEntitySystem.hpp"
#include "../../SDK/Entities/Services/CSPlayerWeaponServices.hpp"
#include "../../SDK/Entities/VData/CSWeaponBaseVData.hpp"
#include "../../SDK/EntityHandle.hpp"
#include "../../SDK/GameClass/CSGameRules.hpp"
#include "../../SDK/GameClass/EngineToClient.hpp"
#include "../../SDK/GameClass/GameTypes.hpp"
#include "../../SDK/GameClass/GlobalVars.hpp"
#include "../../SDK/GameClass/Localize.hpp"

#include "../../Interfaces.hpp"
#include "../../Memory.hpp"

#include "../../Notifications/Notifications.hpp"

#include "BCRL/SearchConstraints.hpp"
#include "BCRL/Session.hpp"
#include "SignatureScanner/PatternSignature.hpp"
#include "SignatureScanner/XRefSignature.hpp"

#include <array>
#include <cstdint>
#include <format>
#include <string>

static CSGameRules** game_rules = nullptr;
static bool (*do_game_rules_prevent_pickup)(CSGameRules*) = nullptr;

AutoPickup::AutoPickup()
	: Feature("Griefing", "Auto pickup")
{
	auto collect_ground_weapons_session
		= BCRL::signature(
			Memory::mem_mgr,
			SignatureScanner::PatternSignature::for_literal_string<"multiple-ground-weapons">(),
			BCRL::everything(Memory::mem_mgr).with_flags("r--").with_name("libclient.so"))
			  .find_xrefs(
				  SignatureScanner::XRefTypes::relative(),
				  BCRL::everything(Memory::mem_mgr).with_flags("r-x").with_name("libclient.so"))
			  .prev_signature_occurrence(SignatureScanner::PatternSignature::for_array_of_bytes<"55 48 89 e5">());

	game_rules
		= collect_ground_weapons_session
			  .clone()
			  .next_signature_occurrence(SignatureScanner::PatternSignature::for_array_of_bytes<"48 8d 1d">())
			  .add(3)
			  .relative_to_absolute()
			  .BCRL_EXPECT(CSGameRules**, game_rules);

	// TODO eventually I would like this rebuild, but it seems quite fragile so I won't do that yet
	do_game_rules_prevent_pickup
		= collect_ground_weapons_session
			  .clone()
			  .repeater([](auto& ptr) {
				  ptr.next_instruction();
				  static int i = 0;
				  if (ptr.does_match(SignatureScanner::PatternSignature::for_array_of_bytes<"e8">()))
					  i++;
				  return i != 3;
			  })
			  .add(1)
			  .relative_to_absolute()
			  .BCRL_EXPECT(decltype(do_game_rules_prevent_pickup), do_game_rules_prevent_pickup);
}

void AutoPickup::update()
{
	if (!enabled.get())
		return;

	if (!Memory::local_player)
		return;

	if (!Memory::local_player->in_buy_zone())
		return;

	// The following three checks are checks that the game does in its collect_ground_weapons code.
	if (!game_rules)
		return;

	if (Interfaces::game_types->unk() != 0)
		return;

	if (do_game_rules_prevent_pickup(*game_rules))
		return;

	const CSPlayerWeaponServices* weapon_services = Memory::local_player->weapon_services();
	if (!weapon_services)
		return;

	std::array<int, GEAR_SLOT_COUNT> price_per_slot{};

	const UtlVector<EntityHandle<BasePlayerWeapon>>& weapons = weapon_services->my_weapons();
	for (int i = 0; i < weapons.size; i++) {
		BasePlayerWeapon* weapon = weapons.elements[i].get();
		if (!weapon)
			continue;

		const auto* vdata = static_cast<CSWeaponBaseVData*>(weapon->get_vdata());
		if (!vdata)
			continue;

		price_per_slot[vdata->gear_slot()] = vdata->price();
	}

	int best_price = 0;
	CSWeaponBase* best_weapon = nullptr;

	// TODO The GameEntitySystem should at some point also offer using entity_cast for the type check so that derivatives are also fine.
	for (BaseEntity* entity : GameEntitySystem::the()->entities()) {
		auto* weapon = entity->entity_cast<CSWeaponBase*>();
		if (!weapon)
			continue;

		if (weapon->most_recent_team_number() != Memory::local_player->team_id())
			continue;

		if (!weapon->dropped_near_buy_zone())
			continue;

		// This number is unfortunately hard coded in the game, it is used inside a function that's near the code collecting all entities.
		// If you need to update this number in the future (which I hope you won't), then look at the sv_allow_ground_weapon_pickup convar.
		static constexpr float DROP_PROTECTION_TIME = 0.5F;
		if ((*Memory::globals)->current_time() - weapon->dropped_at_time() <= DROP_PROTECTION_TIME)
			continue;

		if (weapon->owner_entity().has_entity())
			continue;

		const auto* other_vdata = static_cast<CSWeaponBaseVData*>(weapon->get_vdata());

		if (!other_vdata)
			continue;

		if (other_vdata->price() <= best_price)
			continue;

		// TODO Grenades are allowed, but I would need to check the counts, so I'll do that later...
		if (other_vdata->gear_slot() != GEAR_SLOT_PISTOL && other_vdata->gear_slot() != GEAR_SLOT_RIFLE)
			continue;

		if (other_vdata->price() <= price_per_slot[other_vdata->gear_slot()])
			continue;

		best_price = other_vdata->price();
		best_weapon = weapon;
	}

	if (best_weapon) {
		EconItemView& view = best_weapon->attribute_manager().item();

		const std::uint16_t id = view.item_definition_index();
		// This command is hardcoded into the libserver.so. The client also hardcodes its use.
		// It doesn't show up in the console and in fact the game reports an error when using it.
		// Test it, go in a game and try pick up a weapon using the buy menu, the console will report an error.
		const std::string cmd = std::format("pickup_groundweapon {}", id);

		// Lets try pick it up
		Interfaces::engine->execute_client_cmd(cmd.c_str());

		// And inform the user that we tried:
		std::string name = best_weapon->get_display_name();

		// TODO since this is subject to ping, maybe the notification should only be printed every multiple of the ping?
		Notifications::create(auto_pickup->get_name() /* <- consistency right there*/,
			std::format("Attempted to pick up '{}'", name), Notifications::Severity::INFO);
	}
}
