#pragma once

#include "../Columns.hpp"

#include "../../../SDK/Entities/CSPlayerController.hpp"
#include "../../../SDK/Entities/Services/ActionTrackingServices.hpp"

#include "imgui.h"

#include <optional>
#include <utility>

#define BASIC_SCOREBOARD_COLUMN(ClassName, Name, Code)                                                                               \
	struct ClassName {                                                                                                               \
		static constexpr const char* NAME = Name;                                                                                    \
		static constexpr bool HIDDEN_BY_DEFAULT = true;                                                                              \
		std::optional<decltype(Code(std::declval<ActionTrackingServices*>()))> value;                                                \
                                                                                                                                     \
		void update(CSPlayerController* controller)                                                                                  \
		{                                                                                                                            \
			if (ActionTrackingServices* action_tracking_services = controller->action_tracking_services(); action_tracking_services) \
				value = Code(action_tracking_services);                                                                              \
		}                                                                                                                            \
                                                                                                                                     \
		void fill() const                                                                                                            \
		{                                                                                                                            \
			if (value.has_value())                                                                                                   \
				ImGui::Text("%d", value.value());                                                                                    \
		}                                                                                                                            \
	}

BASIC_SCOREBOARD_COLUMN(KillsColumn, "Kills", [](ActionTrackingServices* action_tracking_services) { return action_tracking_services->match_stats().kills(); });
BASIC_SCOREBOARD_COLUMN(DeathsColumn, "Deaths", [](ActionTrackingServices* action_tracking_services) { return action_tracking_services->match_stats().deaths(); });
BASIC_SCOREBOARD_COLUMN(AssistsColumn, "Assists", [](ActionTrackingServices* action_tracking_services) { return action_tracking_services->match_stats().assists(); });
BASIC_SCOREBOARD_COLUMN(DamageColumn, "Damage", [](ActionTrackingServices* action_tracking_services) { return action_tracking_services->match_stats().damage(); });
BASIC_SCOREBOARD_COLUMN(EquipmentValueColumn, "Equipment Value", [](ActionTrackingServices* action_tracking_services) { return action_tracking_services->match_stats().equipment_value(); });
BASIC_SCOREBOARD_COLUMN(MoneySavedColumn, "Money Saved", [](ActionTrackingServices* action_tracking_services) { return action_tracking_services->match_stats().money_saved(); });
BASIC_SCOREBOARD_COLUMN(KillRewardColumn, "Kill Reward", [](ActionTrackingServices* action_tracking_services) { return action_tracking_services->match_stats().kill_reward(); });
BASIC_SCOREBOARD_COLUMN(LiveTimeColumn, "Live Time", [](ActionTrackingServices* action_tracking_services) { return action_tracking_services->match_stats().live_time(); });
BASIC_SCOREBOARD_COLUMN(HeadShotKillsColumn, "Head Shot Kills", [](ActionTrackingServices* action_tracking_services) { return action_tracking_services->match_stats().head_shot_kills(); });
BASIC_SCOREBOARD_COLUMN(ObjectiveColumn, "Objective", [](ActionTrackingServices* action_tracking_services) { return action_tracking_services->match_stats().objective(); });
BASIC_SCOREBOARD_COLUMN(CashEarnedColumn, "Cash Earned", [](ActionTrackingServices* action_tracking_services) { return action_tracking_services->match_stats().cash_earned(); });
BASIC_SCOREBOARD_COLUMN(UtilityDamageColumn, "Utility Damage", [](ActionTrackingServices* action_tracking_services) { return action_tracking_services->match_stats().utility_damage(); });
BASIC_SCOREBOARD_COLUMN(EnemiesFlashedColumn, "Enemies Flashed", [](ActionTrackingServices* action_tracking_services) { return action_tracking_services->match_stats().enemies_flashed(); });

BASIC_SCOREBOARD_COLUMN(Enemy5KsColumn, "Enemy 5Ks", [](ActionTrackingServices* action_tracking_services) { return action_tracking_services->match_stats().enemy_5ks(); });
BASIC_SCOREBOARD_COLUMN(Enemy4KsColumn, "Enemy 4Ks", [](ActionTrackingServices* action_tracking_services) { return action_tracking_services->match_stats().enemy_4ks(); });
BASIC_SCOREBOARD_COLUMN(Enemy3KsColumn, "Enemy 3Ks", [](ActionTrackingServices* action_tracking_services) { return action_tracking_services->match_stats().enemy_3ks(); });
BASIC_SCOREBOARD_COLUMN(EnemyKnifeKillsColumn, "Enemy Knife Kills", [](ActionTrackingServices* action_tracking_services) { return action_tracking_services->match_stats().enemy_knife_kills(); });
BASIC_SCOREBOARD_COLUMN(EnemyTaserKillsColumn, "Enemy Taser Kills", [](ActionTrackingServices* action_tracking_services) { return action_tracking_services->match_stats().enemy_taser_kills(); });

BASIC_SCOREBOARD_COLUMN(NumRoundKillsColumn, "Round Kills", [](ActionTrackingServices* action_tracking_services) { return action_tracking_services->round_kills(); });
BASIC_SCOREBOARD_COLUMN(NumRoundKillsHeadshotsColumn, "Round Kills Headshots", [](ActionTrackingServices* action_tracking_services) { return action_tracking_services->round_kills_headshots(); });
BASIC_SCOREBOARD_COLUMN(TotalRoundDamageDealtColumn, "Total Round Damage Dealt", [](ActionTrackingServices* action_tracking_services) { return action_tracking_services->total_round_damage_dealt(); });

static_assert(Column<KillsColumn>);
static_assert(Column<DeathsColumn>);
static_assert(Column<AssistsColumn>);
static_assert(Column<DamageColumn>);
static_assert(Column<EquipmentValueColumn>);
static_assert(Column<MoneySavedColumn>);
static_assert(Column<KillRewardColumn>);
static_assert(Column<LiveTimeColumn>);
static_assert(Column<HeadShotKillsColumn>);
static_assert(Column<ObjectiveColumn>);
static_assert(Column<CashEarnedColumn>);
static_assert(Column<UtilityDamageColumn>);
static_assert(Column<EnemiesFlashedColumn>);
static_assert(Column<Enemy5KsColumn>);
static_assert(Column<Enemy4KsColumn>);
static_assert(Column<Enemy3KsColumn>);
static_assert(Column<EnemyKnifeKillsColumn>);
static_assert(Column<EnemyTaserKillsColumn>);
static_assert(Column<NumRoundKillsColumn>);
static_assert(Column<NumRoundKillsHeadshotsColumn>);
static_assert(Column<TotalRoundDamageDealtColumn>);
