#include "Removals.hpp"

Removals::Removals()
	: Feature("Visuals", "Removals")
{
}

bool Removals::should_remove_legs() const {
	return remove_legs.get();
}
