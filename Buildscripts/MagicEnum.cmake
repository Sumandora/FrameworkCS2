# Add MagicEnum to the project
FetchContent_Declare(magic_enum
	GIT_REPOSITORY https://github.com/Neargye/magic_enum
	GIT_PROGRESS TRUE
	GIT_TAG v0.9.7)
FetchContent_MakeAvailable(magic_enum)

target_link_libraries(${CMAKE_PROJECT_NAME} PRIVATE magic_enum)

embed_license("Magic Enum" magic_enum "${magic_enum_SOURCE_DIR}/LICENSE")
