# Add GenericESP to the project
set(IMGUI_DEPENDENCY_OVERRIDE imgui)
FetchContent_Declare(GenericESP
        GIT_REPOSITORY https://github.com/Sumandora/GenericESP
        GIT_PROGRESS TRUE
        GIT_TAG c00b11c)
FetchContent_MakeAvailable(GenericESP)
add_subdirectory("${GenericESP_SOURCE_DIR}/Extensions/DefaultRenderers")
target_link_libraries(${CMAKE_PROJECT_NAME} GenericESP GenericESPDefaultRenderers)
