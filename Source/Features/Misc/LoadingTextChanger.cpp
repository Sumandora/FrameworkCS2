#include "LoadingTextChanger.hpp"

#include "../Feature.hpp"

#include <string>

LoadingTextChanger::LoadingTextChanger()
	: Feature("Misc", "Loading text changer")
{
}

bool LoadingTextChanger::is_enabled() const
{
	return enabled.get();
}

const std::string& LoadingTextChanger::get_replacement() const
{
	return text.get();
}
