#pragma once

#include <vector>
#include <memory>
#include "../Config/Setting.hpp"

namespace Features {
	class Feature {
		std::string name;

	protected:
		explicit Feature(std::string&& name) : name(std::move(name)) {}

	public:
		[[nodiscard]] const std::string& getName() const {
			return name;
		}

		virtual void to_json(nlohmann::json& nlohmann_json_j, const Feature& nlohmann_json_t) = 0;
		virtual void from_json(const nlohmann::json& nlohmann_json_j, Feature& nlohmann_json_t) = 0;
	};

}
