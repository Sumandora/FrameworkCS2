#include "PlayerModelCombo.hpp"

#include "../../../SDK/CUtl/Buffer.hpp"
#include "../../../SDK/Econ/ItemDefinition.hpp"
#include "../../../SDK/Econ/ItemSchema.hpp"
#include "../../../SDK/Econ/ItemSystem.hpp"
#include "../../../SDK/GameClass/FileSystem.hpp"
#include "../../../SDK/GameClass/Localize.hpp"
#include "../../../SDK/GameClass/Source2Client.hpp"

#include "../../../Features/Setting.hpp"

#include "../../../Interfaces.hpp"

#include "../../../Utils/Interval.hpp"
#include "../../../Utils/Logging.hpp"
#include "../../../Utils/VTexDecoder.hpp"

#include "../../../GUI/TextureManager.hpp"

#include "imgui.h"

#include <algorithm>
#include <chrono>
#include <cstddef>
#include <cstring>
#include <filesystem>
#include <format>
#include <functional>
#include <optional>
#include <string>
#include <string_view>
#include <utility>
#include <vector>

static void gather_default_models(std::vector<PlayerModelCombo::DefaultModel>& player_models)
{
	EconItemSchema* schema = Interfaces::source2_client->get_econ_item_system()->item_schema;
	Logging::info("Found {} item definitions", schema->items.size);

	for (int i = 0; i < schema->items.size; i++) {
		EconItemDefinition* item = schema->items.elements[i].econ_item_definition;
		if (!item->type_name() || std::string_view{ item->type_name() } != "#Type_CustomPlayer")
			continue;

		const std::string_view sv{ item->model_path() };

		const bool is_default
			= sv == "customplayer_t_map_based"
			|| sv == "customplayer_ct_map_based";

		if (!is_default
			&& std::ranges::any_of(player_models,
				[&item](const PlayerModelCombo::DefaultModel& model) { return model.model_path == item->model_path(); }))
			continue; // Filter out invalid models that just link back to the default models. The map-based models are of course exempted from this.

		const std::string file_path = std::format("panorama/images/{}_square_png.vtex_c", item->inventory_image_path());

		UtlBuffer buf{ 0, 0, 0 };
		const bool suc = Interfaces::file_system->read_file(file_path.c_str(), "GAME", &buf, 0, 0);

		if (!suc) {
			Logging::warn("Failed to read player model image: {}", file_path);
			continue;
		}

		std::vector<char> vec;
		vec.resize(buf.memory.growSize);
		std::memcpy(vec.data(), buf.memory.memory, buf.memory.growSize);

		const GUI::TextureManager::RawImage image = VTexDecoder::decode(vec).value();

		std::optional<const char*> localized_name = Interfaces::localize->try_translate(item->internal_name());
		bool has_localization = true;
		if (!localized_name.has_value()) {
			localized_name = item->internal_name();
			has_localization = false;
		}

		player_models.emplace_back(localized_name.value(), item->model_path(), image, has_localization);
	}
}

static void gather_custom_models(std::vector<std::string>& player_models)
{
	static const std::filesystem::path BASE{ "../../csgo" }; // urgh... TODO

	static const std::function<void(std::filesystem::path)> RECURSE_MODELS = [&player_models](const std::filesystem::path& path) {
		if (!std::filesystem::exists(path))
			return;

		for (const std::filesystem::path& iter : std::filesystem::directory_iterator{ path }) {
			if (std::filesystem::is_directory(iter)) {
				RECURSE_MODELS(iter);
				continue;
			}

			if (!std::filesystem::is_regular_file(iter))
				continue;

			if (iter.extension() != ".vmdl_c")
				continue;

			std::string relative = std::filesystem::relative(iter, BASE).string();

			relative = relative.substr(0, relative.length() - 2 /* cut the '_c' off */);

			player_models.emplace_back(std::move(relative));
		}
	};

	RECURSE_MODELS(BASE / "characters/models/");
}

std::vector<PlayerModelCombo::DefaultModel> PlayerModelCombo::default_models{};
std::vector<std::string> PlayerModelCombo::custom_models{};

PlayerModelCombo::PlayerModelCombo(SettingsHolder* parent, std::string name)
	: Setting(parent, std::move(name))
{
	if (default_models.empty()) {
		gather_default_models(default_models);
	}

	if (custom_models.empty()) {
		gather_custom_models(custom_models);
	}

	// TODO should a theoretical case be handled in which the default models failed to get filled and we still somehow survived until here?

	player_model = default_models.at(0).model_path;
}

const std::string& PlayerModelCombo::get() const
{
	return player_model;
}

inline void PlayerModelCombo::draw_fancy_model_selection()
{
	bool first = true;

	for (const DefaultModel& model : default_models) {
		const bool selected = player_model == model.model_path;
		const ImTextureID texture_id = GUI::get_texture_manager().get_texture(model.image);

		static constexpr ImVec2 BUTTON_SIZE{ 100.0F, 100.0F };

		if (!first && ImGui::GetItemRectMax().x + ImGui::GetStyle().ItemSpacing.x + BUTTON_SIZE.x < ImGui::GetCursorScreenPos().x + ImGui::GetContentRegionAvail().x) {
			ImGui::SameLine();
		}
		first = false;

		if (selected)
			ImGui::PushStyleColor(ImGuiCol_Border, ImVec4{ 0.0F, 1.0F, 0.0F, 1.0F });

		ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, { 5.0F, 5.0F });
		if (ImGui::ImageButton(model.agent_name.c_str(), texture_id, BUTTON_SIZE))
			player_model = model.model_path;
		ImGui::PopStyleVar();

		if (selected)
			ImGui::PopStyleColor();

		// Sadly I have no idea what I should do with models that don't have a localization...
		// On one hand I would like to show the internal name, but I feel like it breaks the user experience a bit
		if (model.is_localized)
			ImGui::SetItemTooltip("%s", model.agent_name.c_str());
	}
}

void PlayerModelCombo::render()
{
	// Don't update it every frame, that might result in a performance drop.
	static Interval<std::chrono::seconds, 5> custom_model_reload_timer;
	if (custom_model_reload_timer.passed()) {
		// No mutex is needed here because there is only one render thread.
		custom_models.clear();
		gather_custom_models(custom_models);
	}

	if (custom_models.empty()) {
		draw_fancy_model_selection();
		return;
	}

	if (ImGui::BeginTabBar("Model types", ImGuiTabBarFlags_Reorderable)) {
		if (ImGui::BeginTabItem("Default models")) {
			draw_fancy_model_selection();
			ImGui::EndTabItem();
		}
		if (ImGui::BeginTabItem("Custom models")) {
			for (const std::string& model : custom_models) {
				const bool selected = player_model == model;
				if (selected)
					ImGui::PushStyleColor(ImGuiCol_Border, ImVec4{ 0.0F, 1.0F, 0.0F, 1.0F });
				if (ImGui::Button(model.c_str(), { ImGui::GetContentRegionAvail().x, 0.0F }))
					player_model = model;
				if (selected)
					ImGui::PopStyleColor();
			}
			ImGui::EndTabItem();
		}

		ImGui::EndTabBar();
	}
}

void PlayerModelCombo::serialize(nlohmann::json& output_json) const
{
	output_json = player_model;
}

void PlayerModelCombo::deserialize(const nlohmann::json& input_json)
{
	player_model = input_json;

	// TODO if this model no longer exists, perhaps we should just default to the first default model...
}
