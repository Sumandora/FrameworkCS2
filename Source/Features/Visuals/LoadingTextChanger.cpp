#include "LoadingTextChanger.hpp"

#include "../Feature.hpp"

#include <string>

LoadingTextChanger::LoadingTextChanger()
	: Feature("Visuals", "Loading text changer")
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
