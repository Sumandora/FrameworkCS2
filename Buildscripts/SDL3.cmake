# Add SDL3 to the project
FetchContent_Declare(SDL3
        GIT_REPOSITORY https://github.com/libsdl-org/SDL
        GIT_PROGRESS TRUE
        GIT_TAG SDL-prerelease-3.0.0-2949-g666301f9f)
FetchContent_MakeAvailable(SDL3)

target_link_libraries(imgui SDL3::SDL3)
target_include_directories(imgui PUBLIC ${SDL3_SOURCE_DIR}/include)
