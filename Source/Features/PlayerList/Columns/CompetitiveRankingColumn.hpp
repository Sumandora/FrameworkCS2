#pragma once

#include "../Columns.hpp"

#include "../../../SDK/Entities/CSPlayerController.hpp"
#include "../../../SDK/GameClass/Localize.hpp"

#include "../../../Interfaces.hpp"

#include "imgui.h"

#include <algorithm>
#include <array>
#include <bit>
#include <cstdint>
#include <format>
#include <optional>
#include <string>
#include <variant>

struct CompetitiveRankingColumn {
	static constexpr const char* NAME = "Competitive Ranking";
	int last_ranking;
	std::int8_t last_type;

	std::variant<std::monostate, std::string /* rank name */, int /* premier rating */> rank;

	void update(CSPlayerController* controller)
	{
		if (!std::holds_alternative<std::monostate>(rank)
			&& last_ranking == controller->competitive_ranking()
			&& last_type == controller->competitive_rank_type())
			return;

		last_ranking = controller->competitive_ranking();
		last_type = controller->competitive_rank_type();

		if (controller->competitive_ranking() == RANKTYPE_PREMIER) {
			rank = controller->competitive_ranking();
		} else {
			// Actual ranks:
			const std::string rank_name = std::format("RankName_{}", controller->competitive_ranking());
			const std::optional<std::string> localized = Interfaces::localize->try_localize(rank_name.c_str());

			if (localized.has_value()) {
				rank = localized.value();
				return;
			}

			// Rank does not exist. Placeholder with the number, so someone can fix it if they see it.
			rank = std::to_string(controller->competitive_ranking());
		}
	}

	struct RankVisitor {
		void operator()(std::monostate /*nothing*/) { }
		void operator()(const std::string& rank_name)
		{
			ImGui::Text("%s", rank_name.c_str());
		}

		// Refer to rating_emblem.js
		static int get_clamped_rating(int rating)
		{
			const int remapped_rating = rating / 5000;
			return std::clamp(remapped_rating, 0, 6);
		}

		void operator()(int premier_rating)
		{
			const int tier = get_clamped_rating(premier_rating);

			static constexpr std::array<ImColor, 7> TIER_TO_COLOR{
				// Refer to rating_emblem.css
				std::byteswap(0xb0c3d9ff),
				std::byteswap(0x8cc6ffff),
				std::byteswap(0x6a7dffff),
				std::byteswap(0xc166ffff),
				std::byteswap(0xf03cffff),
				std::byteswap(0xeb4b4bff),
				std::byteswap(0xffd700ff),
			};

			ImGui::TextColored(TIER_TO_COLOR[tier], "%d Premier Rating", premier_rating);
		}
	};

	void fill() const
	{
		std::visit(RankVisitor{}, rank);
	}
};

// TODO I would like to replace the following 3 columns with a generic implementation, but I feel like that would probably involve macros.

struct CompetitiveRankingAfterWinColumn {
	static constexpr const char* NAME = "Competitive Ranking After Win";
	static constexpr bool HIDDEN_BY_DEFAULT = true;
	int ranking;
	int delta;
	bool available;

	void update(CSPlayerController* controller)
	{
		available = controller->competitive_rank_type() == RANKTYPE_PREMIER;
		ranking = controller->competitive_ranking_predicted_win();
		delta = controller->competitive_ranking_predicted_win() - controller->competitive_ranking();
	}

	void fill() const
	{
		if (!available)
			ImGui::TextUnformatted("N/A");
		else
			ImGui::Text("%d (%+d)", ranking, delta);
	}
};

struct CompetitiveRankingAfterLossColumn {
	static constexpr const char* NAME = "Competitive Ranking After Loss";
	static constexpr bool HIDDEN_BY_DEFAULT = true;
	int ranking;
	int delta;
	bool available;

	void update(CSPlayerController* controller)
	{
		available = controller->competitive_rank_type() == RANKTYPE_PREMIER;
		ranking = controller->competitive_ranking_predicted_loss();
		delta = controller->competitive_ranking_predicted_loss() - controller->competitive_ranking();
	}

	void fill() const
	{
		if (!available)
			ImGui::TextUnformatted("N/A");
		else
			ImGui::Text("%d (%+d)", ranking, delta);
	}
};

struct CompetitiveRankingAfterTieColumn {
	static constexpr const char* NAME = "Competitive Ranking After Tie";
	static constexpr bool HIDDEN_BY_DEFAULT = true;
	int ranking;
	int delta;
	bool available;

	void update(CSPlayerController* controller)
	{
		available = controller->competitive_rank_type() == RANKTYPE_PREMIER;
		ranking = controller->competitive_ranking_predicted_tie();
		delta = controller->competitive_ranking_predicted_tie() - controller->competitive_ranking();
	}

	void fill() const
	{
		if (!available)
			ImGui::TextUnformatted("N/A");
		else
			ImGui::Text("%d (%+d)", ranking, delta);
	}
};

static_assert(Column<CompetitiveRankingColumn>);
static_assert(Column<CompetitiveRankingAfterWinColumn>);
static_assert(Column<CompetitiveRankingAfterLossColumn>);
static_assert(Column<CompetitiveRankingAfterTieColumn>);
