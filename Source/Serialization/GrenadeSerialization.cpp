#include "GrenadeSerialization.hpp"

#include "../Utils/Logging.hpp"

#include <string_view>
#include <vector>

auto Serialization::Grenades::parse_grenades_for_map(std::string_view map_name) -> std::vector<Grenade>
{
	const nlohmann::json grenades_json = Serialization::Grenades::parse_grenades();
	if (!grenades_json.contains(map_name)) {
		Logging::warn("No grenades found for map '{}'", map_name);
		return {};
	}

	const auto& grenades_for_map = grenades_json[map_name];
	Logging::info("Loading {} grenades for map '{}'", grenades_for_map.size(), map_name);

	return { grenades_for_map.begin(), grenades_for_map.end() };
}
