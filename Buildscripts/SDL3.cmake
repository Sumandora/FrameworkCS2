# Add SDL3 to the project
FetchContent_Declare(SDL3
	GIT_REPOSITORY https://github.com/libsdl-org/SDL
	GIT_PROGRESS TRUE
	GIT_TAG SDL-prerelease-3.1.2-1827-g34c601136)
FetchContent_GetProperties(SDL3)
if(NOT SDL3_POPULATED)
	FetchContent_Populate(SDL3)
endif()

add_library(SDL INTERFACE)
target_include_directories(SDL INTERFACE "${sdl3_SOURCE_DIR}/include")

target_link_libraries(imgui PRIVATE SDL)
target_link_libraries(${CMAKE_PROJECT_NAME} PRIVATE SDL)
