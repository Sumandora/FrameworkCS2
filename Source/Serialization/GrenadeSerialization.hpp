#pragma once

#include "glm/ext/vector_float2.hpp"
#include "glm/ext/vector_float3.hpp"

#include "magic_enum/magic_enum.hpp"

#include "nlohmann/json.hpp"
#include "nlohmann/json_fwd.hpp"

#include <cctype>
#include <cstddef>
#include <cstdint>
#include <optional>
#include <ranges>
#include <stdexcept>
#include <string>
#include <string_view>
#include <vector>

namespace Serialization::Grenades {
	enum class GrenadeWeapon : std::uint8_t {
		WEAPON_HEGRENADE = 0, // This are the same ids that BaseCSGrenade::get_grenade_type outputs.
		WEAPON_FLASHBANG = 1,
		WEAPON_MOLOTOV = 2,
		WEAPON_DECOY = 3,
		WEAPON_SMOKEGRENADE = 4,
	};

	template <typename BasicJsonType>
	inline void to_json(BasicJsonType& j, const GrenadeWeapon& e)
	{
		const auto name = magic_enum::enum_name(e) | std::ranges::views::transform([](char c) { return std::tolower(c); });
		j = std::string{ name.begin(), name.end() };
	}
	template <typename BasicJsonType>
	inline void from_json(const BasicJsonType& j, GrenadeWeapon& e)
	{
		const auto entries = magic_enum::enum_entries<GrenadeWeapon>();
		for (const auto& [e2, v] : entries) {
			const auto lowercased = v | std::ranges::views::transform([](char c) { return std::tolower(c); });
			const std::string lowercased_str{ lowercased.begin(), lowercased.end() };
			if (lowercased_str.c_str() == j) {
				e = e2;
				return;
			}
		}
		throw std::invalid_argument{ j.dump() };
	}

	struct Grenade {
		std::string description;
		bool duck = false;
		struct ThrowInfo {
		private:
			static constexpr bool DEFAULT_JUMP = false;
			static constexpr float DEFAULT_STRENGTH = 1.0F;
			static constexpr float DEFAULT_AIM_TOLERANCE = 1E-2F;

		public:
			bool jump = DEFAULT_JUMP;
			float strength = DEFAULT_STRENGTH;
			float aim_tolerance = DEFAULT_AIM_TOLERANCE;

			[[nodiscard]] bool should_serialize() const
			{
				return jump != DEFAULT_JUMP || strength != DEFAULT_STRENGTH || aim_tolerance != DEFAULT_AIM_TOLERANCE;
			}

			friend void to_json(nlohmann::json& json, const ThrowInfo& throw_info)
			{
				if (throw_info.jump != DEFAULT_JUMP)
					json["jump"] = throw_info.jump;
				if (throw_info.strength != DEFAULT_STRENGTH)
					json["strength"] = throw_info.strength;
				if (throw_info.aim_tolerance != DEFAULT_AIM_TOLERANCE)
					json["aim_tolerance"] = throw_info.aim_tolerance;
			}

			friend void from_json(const nlohmann::json& json, ThrowInfo& throw_info)
			{
				throw_info.jump = json.value("jump", DEFAULT_JUMP);
				throw_info.strength = json.value("strength", DEFAULT_STRENGTH);
				throw_info.aim_tolerance = json.value("aim_tolerance", DEFAULT_AIM_TOLERANCE);
			};
		} throw_info;
		struct Name {
			std::string from;
			std::string to;

			friend void to_json(nlohmann::json& json, const Name& name)
			{
				json = {
					name.from,
					name.to,
				};
			}

			friend void from_json(const nlohmann::json& json, Name& name)
			{
				name.from = json[0].get<std::string>();
				name.to = json[1].get<std::string>();
			};
		} name;
		glm::vec3 position;
		glm::vec2 viewangles;
		GrenadeWeapon weapon;

		// NOLINTNEXTLINE(modernize-use-constraints)
		friend void to_json(nlohmann::json& json, const Grenade& grenade)
		{
			if (!grenade.description.empty())
				json["description"] = grenade.description;
			if (grenade.duck)
				json["duck"] = grenade.duck;
			if (grenade.throw_info.should_serialize())
				json["grenade"] = grenade.throw_info;
			json["name"] = grenade.name;
			json["position"] = {
				grenade.position.x,
				grenade.position.y,
				grenade.position.z,
			};
			json["viewangles"] = {
				grenade.viewangles.x,
				grenade.viewangles.y,
			};
			json["weapon"] = grenade.weapon;
		}

		friend void from_json(const nlohmann::json& json, Grenade& grenade)
		{
			grenade.description = json.value("description", std::string{});
			grenade.duck = json.value("duck", false);
			grenade.throw_info = json.value("grenade", ThrowInfo{});
			grenade.name = json["name"];
			const auto& position = json["position"];
			grenade.position.x = position[0];
			grenade.position.y = position[1];
			grenade.position.z = position[2];
			const auto& viewangles = json["viewangles"];
			grenade.viewangles.x = viewangles[0];
			grenade.viewangles.y = viewangles[1];
			grenade.weapon = json["weapon"];
		};
	};

	std::optional<nlohmann::json> parse_grenades();
	std::vector<Grenade> parse_grenades_for_map(std::string_view map_name);
}
