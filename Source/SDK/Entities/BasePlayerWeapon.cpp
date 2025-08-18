#include "BasePlayerWeapon.hpp"

#include "../Econ/ItemDefinition.hpp"
#include "../GameClass/Localize.hpp"
#include "../Schema/SchemaClassInfo.hpp"
#include "EntityIdentity.hpp"

#include "../../Interfaces.hpp"

#include <string>

std::string BasePlayerWeapon::get_display_name() const
{
	EconItemDefinition* econ_item_definition = attribute_manager().item().get_item_definition();
	if (econ_item_definition && econ_item_definition->internal_name())
		return Interfaces::localize->find_safe(econ_item_definition->internal_name());

	if (EntityIdentity* identity = entity(); identity)
		return identity->designer_name().string();

	// urgh... well better than nothing...
	return getSchemaType()->className;
}
