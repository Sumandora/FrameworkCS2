#include "Config.hpp"

#include <fstream>
#include <string>
#include <valarray>

#include "../Utils/Log.hpp"
#include "Keybinds.hpp"

#include "../Features/Features.hpp"

namespace Config {
	void addKeybind(KeybindButton* keybind) { keybinds.push_back(keybind); }

	void processInput()
	{
		for (const auto keybind : keybinds) {
			keybind->processInput();
		}
	}

	void save(std::string path)
	{
		Log::log(Log::Level::Debug, "Saving config to {}", path);
		nlohmann::json cfg;

		for(Features::Feature* feature : Features::allFeatures) {
			nlohmann::json e = feature;
			cfg[feature->getName()] = e;
		}

		std::ofstream output{ path };
		output << cfg << std::endl;
		output.close();
	}

	void load(std::string path)
	{
		Log::log(Log::Level::Debug, "Loading config from {}", path);
		libconfig::Config cfg;
		cfg.readFile(path);


		auto& root = cfg.getRoot();

		for(Features::Feature* feature : Features::allFeatures) {
			auto& group = root.lookup(feature->getName());
			feature->deserialize(group);
		}
	}
}