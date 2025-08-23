#pragma once

#include "../../Schema/FieldOffset.hpp"
#include "PlayerControllerComponent.hpp"
#include <cstdint>

struct CSPerRoundStats {
	CLASS_INFO("libclient.so", "CSPerRoundStats_t");

	SCHEMA_VAR(std::int32_t, kills, "m_iKills");
	SCHEMA_VAR(std::int32_t, deaths, "m_iDeaths");
	SCHEMA_VAR(std::int32_t, assists, "m_iAssists");
	SCHEMA_VAR(std::int32_t, damage, "m_iDamage");
	SCHEMA_VAR(std::int32_t, equipment_value, "m_iEquipmentValue");
	SCHEMA_VAR(std::int32_t, money_saved, "m_iMoneySaved");
	SCHEMA_VAR(std::int32_t, kill_reward, "m_iKillReward");
	SCHEMA_VAR(std::int32_t, live_time, "m_iLiveTime");
	SCHEMA_VAR(std::int32_t, head_shot_kills, "m_iHeadShotKills");
	SCHEMA_VAR(std::int32_t, objective, "m_iObjective");
	SCHEMA_VAR(std::int32_t, cash_earned, "m_iCashEarned");
	SCHEMA_VAR(std::int32_t, utility_damage, "m_iUtilityDamage");
	SCHEMA_VAR(std::int32_t, enemies_flashed, "m_iEnemiesFlashed");
};

struct CSMatchStats : public CSPerRoundStats {
	CLASS_INFO("libclient.so", "CSMatchStats_t");

	SCHEMA_VAR(std::int32_t, enemy_5ks, "m_iEnemy5Ks");
	SCHEMA_VAR(std::int32_t, enemy_4ks, "m_iEnemy4Ks");
	SCHEMA_VAR(std::int32_t, enemy_3ks, "m_iEnemy3Ks");
	SCHEMA_VAR(std::int32_t, enemy_knife_kills, "m_iEnemyKnifeKills");
	SCHEMA_VAR(std::int32_t, enemy_taser_kills, "m_iEnemyTaserKills");
};

struct ActionTrackingServices : public PlayerControllerComponent {
	CLASS_INFO("libclient.so", "CCSPlayerController_ActionTrackingServices");

	SCHEMA_VAR(CSMatchStats, match_stats, "m_matchStats");
	SCHEMA_VAR(std::int32_t, round_kills, "m_iNumRoundKills");
	SCHEMA_VAR(std::int32_t, round_kills_headshots, "m_iNumRoundKillsHeadshots");
	SCHEMA_VAR(std::uint32_t, total_round_damage_dealt, "m_unTotalRoundDamageDealt");
};
