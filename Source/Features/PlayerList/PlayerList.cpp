#include "PlayerList.hpp"

#include "../Feature.hpp"

#include "../../GUI/GUI.hpp"

#include "../../SDK/Entities/CSPlayerController.hpp"
#include "../../SDK/Entities/GameEntitySystem.hpp"
#include "../../SDK/EntityHandle.hpp"

#include "Columns.hpp"
#include "Columns/CompetitiveRankingColumn.hpp"
#include "Columns/CompetitiveWinsColumn.hpp"
#include "Columns/HealthColumn.hpp"
#include "Columns/NameColumn.hpp"
#include "Columns/ObserveeColumn.hpp"
#include "Columns/ObserverModeColumn.hpp"
#include "Columns/PingColumn.hpp"
#include "Columns/TeamColumn.hpp"
#include "Columns/TeamDamageColumn.hpp"
#include "Columns/TeamKillsColumn.hpp"

#include "imgui.h"
#include "imgui_internal.h"

#include <cfloat>
#include <cstddef>
#include <map>
#include <mutex>
#include <ranges>
#include <tuple>
#include <utility>

using Columns
	= std::tuple<
		NameColumn,
		HealthColumn,
		TeamColumn,
		CompetitiveWinsColumn,
		CompetitiveRankingColumn,
		CompetitiveRankingAfterWinColumn,
		CompetitiveRankingAfterLossColumn,
		CompetitiveRankingAfterTieColumn,
		TeamDamageColumn,
		TeamKillsColumn,
		ObserveeColumn,
		ObserverModeColumn,
		PingColumn>;

struct PlayerData {
	Columns columns;
	bool invalid;
};

static std::map<EntityHandle<CSPlayerController>, PlayerData> player_data_map;

PlayerList::PlayerList()
	: Feature("Intelligence", "Player list")
{
	TeamDamageColumn::resolve_convars();
}

template <typename T, T... Ints>
static void create_columns(std::integer_sequence<T, Ints...> /*nums*/)
{
	(ImGui::TableSetupColumn(std::tuple_element_t<Ints, Columns>::NAME,
		 is_hidden_by_default<std::tuple_element_t<Ints, Columns>>()
			 ? ImGuiTableColumnFlags_DefaultHide
			 : 0),
		...);
}

static void update_visible(auto& column)
{
	if constexpr (ColumnHasEnabled<decltype(column)>) {
		const bool enabled = ImGui::TableGetColumnFlags() & ImGuiTableColumnFlags_IsEnabled;
		column.enabled = enabled;
	}
}

std::size_t PlayerList::render_player_list()
{
	std::size_t count = 0;
	if (ImGui::BeginTable("##Player list", std::tuple_size_v<Columns>,
			ImGuiTableFlags_Resizable
				| ImGuiTableFlags_Reorderable
				| ImGuiTableFlags_Hideable
				| ImGuiTableFlags_SizingStretchProp
				| ImGuiTableFlags_Borders)) {
		create_columns(std::make_index_sequence<std::tuple_size_v<Columns>>{});
		ImGui::TableHeadersRow();

		{
			const std::lock_guard guard{ mutex };
			for (PlayerData& data : player_data_map | std::ranges::views::values) {
				ImGui::TableNextRow();
				std::apply([](auto&... column) {
					((ImGui::TableNextColumn(), update_visible(column), column.fill()), ...);
				},
					data.columns);
			}
			count = player_data_map.size();
		}

		ImGui::EndTable();
	}
	return count;
}

void PlayerList::invalidate_caches()
{
	const std::lock_guard guard{ mutex };
	player_data_map.clear();
}

static void update_column(auto& column, CSPlayerController* controller, CSPlayerPawn* pawn)
{
	if constexpr (UpdateColoumnWithController<decltype(column)>) {
		column.update(controller);
	}
	if constexpr (UpdateColoumnWithPawn<decltype(column)>) {
		column.update(pawn);
	}
};

void PlayerList::update()
{
	const std::lock_guard guard{ mutex };

	GameEntitySystem* entity_system = GameEntitySystem::the();

	if (!entity_system) {
		player_data_map.clear();
		return;
	}

	for (auto& [handle, data] : player_data_map) {
		data.invalid = true;
	}

	for (CSPlayerController* controller : entity_system->entities_of_type<CSPlayerController>()) {
		PlayerData& data = player_data_map[controller->get_handle()];
		CSPlayerPawn* pawn = controller->player_pawn().get();

		data.invalid = false;
		std::apply([controller, pawn](auto&... columns) { (update_column(columns, controller, pawn), ...); }, data.columns);
	}

	std::erase_if(player_data_map, [](const auto& pair) { return pair.second.invalid; });
}

static void update_column(auto& column, CSPlayerPawn* pawn, GameEvent* event)
{
	if constexpr (UpdateColoumnWithGameEvent<decltype(column)>) {
		column.update(pawn, event);
	}
}

void PlayerList::event_handler(GameEvent* event)
{
	const std::lock_guard guard{ mutex };

	GameEntitySystem* entity_system = GameEntitySystem::the();

	if (!entity_system)
		// Don't clear the entries, this function doesn't need to concern itself with that
		return;

	for (CSPlayerController* controller : entity_system->entities_of_type<CSPlayerController>()) {
		PlayerData& data = player_data_map[controller->get_handle()];
		CSPlayerPawn* pawn = controller->player_pawn().get();

		std::apply([event, pawn](auto&... columns) { (update_column(columns, pawn, event), ...); }, data.columns);
	}
}

void PlayerList::draw()
{
	if (!draw_overlay_player_list.get())
		return;

	static constexpr ImGuiWindowFlags MENU_CLOSED_FLAGS
		= ImGuiWindowFlags_NoInputs
		| ImGuiWindowFlags_NoMove
		| ImGuiWindowFlags_NoDecoration
		| ImGuiWindowFlags_NoNav
		| ImGuiWindowFlags_NoCollapse
		| ImGuiWindowFlags_NoScrollWithMouse
		| ImGuiWindowFlags_NoFocusOnAppearing;

	static constexpr ImGuiWindowFlags MENU_OPEN_FLAGS
		= (ImGuiWindowFlags_NoDecoration
			  | ImGuiWindowFlags_NoNav
			  | ImGuiWindowFlags_NoCollapse
			  | ImGuiWindowFlags_NoScrollWithMouse
			  | ImGuiWindowFlags_NoFocusOnAppearing)
		// NoDecoration includes NoResize, which is undesirable here.
		& ~ImGuiWindowFlags_NoResize;

	// Prevent user from resizing vertically, this is done automatically for them.
	ImGui::SetNextWindowSizeConstraints(
		{},
		{ FLT_MAX, FLT_MAX },
		+[](ImGuiSizeCallbackData* data) { data->DesiredSize.y = data->CurrentSize.y; });

	if (ImGui::Begin("Player list", nullptr, GUI::is_menu_open() ? MENU_OPEN_FLAGS : MENU_CLOSED_FLAGS)) {
		static std::size_t last_size = 0;
		const std::size_t new_size = render_player_list();

		if (new_size != last_size)
			// auto resize height
			ImGui::SetWindowSize({ ImGui::GetWindowSize().x, 0.0F });

		last_size = new_size;
	}

	if (!GUI::is_menu_open())
		ImGui::BringWindowToDisplayBack(ImGui::GetCurrentWindow());

	ImGui::End();
}
