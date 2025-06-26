# Add MurmurHash to the project
# Funny story. This library was actually the same that Valve used. It's kind of weird.
FetchContent_Declare(murmurhash
	GIT_REPOSITORY https://github.com/explosion/murmurhash.git
	GIT_PROGRESS TRUE
	GIT_TAG release-v1.0.13
	GIT_SHALLOW TRUE)
FetchContent_MakeAvailable(murmurhash)

# MurmurHash3 is not really needed, but I'll add it anyways, the project is configured to do DCE anyways.
add_library(murmurhash STATIC
	"${murmurhash_SOURCE_DIR}/murmurhash/MurmurHash2.cpp"
	"${murmurhash_SOURCE_DIR}/murmurhash/MurmurHash3.cpp")

target_include_directories(murmurhash PUBLIC "${murmurhash_SOURCE_DIR}/murmurhash/include")

target_link_libraries(${CMAKE_PROJECT_NAME} PRIVATE murmurhash)

# the copyright headers in the header files suggest that there is no copyright, but I'm just gonna add it anyways
embed_license("MurmurHash" murmurhash "${murmurhash_SOURCE_DIR}/LICENSE")
