#include "ModelChanger.hpp"

#include "../../../SDK/CUtl/SymbolLarge.hpp"
#include "../../../SDK/Entities/BaseModelEntity.hpp"
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
#include "SignatureScanner/XRefSignature.hpp"

#include "RetAddrSpoofer.hpp"

#include <cctype>
#include <cstddef>
#include <cstring>
#include <string>
#include <string_view>

static void (*set_model)(BaseModelEntity* entity, const char* model_name) = nullptr;

ModelChanger::ModelChanger()
	: Feature("Visuals", "Model changer")
{
	set_model
		= BCRL::signature(Memory::mem_mgr,
			SignatureScanner::PatternSignature::for_literal_string<"models/inventory_items/dogtags.vmdl">(),
			BCRL::everything(Memory::mem_mgr).with_flags("r*-").with_name("libclient.so"))
			  .find_xrefs(SignatureScanner::XRefTypes::relative(),
				  BCRL::everything(Memory::mem_mgr).with_flags("r-x").with_name("libclient.so"))
			  .add(8)
			  .relative_to_absolute()
			  .filter(BCRL::everything(Memory::mem_mgr).with_flags("r-x"))
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
	RetAddrSpoofer::invoke(set_model, static_cast<BaseModelEntity*>(Memory::local_player), model_path.c_str());
}
