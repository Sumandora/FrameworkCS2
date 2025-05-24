# Add nlohmann/json to the project
FetchContent_Declare(
	json
	GIT_REPOSITORY https://github.com/nlohmann/json.git
	GIT_PROGRESS TRUE
	GIT_TAG v3.12.0)
FetchContent_MakeAvailable(json)

target_link_libraries(${CMAKE_PROJECT_NAME} PUBLIC nlohmann_json)
