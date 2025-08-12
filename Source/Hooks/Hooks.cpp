#include "Hooks.hpp"

#include "Game/GameHook.hpp"
#include "Graphics/GraphicsHook.hpp"

#include "../Utils/Logging.hpp"

#include <csignal>
#include <filesystem>
#include <signal.h>
#include <string>
#include <sys/types.h>
#include <unistd.h>
#include <vector>

void Hooks::create()
{
	std::vector<pid_t> other_threads;

	const pid_t my_tid = gettid();

	for (const std::filesystem::path& it : std::filesystem::directory_iterator{ "/proc/self/task/" }) {
		const pid_t p = std::stoi(it.filename());
		if (p == my_tid)
			continue;

		kill(p, SIGSTOP);
		other_threads.push_back(p);
	}
#ifdef __cpp_lib_format_ranges
	Logging::debug("Paused threads: {}", other_threads);
#else
	// temporary, bad reimplementation:
	std::string vec_string;
	for (const pid_t pid : other_threads) {
		if (!vec_string.empty())
			vec_string += ", ";
		vec_string += std::to_string(pid);
	}
	Logging::debug("Paused threads: [{}]", vec_string);
#endif

	Game::create();

	if (!GraphicsHook::hookSDL()) {
		Logging::error("Failed to hook SDL");
		return;
	}

	if (!GraphicsHook::hookVulkan()) {
		Logging::error("Failed to hook vulkan");
		return;
	}

	for (const pid_t pid : other_threads) {
		kill(pid, SIGCONT);
	}
}

void Hooks::destroy()
{
	GraphicsHook::unhookVulkan();
	GraphicsHook::unhookSDL();

	Game::destroy();
}
