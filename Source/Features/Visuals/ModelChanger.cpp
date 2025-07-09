#include "ModelChanger.hpp"

#include "../../SDK/CUtl/BufferString.hpp"
#include "../../SDK/Entities/CSPlayerPawn.hpp" // IWYU pragma: keep
#include "../../SDK/GameClass/ResourceSystem.hpp"

#include "../../Memory.hpp"

#include "../Feature.hpp"

#include "../../Interfaces.hpp"

#include "BCRL/SearchConstraints.hpp"
#include "BCRL/Session.hpp"
#include "SignatureScanner/PatternSignature.hpp"

#include <string>

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

	const std::string& model_path = this->model_path.get();

	if (model_path.empty())
		return;

	// Precache every time? really?
	Interfaces::resource_system->precache(model_path.c_str());
	set_model(Memory::local_player, model_path.c_str());
}
