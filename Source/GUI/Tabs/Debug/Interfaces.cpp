#include "../Tabs.hpp"

#include "imgui.h"

#include <cstddef>
#include <string>
#include <unordered_map>

#include <dlfcn.h>
#include <link.h>

#include "../../../Interfaces.hpp"

#include "../../../Utils/Logging.hpp"

#include "../../Elements/HelpMarker.hpp"

void GUI::Tabs::Debug::draw_interfaces()
{
	struct InterfaceData {
		void* create_fn = nullptr;
		void* uncovered = nullptr;
		void* real = nullptr;
	};

	static std::unordered_map<std::string, std::unordered_map<const char*, InterfaceData>> interfaces;

	if (interfaces.empty()) {
		dl_iterate_phdr(+[](struct dl_phdr_info* info, std::size_t /*size*/, void* data) {
            if(info->dlpi_name[0] == '\0') // Let's ignore the root object
				return 0;

            Logging::debug("Looking up interfaces of {}", info->dlpi_name);

			auto interfaced_library = Interfaces::InterfacedLibrary::create(info->dlpi_name);

			if(!interfaced_library.has_value())
				return 0;

			auto& interfaces = *reinterpret_cast<std::unordered_map<std::string, std::unordered_map<const char*, InterfaceData>>*>(data);
			for(const auto& [name, create_fn] : interfaced_library->get_interfaces()) {
				interfaces[info->dlpi_name][name] = InterfaceData{ .create_fn = create_fn};
			}
			
			return 0; }, &interfaces);
	}

	for (auto& [library_name, interfaces] : interfaces) {
		if (ImGui::TreeNode(library_name.c_str())) {
			for (auto& [interface_name, interface_data] : interfaces) {
				if (ImGui::TreeNode(interface_name)) {
					ImGui::Text("Create function: %p", interface_data.create_fn);
					ImGui::Text("Uncovered: %p", interface_data.uncovered);
					ImGui::Text("Real: %p", interface_data.real);

					ImGui::Separator();

					ImGui::Text("The next two addresses should line up, if they don't, the uncover function has to be adjusted.");
					if (ImGui::Button("Uncover create function")) {
						interface_data.uncovered = Interfaces::uncover_create_function(interface_data.create_fn);
					}
					if (ImGui::Button("Invoke create function")) {
						using CreateFnSig = void* (*)();
						interface_data.real = reinterpret_cast<CreateFnSig>(interface_data.create_fn)();
					}
					ImGuiExt::HelpMarker("Warning: This is considered unsafe");

					ImGui::TreePop();
				}
			}
			ImGui::TreePop();
		}
	}
}
