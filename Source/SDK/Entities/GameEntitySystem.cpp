#include "GameEntitySystem.hpp"

#include <BCRL.hpp>
#include <cstdio>
#include <RetAddrSpoofer.hpp>

GameEntitySystem::GameEntitySystem()
{
	// To the valve employee, who wrote this function: Kuss auf Nuss!
	auto /*CPrediction::*/ reinitPredictables = BCRL::Session::module("libclient.so")
													.nextStringOccurrence("CL:  reinitialized %i predictable entities\n")
													.findXREFs("libclient.so", true, false)
													.prevByteOccurrence("48 83 ec ? 4c 8d 35")
													.add(7);

	printf("ReinitPredictables at %p\n", reinitPredictables.expect("Can't find ReinitPredictables"));

	thisPtrPtr = static_cast<void**>(reinitPredictables.relativeToAbsolute().expect("Couldn't find GameEntitySystem"));
	printf("Found GameEntitySystem pointer at: %p\n", thisPtrPtr);
	reinitPredictables = reinitPredictables.add(4).nextByteOccurrence("49 8b 3e e8").add(4);
	getHighestEntityIndexPtr = reinitPredictables.relativeToAbsolute().expect("Couldn't find getHighestEntityIndex");
	printf("Found getHighestEntityIndex at: %p\n", getHighestEntityIndexPtr);
	reinitPredictables = reinitPredictables.add(4).nextByteOccurrence("3e 44 89 fe e8").add(5);
	getBaseEntityPtr = reinitPredictables.relativeToAbsolute().expect("Couldn't find getBaseEntity");
	printf("Found getBaseEntity at: %p\n", getBaseEntityPtr);
}

int GameEntitySystem::getHighestEntityIndex() const
{
	return RetAddrSpoofer::invoke<int, void*>(getHighestEntityIndexPtr, *thisPtrPtr);
}

BaseEntity* GameEntitySystem::getBaseEntity(const int index) const
{
	return RetAddrSpoofer::invoke<BaseEntity*, void*, int>(getBaseEntityPtr, *thisPtrPtr, index);
}
