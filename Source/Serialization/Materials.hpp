#pragma once

#include "../Utils/MutexGuard.hpp"

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

	// Don't store this guard for an extended period of time. Treat this structure like a std::lock_guard.
	MutexGuard<std::vector<Material>> get_materials();
}
