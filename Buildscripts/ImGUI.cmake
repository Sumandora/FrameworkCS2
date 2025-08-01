# Add ImGUI to the project
FetchContent_Declare(imgui
	GIT_REPOSITORY https://github.com/ocornut/imgui
	GIT_PROGRESS TRUE
	GIT_TAG v1.91.7
	GIT_SHALLOW TRUE
	PATCH_COMMAND git apply ${CMAKE_CURRENT_SOURCE_DIR}/Patches/ImGUI/FullyRoundedTabs.patch
							${CMAKE_CURRENT_SOURCE_DIR}/Patches/ImGUI/RoundedImageButtons.patch
							${CMAKE_CURRENT_SOURCE_DIR}/Patches/ImGUI/PrependDrawList.patch
	UPDATE_DISCONNECTED TRUE)
FetchContent_MakeAvailable(imgui)

file(GLOB IMGUI_SOURCE_FILES "${imgui_SOURCE_DIR}/*.cpp") # Base

# Backends:
list(APPEND IMGUI_SOURCE_FILES "${imgui_SOURCE_DIR}/backends/imgui_impl_sdl3.cpp") # SDL 3
list(APPEND IMGUI_SOURCE_FILES "${imgui_SOURCE_DIR}/backends/imgui_impl_vulkan.cpp") # Vulkan

list(APPEND IMGUI_SOURCE_FILES "${imgui_SOURCE_DIR}/misc/cpp/imgui_stdlib.cpp") # C++ stdlib

# Remove unused components
# list(REMOVE_ITEM IMGUI_SOURCE_FILES "${imgui_SOURCE_DIR}/imgui_demo.cpp")
# target_compile_definitions(imgui PUBLIC IMGUI_DISABLE_DEMO_WINDOWS)
# target_compile_definitions(imgui PUBLIC IMGUI_DISABLE_DEBUG_TOOLS)

add_library(imgui STATIC ${IMGUI_SOURCE_FILES})
target_include_directories(imgui PUBLIC ${imgui_SOURCE_DIR})
target_compile_definitions(imgui PUBLIC IMGUI_DEFINE_MATH_OPERATORS)

set_target_properties(imgui PROPERTIES LINKER_LANGUAGE CXX)

target_link_libraries(${CMAKE_PROJECT_NAME} PRIVATE imgui)

embed_license("Dear ImGui" imgui "${imgui_SOURCE_DIR}/LICENSE.txt")
