# Add nlohmann/json to the project
FetchContent_Declare(json
	GIT_REPOSITORY https://github.com/nlohmann/json.git
	GIT_PROGRESS TRUE
	GIT_TAG v3.12.0
	GIT_SHALLOW TRUE)
FetchContent_MakeAvailable(json)

target_link_libraries(${CMAKE_PROJECT_NAME} PRIVATE nlohmann_json)

embed_license("JSON for Modern C++ (nlohmann/json)" nlohmann_json "${json_SOURCE_DIR}/LICENSE.MIT")
