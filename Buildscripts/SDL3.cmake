# Add SDL3 to the project
FetchContent_Declare(SDL3
        GIT_REPOSITORY https://github.com/libsdl-org/SDL
        GIT_PROGRESS TRUE
        GIT_TAG SDL-prerelease-3.0.0-2926-gaaf54b09a)
FetchContent_MakeAvailable(SDL3)

target_link_libraries(imgui PRIVATE SDL3)
target_link_libraries(${CMAKE_PROJECT_NAME} PRIVATE SDL3)

target_include_directories(imgui PRIVATE ${SDL3_SOURCE_DIR}/include)
target_include_directories(${CMAKE_PROJECT_NAME} PRIVATE ${SDL3_SOURCE_DIR}/include)
