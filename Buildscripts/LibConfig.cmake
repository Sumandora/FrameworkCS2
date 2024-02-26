# Add libconfig++ to the project

# TODO global scope sucks
set(BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
set(BUILD_SHARED_LIBS OFF CACHE BOOL "" FORCE)
set(BUILD_TESTS OFF CACHE BOOL "" FORCE)

FetchContent_Declare(libconfig
        GIT_REPOSITORY https://github.com/hyperrealm/libconfig.git
        GIT_PROGRESS TRUE
        GIT_TAG v1.7.3)
FetchContent_MakeAvailable(libconfig)
target_link_libraries(${CMAKE_PROJECT_NAME} config++)
target_include_directories(${CMAKE_PROJECT_NAME} PUBLIC "${libconfig_SOURCE_DIR}/lib")
