# Add libconfig++ to the project
FetchContent_Declare(libconfig
        GIT_REPOSITORY https://github.com/hyperrealm/libconfig.git
        GIT_PROGRESS TRUE
        GIT_TAG v1.7.3)
FetchContent_MakeAvailable(libconfig)
target_link_libraries(${CMAKE_PROJECT_NAME} libconfig)