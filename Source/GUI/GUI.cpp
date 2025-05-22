#include "GUI.hpp"

#include "backends/imgui_impl_sdl3.h"
#include "imgui.h"
#include "SDL3/SDL_events.h"
#include "SDL3/SDL_video.h"

#include "../Utils/Logging.hpp"

#include <cmath>
#include <cstdlib>
#include <cstring>
#include <mutex>
#include <unistd.h>
#include <utility>
#include <vector>

#include "Construction/Construction.hpp"

class OwningSDLEvent {
	SDL_Event event;

public:
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
		case SDL_EVENT_USER:
			// we don't know what the data is, so we can't copy it :(
			// let's just hope it's not deallocated before it's used
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
	~OwningSDLEvent()
	{
		switch (event.type) {
			// NOLINTBEGIN(hicpp-no-malloc, cppcoreguidelines-no-malloc, google-readability-casting)
		case SDL_EVENT_TEXT_INPUT:
			free((void*)event.text.text);
			break;
		case SDL_EVENT_TEXT_EDITING:
			free((void*)event.edit.text);
			break;
		case SDL_EVENT_USER:
			// we don't know what the data is, so we can't copy it :(
			// let's just hope it's not deallocated before it's used
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

void GUI::init()
{
	ImGui::CreateContext();

	ImGuiIO& io = ImGui::GetIO();

	io.IniFilename = nullptr; // TODO Bring it back
	io.LogFilename = nullptr;

	Logging::info("Initialized ImGui Context");
}

void GUI::destroy()
{
	ImGui::DestroyContext();
}

void GUI::create_font(SDL_Window* window)
{

	// We are running straight into the multi monitor dpi issue here, but to my knowledge there is no appropriate solution to this when using ImGui
	const SDL_DisplayID display_index = SDL_GetDisplayForWindow(window);
	const SDL_DisplayMode* mode = SDL_GetCurrentDisplayMode(display_index);

	// I want the font size to be something around 12 on full hd screens.
	// 4k screens get something around 24.
	// Mathematically expressed:
	// font_size(screen_height) = screen_height * x
	// font_size(1080) = 12
	// font_size(2160) = 24
	// x = 1/90, also technically one pair of values is enough here to solve for x here.

	const float font_size = floorf(static_cast<float>(mode->h) * (1.0F / 90.0F));

	// Might not work on certain distros/configurations
	bool loaded_font = false;

	ImGuiIO& io = ImGui::GetIO();

	for (const char* path : {
			 "/usr/share/fonts/noto/NotoSans-Regular.ttf",
			 "/usr/share/fonts/google-noto/NotoSans-Regular.ttf" }) {
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

void GUI::render()
{
	ImGui::NewFrame();

	Construction::render();

	ImGui::Render();
}

void GUI::queue_event(const SDL_Event* event)
{
	const std::lock_guard lock{ event_queue_mutex };
	event_queue.emplace_back(*event);
}

void GUI::flush_events()
{
	std::vector<OwningSDLEvent> events;

	{
		// Move events into local variables and create new vector to reduce time spent in this lock.
		const std::lock_guard<std::mutex> lock{ event_queue_mutex };
		std::swap(events, event_queue);
	}

	std::erase_if(events, [](const OwningSDLEvent& event) {
		ImGui_ImplSDL3_ProcessEvent(event.get_event());
		return true;
	});
}
