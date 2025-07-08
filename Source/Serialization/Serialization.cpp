#include "Serialization.hpp"

#include "GrenadeSerialization.hpp"
#include "Materials.hpp"

#include <algorithm>
#include <array>
#include <cstdlib>
#include <expected>
#include <filesystem>
#include <fstream>
#include <optional>
#include <string>
#include <string_view>
#include <system_error>
#include <unistd.h>
#include <vector>

#include "../Features/Feature.hpp"
#include "../Features/Features.hpp"

#include "../Utils/Logging.hpp"

#include "nlohmann/json.hpp"
#include "nlohmann/json_fwd.hpp"

static bool is_first_launch = false;
static bool available = true;

static constexpr std::string_view CONFIG_DIR_NAME = "FrameworkCS2";

static constexpr std::string_view GRENADES_FILE = "grenades.json";
static constexpr std::string_view PLAYERLIST_FILE = "users.json";

static constexpr std::array RESERVED_CONFIG_NAMES = {
	GRENADES_FILE, // Grenade helper
	PLAYERLIST_FILE, // Player list
};

void Serialization::create_config_directory()
{
	const std::filesystem::path config_dir = get_config_directory();
	Logging::info("Config directory: {}", config_dir.string());

	::is_first_launch = !std::filesystem::exists(config_dir);

	if (::is_first_launch) {
		std::error_code status;
		if (!std::filesystem::create_directory(config_dir, status)) {
			Logging::error("Failed to create config directory, configs are unavailable: {}", status.message());
			available = false;
		}
	}

	if (access(config_dir.c_str(), R_OK | W_OK) != F_OK) {
		available = false;
		Logging::perror("Failed to access config directory: {}");
	}

	if (available) {
		Materials::initialize_directory(config_dir);
	}
}

void Serialization::shutdown()
{
	if (!available) {
		Logging::error("Not saving current config, as configs are unavailable...");
		return;
	}
	// Save config temporarily...
}

std::expected<void, std::string> Serialization::create_config(std::string_view name, bool overwrite)
{
	// NOTE: Despite it saying '+=', this does not affect the name variable itself, only the path
	const std::filesystem::path config_file{ get_config_directory() / name += ".json" };

	if (std::ranges::contains(RESERVED_CONFIG_NAMES, config_file.filename()))
		return std::unexpected("Config name is reserved.");

	std::error_code status;
	if (std::filesystem::exists(config_file, status)) {
		if (!overwrite)
			return std::unexpected("File exists.");
	} else if (status)
		return std::unexpected(status.message());

	nlohmann::json j;
	for (const auto& [category, features] : Features::features) {
		auto& category_json = j[category];
		for (const Feature* feature : features) {
			auto& feature_json = category_json[feature->get_name()];
			feature->serialize(feature_json);
		}
	}

	std::ofstream stream{ config_file };
	stream << j.dump(2); // pretty print
	stream.close();

	return {};
}

bool Serialization::load_config(std::string_view name, std::error_code& status)
{
	// NOTE: Despite it saying '+=', this does not affect the name variable itself, only the path
	const std::filesystem::path config_file{ get_config_directory() / name += ".json" };

	if (std::ranges::contains(RESERVED_CONFIG_NAMES, config_file.filename()))
		return false;

	if (!std::filesystem::exists(config_file, status))
		return false;

	nlohmann::json j;
	std::ifstream stream{ config_file };
	stream >> j;
	stream.close();

	for (const auto& [category, features] : Features::features) {
		if (!j.contains(category))
			continue;
		const auto& category_json = j[category];
		for (Feature* feature : features) {
			if (!category_json.contains(feature->get_name()))
				continue;
			const auto& feature_json = category_json[feature->get_name()];
			feature->deserialize(feature_json);
		}
	}

	return {};
}

std::vector<std::filesystem::path> Serialization::get_available_configs()
{
	std::vector<std::filesystem::path> configs;

	for (const std::filesystem::path& config_file : std::filesystem::directory_iterator{ get_config_directory() }) {
		if (config_file.extension() != ".json")
			continue;

		if (std::ranges::contains(RESERVED_CONFIG_NAMES, config_file.filename()))
			continue;

		configs.emplace_back(config_file);
	}

	return configs;
}

bool Serialization::are_configs_available()
{
	return available;
}

std::filesystem::path Serialization::get_config_directory()
{
	static const std::filesystem::path CACHED_CONFIG_DIRECTORY = [] {
		// https://specifications.freedesktop.org/basedir-spec/latest/
		// Not following these specs completely, because there shouldn't be configs inside let's say /etc
		const char* config_home = std::getenv("XDG_CONFIG_HOME");
		if (config_home)
			return std::filesystem::path{ config_home } / CONFIG_DIR_NAME;
		const char* home = std::getenv("HOME");
		if (home)
			return std::filesystem::path{ home } / ".config" / CONFIG_DIR_NAME;

		Logging::error("You don't have XDG_CONFIG_HOME nor HOME set, guessing config path with username");

		char* username = getlogin();
		return std::filesystem::path{ "/" } / "home" / username / ".config" / CONFIG_DIR_NAME;
	}();
	return CACHED_CONFIG_DIRECTORY;
}

bool Serialization::is_first_launch()
{
	return ::is_first_launch;
}

std::optional<nlohmann::json> Serialization::Grenades::parse_grenades()
{
	const std::filesystem::path grenades_path = get_config_directory() / GRENADES_FILE;

	if (!std::filesystem::exists(grenades_path))
		return std::nullopt;

	nlohmann::json j;
	std::ifstream stream{ grenades_path };
	stream >> j;
	stream.close();

	return j;
}
