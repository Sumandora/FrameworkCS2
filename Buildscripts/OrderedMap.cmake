# Add ordered_map to the project
set(CMAKE_POLICY_VERSION_MINIMUM 3.5 CACHE STRING "" FORCE)
FetchContent_Declare(ordered_map
	GIT_REPOSITORY https://github.com/Tessil/ordered-map.git
	GIT_PROGRESS TRUE
	GIT_TAG v1.1.0
	PATCH_COMMAND git apply ${CMAKE_CURRENT_SOURCE_DIR}/Patches/ordered_map/DontInstall.patch
							${CMAKE_CURRENT_SOURCE_DIR}/Patches/ordered_map/IncreaseCMakeMinimumVersion.patch
	UPDATE_DISCONNECTED TRUE)
FetchContent_MakeAvailable(ordered_map)

target_link_libraries(${CMAKE_PROJECT_NAME} PRIVATE ordered_map)

embed_license("ordered-map" ordered_map "${ordered_map_SOURCE_DIR}/LICENSE")
