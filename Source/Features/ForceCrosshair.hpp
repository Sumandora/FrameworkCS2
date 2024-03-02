#pragma once

#include "Feature.hpp"

namespace Features {
	class ForceCrosshair : public Feature {
		Config::Setting<bool> enabled{ false };

	public:
		ForceCrosshair();

		bool shouldForce();
		void imguiRender();

		NLOHMANN_DEFINE_TYPE_INTRUSIVE(ForceCrosshair, enabled)
	};
}