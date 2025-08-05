#pragma once

#include <expected>
#include <filesystem>
#include <string>
#include <string_view>
#include <system_error>
#include <vector>

#include "nlohmann/json_fwd.hpp"

namespace Serialization {
	void create_config_directory();
	void shutdown();

	bool has_config(std::string_view name);
	bool is_reserved_name(const std::string& name);
	std::expected<void, std::string> create_config(std::string_view name);
	bool load_config(std::string_view name, std::error_code& status);

	std::vector<std::filesystem::path> get_available_configs();

	bool are_configs_available();

	std::filesystem::path get_config_directory();

	bool is_first_launch(); // Was the config directory already present before injection?
}
