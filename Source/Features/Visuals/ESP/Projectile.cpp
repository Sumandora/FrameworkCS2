#include "Projectile.hpp"

#include "../../Setting.hpp"
#include "../../Settings/MetaSetting.hpp"

#include "../../../SDK/Entities/BaseCSGrenadeProjectile.hpp"
#include "../../../SDK/Entities/CSPlayerController.hpp"
#include "../../../SDK/GameClass/Localize.hpp"

#include "../../../Interfaces.hpp"

#include "GenericESP/Element/SidedElement.hpp"
#include "GenericESP/UnionedRect.hpp"

#include "imgui.h"
#include "imgui_internal.h"

#include <string>
#include <utility>

ESPProjectile::ESPProjectile(SettingsHolder* parent, std::string name, const char* localization_key)
	: MetaSetting(parent, std::move(name))
	, localization_key(localization_key)
{
	thrower.side.set(GenericESP::Side::BOTTOM);
}

bool ESPProjectile::is_enabled() const
{
	return box_enabled.get() || name_enabled.get() || thrower_enabled.get();
}

void ESPProjectile::draw_projectile(ImDrawList* draw_list, BaseCSGrenadeProjectile* projectile, const ImRect& screenspace_rect) const
{
	GenericESP::UnionedRect unioned_rect{ screenspace_rect };

	if (box_enabled.get())
		box.draw(draw_list, projectile, unioned_rect);

	if (name_enabled.get()) {
		const std::string name = Interfaces::localize->find_safe(localization_key);
		this->name.draw(draw_list, projectile, name, unioned_rect);
	}

	if (thrower_enabled.get()) {
		const CSPlayerPawn* thrower = projectile->thrower().get();
		if (thrower) {
			const CSPlayerController* controller = thrower->original_controller().get();
			if (controller)
				this->thrower.draw(draw_list, projectile, controller->get_decorated_player_name(), unioned_rect);
		}
	}
}
