# Add lz4 to the project
FetchContent_Declare(lz4
	GIT_REPOSITORY https://github.com/lz4/lz4
	GIT_PROGRESS TRUE
	GIT_TAG v1.10.0
	GIT_SHALLOW TRUE)
FetchContent_MakeAvailable(lz4)

set(LZ4_BUILD_CLI OFF CACHE BOOL "" FORCE)
set(BUILD_SHARED_LIBS OFF CACHE BOOL "" FORCE)
set(BUILD_STATIC_LIBS ON CACHE BOOL "" FORCE)
add_subdirectory("${lz4_SOURCE_DIR}/build/cmake/")
target_link_libraries(${CMAKE_PROJECT_NAME} PRIVATE lz4)

embed_license("LZ4" lz4 "${lz4_SOURCE_DIR}/lib/LICENSE")
