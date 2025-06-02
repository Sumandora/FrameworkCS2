# Add GenericESP to the project
set(IMGUI_DEPENDENCY_OVERRIDE imgui)
FetchContent_Declare(GenericESP
	GIT_REPOSITORY https://github.com/Sumandora/GenericESP
	GIT_PROGRESS TRUE
	GIT_TAG no-internal-settings)
FetchContent_MakeAvailable(GenericESP)
target_link_libraries(${CMAKE_PROJECT_NAME} PRIVATE GenericESP)
