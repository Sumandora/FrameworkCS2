# Add DetourHooking to the project
FetchContent_Declare(DetourHooking
        GIT_REPOSITORY https://github.com/Sumandora/DetourHooking
        GIT_PROGRESS TRUE
        GIT_TAG 4f14fde)
FetchContent_MakeAvailable(DetourHooking)
target_link_libraries(${CMAKE_PROJECT_NAME} PRIVATE DetourHooking)