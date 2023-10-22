#include "GameHook.hpp"

#include "../Features/Features.hpp"

const char* GameHook::GetFunLoading::hookFunc(char* fallback)
{
	const char* replacement = Features::LoadingTextChanger::getReplacement();
	if(!replacement)
		return RetAddrSpoofer::invoke<const char*>(hook->proxy, fallback);
	return replacement;
}