# Add ConvexHull to the project
FetchContent_Declare(convex-hull
	GIT_REPOSITORY https://github.com/Sumandora/convex-hull-cpp14
	GIT_PROGRESS TRUE
	GIT_TAG 818369e2f8150286d9850b46a19ba77d8df64192)
FetchContent_MakeAvailable(convex-hull)

target_link_libraries(${CMAKE_PROJECT_NAME} PRIVATE hull)

embed_license("Convex Hull Algorithms in C++14 (ConvexHull)" glm "${convex-hull_SOURCE_DIR}/LICENSE")
