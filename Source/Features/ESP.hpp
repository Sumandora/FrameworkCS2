#pragma once

#include "Feature.hpp"
#include "imgui.h"

namespace Features {
	class ESP : public Feature {
	public:
		ESP();

		void drawEsp(ImDrawList* drawList);
		void imguiRender();

		// TODO
		friend void to_json(nlohmann::json& nlohmann_json_j, const ESP& nlohmann_json_t) {  }
		friend void from_json(const nlohmann::json& nlohmann_json_j, ESP& nlohmann_json_t) {  }
	};
}