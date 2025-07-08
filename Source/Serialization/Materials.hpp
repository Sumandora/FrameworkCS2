#pragma once

#include <filesystem>
#include <functional>
#include <string>
#include <vector>

namespace Serialization::Materials {
	struct Material {
		std::string name;
		std::function<std::string()> acquire_kv3;
	};

	// Creates directory and provides a few default materials
	void initialize_directory(const std::filesystem::path& config_dir);

	const std::vector<Material>& get_materials(); // Don't store this vector or its contents, re-request them.
}
