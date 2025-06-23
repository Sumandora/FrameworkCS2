#include "GUI.hpp"

#include "Theme.hpp"

#include "backends/imgui_impl_sdl3.h"
#include "backends/imgui_impl_vulkan.h"
#include "imgui.h"

#include "SDL3/SDL_events.h"
#include "SDL3/SDL_keycode.h"
#include "SDL3/SDL_video.h"

#include "../Utils/Logging.hpp"

#include <cfloat>
#include <cmath>
#include <cstdlib>
#include <cstring>
#include <filesystem>
#include <mutex>
#include <string.h>
#include <unistd.h>
#include <utility>
#include <vector>

#include "Tabs/Tabs.hpp"

#include "../Hooks/Graphics/GraphicsHook.hpp"

#include "imnodes.h"

#include "../Notifications/Notifications.hpp"

#include "../Features/Visuals/GrenadeHelper.hpp"

class OwningSDLEvent {
	SDL_Event event;

public:
	OwningSDLEvent() = delete;
	explicit OwningSDLEvent(const SDL_Event& event)
		: event(event)
	{
		switch (event.type) {
		case SDL_EVENT_TEXT_INPUT:
			this->event.text.text = strdup(event.text.text);
			break;
		case SDL_EVENT_TEXT_EDITING:
			this->event.edit.text = strdup(event.edit.text);
			break;
		case SDL_EVENT_TEXT_EDITING_CANDIDATES:
			if (!event.edit_candidates.num_candidates)
				break;
			// NOLINTNEXTLINE(hicpp-no-malloc, cppcoreguidelines-no-malloc)
			this->event.edit_candidates.candidates = static_cast<char**>(calloc(event.edit_candidates.num_candidates, sizeof(char*)));
			for (int i = 0; i < event.edit_candidates.num_candidates; i++) {
				// NOLINTNEXTLINE(cppcoreguidelines-pro-type-const-cast)
				const_cast<char**>(this->event.edit_candidates.candidates)[i] = strdup(event.edit_candidates.candidates[i]);
			}
			break;
		case SDL_EVENT_DROP_FILE:
		case SDL_EVENT_DROP_TEXT:
		case SDL_EVENT_DROP_BEGIN:
		case SDL_EVENT_DROP_COMPLETE:
		case SDL_EVENT_DROP_POSITION:
			this->event.drop.data = strdup(event.drop.data);
			this->event.drop.source = strdup(event.drop.source);
			break;
		default:
			break;
		}
	}
	OwningSDLEvent(const OwningSDLEvent&) = delete;
	OwningSDLEvent& operator=(const OwningSDLEvent&) = delete;
	OwningSDLEvent(OwningSDLEvent&& other) noexcept
		: event(other.event)
	{
		// Delete other event to prevent double free
		switch (event.type) {
		case SDL_EVENT_TEXT_INPUT:
			other.event.text.text = nullptr;
			break;
		case SDL_EVENT_TEXT_EDITING:
			other.event.edit.text = nullptr;
			break;
		case SDL_EVENT_TEXT_EDITING_CANDIDATES:
			other.event.edit_candidates.candidates = nullptr;
			break;
		case SDL_EVENT_DROP_FILE:
		case SDL_EVENT_DROP_TEXT:
		case SDL_EVENT_DROP_BEGIN:
		case SDL_EVENT_DROP_COMPLETE:
		case SDL_EVENT_DROP_POSITION:
			other.event.drop.data = nullptr;
			other.event.drop.source = nullptr;
			break;
		default:
			break;
		}
	}
	OwningSDLEvent& operator=(OwningSDLEvent&&) = delete;
	~OwningSDLEvent()
	{
		// Free does not do anything when nullptr is provided.
		switch (event.type) {
			// NOLINTBEGIN(hicpp-no-malloc, cppcoreguidelines-no-malloc, google-readability-casting)
		case SDL_EVENT_TEXT_INPUT:
			free((void*)event.text.text);
			break;
		case SDL_EVENT_TEXT_EDITING:
			free((void*)event.edit.text);
			break;
		case SDL_EVENT_TEXT_EDITING_CANDIDATES:
			if (!event.edit_candidates.num_candidates)
				break;
			for (int i = 0; i < event.edit_candidates.num_candidates; i++) {
				free((void*)event.edit_candidates.candidates[i]);
			}
			free((void*)event.edit_candidates.candidates);
			break;
		case SDL_EVENT_DROP_FILE:
		case SDL_EVENT_DROP_TEXT:
		case SDL_EVENT_DROP_BEGIN:
		case SDL_EVENT_DROP_COMPLETE:
		case SDL_EVENT_DROP_POSITION:
			free((void*)event.drop.data);
			free((void*)event.drop.source);
			break;
		default:
			break;
			// NOLINTEND(hicpp-no-malloc, cppcoreguidelines-no-malloc, google-readability-casting)
		}
	}

	// NOLINTNEXTLINE(google-explicit-constructor, hicpp-explicit-conversions)
	[[nodiscard]] const SDL_Event* get_event() const { return &event; }
};

static std::mutex event_queue_mutex; // https://github.com/ocornut/imgui/issues/6895
static std::vector<OwningSDLEvent> event_queue{};

static bool is_open = true;

static SDL_Window* window;

static float scale = 1.0F;
static float font_size = 12.0F;

void GUI::init(const std::filesystem::path& config_directory)
{
	ImGui::CreateContext();

	ImGuiIO& io = ImGui::GetIO();

	io.IniFilename = strdup((config_directory / "imgui.ini").c_str());
	io.LogFilename = nullptr;

	Logging::info("Initialized ImGui Context");

	Theme::anti_purple_theme();
}

void GUI::destroy()
{
	ImGui::DestroyContext();
}

static void create_font()
{
	// We are running straight into the multi monitor dpi issue here, but to my knowledge there is no appropriate solution to this when using ImGui
	const SDL_DisplayID display_index = SDL_GetDisplayForWindow(window);
	const SDL_DisplayMode* mode = SDL_GetCurrentDisplayMode(display_index);

	// I want the font size to be something around 16 on full hd screens.
	// 4k screens get something around 32.
	// Mathematically expressed:
	// font_size(1080) = 16
	// font_size(2160) = 32
	// Simplified
	// font_size(x) = x / 1080 * 16

	// Assign global scale variable
	scale = static_cast<float>(mode->h) / 1080;

	// Floor to not get ugly subpixel values.
	font_size = floorf(scale * 16.0F);

	Logging::info("Using font size {} for display {}x{} with {} density", font_size, mode->w, mode->h, mode->pixel_density);

	// Might not work on certain distros/configurations
	bool loaded_font = false;

	ImGuiIO& io = ImGui::GetIO();

	for (const char* path : {
			 // Preferably load a system Noto Sans, but if we can't find one, then the game offers its own.
			 "/usr/share/fonts/noto/NotoSans-Regular.ttf",
			 "/usr/share/fonts/google-noto/NotoSans-Regular.ttf",
			 "../../csgo/panorama/fonts/notosans-regular.ttf" }) {
		if (access(path, F_OK) == 0 && io.Fonts->AddFontFromFileTTF(path, font_size)) {
			loaded_font = true;
			break;
		}
	}

	if (!loaded_font) {
		ImFontConfig config;
		config.SizePixels = font_size;
		io.Fonts->AddFontDefault(&config);
	}
}

void GUI::provide_window(SDL_Window* window)
{
	::window = window;

	create_font();
}

void GUI::render()
{
	ImGuiIO& io = ImGui::GetIO();

	static constexpr ImGuiConfigFlags INPUT_MASK = ImGuiConfigFlags_NoKeyboard | ImGuiConfigFlags_NoMouse;
	if (is_open) {
		io.ConfigFlags &= ~INPUT_MASK;
	} else {
		io.ConfigFlags |= INPUT_MASK;
	}
	io.MouseDrawCursor = is_open && SDL_GetWindowMouseGrab(window);

	ImGui::NewFrame();

	if (is_open) {
		Tabs::render();

		ImGui::ShowDemoWindow();
	}

	Notifications::render();
	grenade_helper->draw();

	ImGui::Render();
}

bool GUI::queue_event(const SDL_Event* event)
{
	if (event->type < SDL_EVENT_KEY_DOWN || event->type > SDL_EVENT_DROP_POSITION)
		return false; // These kinds of events should probably not be swallowed

	if (event->type == SDL_EVENT_USER)
		return false; // The data in this thing is pretty much uncloneable. ImGui doesn't need it, I don't need it, don't save it.

	if (event->type == SDL_EVENT_KEY_DOWN && event->key.key == SDLK_INSERT)
		is_open = !is_open;

	const std::lock_guard lock{ event_queue_mutex };
	event_queue.emplace_back(*event);

	return is_open;
}

void GUI::flush_events()
{
	std::vector<OwningSDLEvent> events;

	{
		// Move events into local variables and create new vector to reduce time spent in this lock.
		const std::lock_guard<std::mutex> lock{ event_queue_mutex };
		std::swap(events, event_queue);
	}

	for (const OwningSDLEvent& event : events) {
		ImGui_ImplSDL3_ProcessEvent(event.get_event());
	}
}

bool GUI::is_using_wayland()
{
	static const char* driver = []() {
		const char* driver = SDL_GetCurrentVideoDriver();
		Logging::info("SDL Video Driver: {}", driver);
		return driver;
	}();

	return strcmp(driver, "wayland") == 0;
}

float GUI::get_scale()
{
	return scale;
}

float GUI::get_base_font_size()
{
	return font_size;
}
