# Add nlohmann/json to the project

FetchContent_Declare(json URL https://github.com/nlohmann/json/releases/download/v3.11.3/json.tar.xz)
FetchContent_MakeAvailable(json)

target_link_libraries(${CMAKE_PROJECT_NAME} nlohmann_json::nlohmann_json)
