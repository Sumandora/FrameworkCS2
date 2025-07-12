#include "ModelChanger.hpp"

#include "../../../SDK/CUtl/SymbolLarge.hpp"
#include "../../../SDK/Entities/Components/BodyComponent.hpp"
#include "../../../SDK/Entities/Components/BodyComponentSkeletonInstance.hpp"
#include "../../../SDK/Entities/CSPlayerPawn.hpp"
#include "../../../SDK/GameClass/ModelState.hpp"
#include "../../../SDK/GameClass/NetworkClientService.hpp"
#include "../../../SDK/GameClass/NetworkGameClient.hpp"
#include "../../../SDK/GameClass/ResourceSystem.hpp"
#include "../../../SDK/GameClass/SkeletonInstance.hpp"

#include "../../Feature.hpp"

#include "../../../Memory.hpp"

#include "../../../Interfaces.hpp"

#include "BCRL/SearchConstraints.hpp"
#include "BCRL/Session.hpp"
#include "SignatureScanner/PatternSignature.hpp"

#include <cctype>
#include <cstddef>
#include <cstring>
#include <string>
#include <string_view>

static void (*set_model)(BaseEntity* entity, const char* model_name);

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
