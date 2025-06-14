# Add GenericESP to the project
set(IMGUI_DEPENDENCY_OVERRIDE imgui)
FetchContent_Declare(GenericESP
	GIT_REPOSITORY https://github.com/Sumandora/GenericESP
	GIT_PROGRESS TRUE
	GIT_TAG 7095d68215e7b74d993e52f1aa8cee1c6afeaeff)
FetchContent_MakeAvailable(GenericESP)
target_link_libraries(${CMAKE_PROJECT_NAME} PRIVATE GenericESP)
