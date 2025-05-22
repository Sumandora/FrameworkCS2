#include "GUI.hpp"

#include "SDL3/SDL_events.h"
#include "backends/imgui_impl_sdl3.h"
#include "imgui.h"

#include "../Utils/Logging.hpp"

#include <cstdlib>
#include <cstring>
#include <mutex>
#include <utility>
#include <vector>

#include "../Hooks/Graphics/GraphicsHook.hpp"

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

void GUI::render()
{
	ImGui::NewFrame();

	GraphicsHook::mainLoop();

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
