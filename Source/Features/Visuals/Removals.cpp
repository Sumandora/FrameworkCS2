#include "Removals.hpp"

#include "../Feature.hpp"

Removals::Removals()
	: Feature("Visuals", "Removals")
{
}

bool Removals::should_remove_legs() const
{
	return remove_legs.get();
}

bool Removals::should_remove_sleeves() const
{
	return remove_sleeves.get();
}
