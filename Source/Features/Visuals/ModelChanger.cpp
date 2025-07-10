#include "ModelChanger.hpp"

#include "../../SDK/CUtl/SymbolLarge.hpp"
#include "../../SDK/Entities/Components/BodyComponent.hpp"
#include "../../SDK/Entities/Components/BodyComponentSkeletonInstance.hpp"
#include "../../SDK/Entities/CSPlayerPawn.hpp"
#include "../../SDK/GameClass/Localize.hpp"
#include "../../SDK/GameClass/ModelState.hpp"
#include "../../SDK/GameClass/NetworkClientService.hpp"
#include "../../SDK/GameClass/NetworkGameClient.hpp"
#include "../../SDK/GameClass/ResourceSystem.hpp"
#include "../../SDK/GameClass/SkeletonInstance.hpp"

#include "../Feature.hpp"
#include "../Setting.hpp"

#include "../../Memory.hpp"

#include "../../Interfaces.hpp"

#include "../../Utils/Logging.hpp"

#include "BCRL/SearchConstraints.hpp"
#include "BCRL/Session.hpp"
#include "SignatureScanner/PatternSignature.hpp"

#include "imgui.h"

#include "../../SDK/Padding.hpp"

#include <algorithm>
#include <cctype>
#include <chrono>
#include <cstddef>
#include <cstring>
#include <filesystem>
#include <functional>
#include <string>
#include <string_view>
#include <utility>
#include <variant>
#include <vector>

static void (*set_model)(BaseEntity* entity, const char* model_name);

struct EconItemDefinition {
	PADDING(0x200);

public:
	const char* name;

	// TODO convert to struct with paddings
	OFFSET(const char*, internal_name, 0x70);
	OFFSET(const char*, type_name, 0x80);
	OFFSET(const char*, description, 0x90);
	OFFSET(const char*, inventory_image_path, 0xa8);
	OFFSET(const char*, model_path, 0xD8);
};

static_assert(offsetof(EconItemDefinition, name) == 0x200);

struct ItemSchema {
private:
	PADDING(0x108);

public:
	struct ItemEntry {
		PADDING(0x8);
		EconItemDefinition* econ_item_definition;
		PADDING(0x8);
	};
	static_assert(sizeof(ItemEntry) == 0x18);
	ItemEntry* items;

private:
	PADDING(0x10);

public:
	int count;
};

static_assert(offsetof(ItemSchema, items) == 0x108);
static_assert(offsetof(ItemSchema, count) == 0x120);

static void gather_default_models(std::vector<PlayerModelCombo::DefaultModel>& player_models)
{
	struct ItemSystem {
	private:
		PADDING(0x8);

	public:
		ItemSchema* item_schema;
	};

	// TODO garbage sig.
	static auto* item_system
		= BCRL::signature(
			Memory::mem_mgr,
			SignatureScanner::PatternSignature::for_array_of_bytes<"48 89 1D ? ? ? ? 48 89 13">(),
			BCRL::everything(Memory::mem_mgr).thats_readable().thats_not_writable().thats_executable().with_name("libclient.so"))
			  .add(3)
			  .relative_to_absolute()
			  .expect<ItemSystem**>("Couldn't find Item Schema");

	ItemSchema* schema = (*item_system)->item_schema;

	for (int i = 0; i < schema->count; i++) {
		EconItemDefinition* item = schema->items[i].econ_item_definition;
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

		const char* localized_name = Interfaces::localize->translate(item->internal_name());
		// TODO How should I deal with the unlocalized models? I think those are mainly map based ones, but I would still like to offer the user the option to force themselves to them...

		player_models.emplace_back(localized_name, item->model_path());
	}
}

static void gather_custom_models(std::vector<std::string>& player_models)
{
	static const std::filesystem::path BASE{ "../../csgo" }; // urgh... TODO

	static const std::function<void(std::filesystem::path)> RECURSE_MODELS = [&player_models](const std::filesystem::path& path) {
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

	player_model = default_models.at(0);
}

const std::string& PlayerModelCombo::get_player_model_path() const
{
	if (const auto* model = std::get_if<DefaultModel>(&player_model)) {
		return model->model_path;
	}
	if (const auto* model = std::get_if<std::string>(&player_model)) {
		return *model;
	}
	std::unreachable();
}

const std::string& PlayerModelCombo::get_player_model_name() const
{
	if (const auto* model = std::get_if<DefaultModel>(&player_model)) {
		return model->agent_name;
	}
	if (const auto* model = std::get_if<std::string>(&player_model)) {
		return *model;
	}
	std::unreachable();
}

const std::string& PlayerModelCombo::get() const
{
	return get_player_model_path();
}

void PlayerModelCombo::render()
{
	// Don't update it every frame, that might result in a performance drop.
	using std::chrono::system_clock;
	static system_clock::time_point last_update = system_clock::now();
	const system_clock::time_point right_now = system_clock::now();
	if (right_now - last_update > std::chrono::seconds(5)) {
		// Maybe TODO: The correct way of doing this is inotify, but I'm not too sure if I want to use this in a context like this one...
		// No mutex is needed here because there is only one render thread.
		custom_models.clear();
		gather_custom_models(custom_models);
		last_update = right_now;
	}

	const std::string model_name = get_player_model_name();

	if (ImGui::BeginCombo(get_name().c_str(), model_name.c_str(), ImGuiComboFlags_None)) {
		const std::string& model_path = get_player_model_path();
		for (const DefaultModel& model : default_models) {
			const bool selected = model_path == model.model_path;
			if (ImGui::Selectable(model.agent_name.c_str(), selected))
				player_model = model;

			if (selected)
				ImGui::SetItemDefaultFocus();
		}
		for (const std::string& model : custom_models) {
			const bool selected = model_path == model;
			if (ImGui::Selectable(model.c_str(), selected))
				player_model = model;

			if (selected)
				ImGui::SetItemDefaultFocus();
		}
		ImGui::EndCombo();
	}
}

void PlayerModelCombo::serialize(nlohmann::json& output_json) const
{
	output_json = get_player_model_path();
}

void PlayerModelCombo::deserialize(const nlohmann::json& input_json)
{
	const std::string model_path = input_json.get<std::string>();
	if (auto it = std::ranges::find(
			default_models,
			model_path,
			[](const DefaultModel& model) { return model.model_path; });
		it != default_models.end()) {
		player_model = *it;
	} else {
		player_model = model_path;
	}
}

ModelChanger::ModelChanger()
	: Feature("Visuals", "Model changer")
{
	// TODO better signature
	set_model
		= BCRL::signature(
			Memory::mem_mgr,
			SignatureScanner::PatternSignature::for_array_of_bytes<"55 48 89 F2 48 89 E5 41 54 49 89 FC 48 8D 7D ? 48 83 EC 18 48 8D 05 ? ? ? ? 48 8B 30 48 8B 06">(),
			BCRL::everything(Memory::mem_mgr).thats_readable().thats_executable().with_name("libclient.so"))
			  .expect<decltype(set_model)>("Couldn't find set model function");
}

void ModelChanger::update_model()
{
	// https://www.unknowncheats.me/forum/counter-strike-2-a/578692-change-models.html
	// what would i do without you, bruhmoment21 :sob: :sob: <3

	if (!enabled.get())
		return;

	if (!Memory::local_player)
		return;

	const std::string& model_path = model.get();

	if (model_path.empty())
		return;

	if (Interfaces::network_client_service && Interfaces::network_client_service->network_game_client
			? Interfaces::network_client_service->network_game_client->delta_tick == -1
			: false)
		return; // urgh...

	const BodyComponent* body_component = Memory::local_player->body_component();
	const SkeletonInstance& skeleton = static_cast<const BodyComponentSkeletonInstance*>(body_component)->skeleton_instance();
	const ModelState& model_state = skeleton.model_state();

	if (!model_state.model())
		return;

	const UtlSymbolLarge& model_name = model_state.model_name();

	if (!model_name.is_valid())
		return;

	if (model_name.string() == model_path)
		return;

	Interfaces::resource_system->precache(model_path.c_str());
	set_model(Memory::local_player, model_path.c_str());
}
