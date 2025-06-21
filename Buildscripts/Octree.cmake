# Add attcs/Octree to the project
FetchContent_Declare(octree
	GIT_REPOSITORY https://github.com/annell/octree-cpp.git
	GIT_PROGRESS TRUE
	GIT_SUBMODULES ""
	GIT_TAG 90f428db2fe436ec1564aae4eb0b0123c2c26256
	PATCH_COMMAND git apply ${CMAKE_CURRENT_SOURCE_DIR}/Patches/octree-cpp/DisableTests.patch
	UPDATE_DISCONNECTED TRUE)
FetchContent_MakeAvailable(octree)

target_link_libraries(${CMAKE_PROJECT_NAME} PRIVATE octree-cpp)

embed_license("octree-cpp" octree "${octree_SOURCE_DIR}/LICENSE")
