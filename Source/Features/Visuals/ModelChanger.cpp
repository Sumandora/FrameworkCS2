#include "ModelChanger.hpp"

#include "../../SDK/CUtl/String.hpp"
#include "../../SDK/CUtl/SymbolLarge.hpp"
#include "../../SDK/CUtl/Vector.hpp"
#include "../../SDK/Entities/Components/BodyComponent.hpp"
#include "../../SDK/Entities/Components/BodyComponentSkeletonInstance.hpp"
#include "../../SDK/Entities/CSPlayerPawn.hpp"
#include "../../SDK/GameClass/FileSystem.hpp"
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

#include <array>
#include <cctype>
#include <chrono>
#include <cstddef>
#include <cstring>
#include <filesystem>
#include <functional>
#include <ranges>
#include <string>
#include <string_view>
#include <utility>
#include <vector>

static void (*set_model)(BaseEntity* entity, const char* model_name);

static void gather_default_models(std::vector<std::string>& player_models)
{
	// TODO: it would be beneficial if the econ item definitions were confronted instead, because then one could display the actual name of the model, not the internal path.
	UtlVector<UtlString> model_paths{};
	Interfaces::file_system->find_file_absolute_list(&model_paths, "characters/models/*", "GAME");
	for (int i = 0; i < model_paths.size; i++) {
		const std::string_view sv{ model_paths.elements[i] };
		if (!sv.starts_with("vpk:"))
			continue; // The rest are custom models, ignore those here.

		std::string path{ "characters/models/" };
		path += (sv.data() + sv.find_last_of('/') + 1);
		path += "/*.vmdl_c";

		UtlVector<UtlString> model_files{};
		Interfaces::file_system->find_file_absolute_list(&model_files, path.c_str(), "GAME");
		for (int j = 0; j < model_files.size; j++) {
			const std::string_view model_file_path{ model_files.elements[j] };
			if (!model_file_path.ends_with(".vmdl_c"))
				continue; // Why do I even get other files?

			std::string model_path = model_file_path.data() + model_file_path.find_last_of(':') + 1;

			model_path = model_path.substr(0, model_path.size() - 2 /* cut the '_c' off */);

			player_models.emplace_back(std::move(model_path));
		}
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

std::vector<std::string> PlayerModelCombo::default_models{};
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

const std::string& PlayerModelCombo::get() const
{
	return player_model;
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

	if (ImGui::BeginCombo(get_name().c_str(), player_model.c_str(), ImGuiComboFlags_None)) {
		for (const std::string& model : std::ranges::views::join(std::array{ default_models, custom_models })) {
			const bool selected = player_model == model;
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
	output_json = player_model;
}

void PlayerModelCombo::deserialize(const nlohmann::json& input_json)
{
	player_model = input_json;
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
