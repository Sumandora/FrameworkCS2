#include "GameEntitySystem.hpp"

#include "../../Memory.hpp"
#include "../../Utils/Logging.hpp"

#include "BCRL/SearchConstraints.hpp"
#include "BCRL/Session.hpp"

#include "SignatureScanner/PatternSignature.hpp"
#include "SignatureScanner/XRefSignature.hpp"

int (*GameEntitySystem::get_highest_entity_index_ptr)(GameEntitySystem* thisptr) = nullptr;
BaseEntity* (*GameEntitySystem::get_base_entity_ptr)(GameEntitySystem* thisptr, int index) = nullptr;

GameEntitySystem** GameEntitySystem::find()
{
	// To the valve employee, who wrote this function: Kuss auf Nuss!
	auto reinit_predictables = BCRL::signature(Memory::mem_mgr, SignatureScanner::PatternSignature::for_literal_string<"CL:  reinitialized %i predictable entities\n">(), BCRL::everything(Memory::mem_mgr).thats_readable().with_name("libclient.so"))
								   .find_xrefs(SignatureScanner::XRefTypes::relative(), BCRL::everything(Memory::mem_mgr).thats_readable().thats_executable().with_name("libclient.so"))
								   .sub(3)
								   .filter([](const auto& ptr) {
									   return ptr.does_match(SignatureScanner::PatternSignature::for_array_of_bytes<"48 8d 15">());
								   })
								   .prev_signature_occurrence(SignatureScanner::PatternSignature::for_array_of_bytes<"48 83 ec ? 4c 8d 35">())
								   .add(7);

	Logging::info("ReinitPredictables at: {}", reinit_predictables.expect<void*>("Can't find ReinitPredictables"));

	auto** game_entity_system = reinit_predictables
									.clone()
									.relative_to_absolute()
									.expect<GameEntitySystem**>("Couldn't find GameEntitySystem");
	Logging::info("Found GameEntitySystem at: {}", game_entity_system);
	reinit_predictables = reinit_predictables
							  .add(4)
							  .next_signature_occurrence(SignatureScanner::PatternSignature::for_array_of_bytes<"49 8b 3e e8">())
							  .add(4);
	get_highest_entity_index_ptr = reinit_predictables
														 .clone()
														 .relative_to_absolute()
														 .expect<decltype(get_highest_entity_index_ptr)>("Couldn't find getHighestEntityIndex");
	Logging::info("Found getHighestEntityIndex at: {}", get_highest_entity_index_ptr);
	reinit_predictables = reinit_predictables
							  .add(4)
							  .next_signature_occurrence(SignatureScanner::PatternSignature::for_array_of_bytes<"3e 44 89 fe e8">())
							  .add(5);
	get_base_entity_ptr = reinit_predictables
												.clone()
												.relative_to_absolute()
												.expect<decltype(get_base_entity_ptr)>("Couldn't find getBaseEntity");
	Logging::info("Found getBaseEntity at: {}", get_base_entity_ptr);

	return game_entity_system;
}

GameEntitySystem* GameEntitySystem::the()
{
	static GameEntitySystem** game_entity_system = find();
	return *game_entity_system;
}
