# Add ImNodes to the project
FetchContent_Declare(imnodes
	GIT_REPOSITORY https://github.com/Nelarius/imnodes
	GIT_PROGRESS TRUE
	GIT_TAG v0.5)
FetchContent_MakeAvailable(imnodes)

add_library(imnodes STATIC "${imnodes_SOURCE_DIR}/imnodes.cpp")
target_include_directories(imnodes PUBLIC ${imnodes_SOURCE_DIR})
target_link_libraries(imnodes PUBLIC imgui)

target_link_libraries(${CMAKE_PROJECT_NAME} PRIVATE imnodes)

embed_license("ImNodes" imnodes "${imnodes_SOURCE_DIR}/LICENSE.md")
