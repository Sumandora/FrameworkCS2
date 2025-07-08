# Add small_vector to the project
# TODO remove installs
FetchContent_Declare(small_vector
	GIT_REPOSITORY https://github.com/gharveymn/small_vector
	GIT_PROGRESS TRUE
	GIT_TAG v0.10.2)
FetchContent_MakeAvailable(small_vector)

target_link_libraries(${CMAKE_PROJECT_NAME} PRIVATE gch::small_vector)

embed_license("small_vector" small_vector "${small_vector_SOURCE_DIR}/LICENSE")
