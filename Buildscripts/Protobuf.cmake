# Add protobuf to the project
set(protobuf_INSTALL OFF CACHE BOOL "" FORCE)
set(protobuf_BUILD_TESTS OFF CACHE BOOL "" FORCE)
# set(protobuf_WITH_ZLIB OFF CACHE BOOL "" FORCE)
set(protobuf_DISABLE_RTTI ON CACHE BOOL "" FORCE)

# What a beautiful build system.
set(ORIG_BUILD_TYPE ${CMAKE_BUILD_TYPE})
set(CMAKE_BUILD_TYPE Release)

FetchContent_Declare(protobuf
	GIT_REPOSITORY https://github.com/protocolbuffers/protobuf
	GIT_PROGRESS TRUE
	GIT_TAG v3.21.8)
FetchContent_MakeAvailable(protobuf)

set(CMAKE_BUILD_TYPE ${ORIG_BUILD_TYPE})

target_compile_options(libprotobuf-lite PRIVATE "-fdata-sections" "-Oz")
target_compile_options(libprotobuf PRIVATE "-fdata-sections" "-Oz")
target_link_options(libprotobuf-lite PRIVATE "-Wl,--gc-sections")
target_link_options(libprotobuf PRIVATE "-Wl,--gc-sections")

target_link_libraries(${CMAKE_PROJECT_NAME} PRIVATE libprotobuf)

set(PROTOBUF_DIR "${CMAKE_SOURCE_DIR}/Protobufs")
file(GLOB_RECURSE PROTOBUF_FILES "${PROTOBUF_DIR}/*.proto")

set(PROTOBUF_OUTDIR "${CMAKE_CURRENT_BINARY_DIR}/GeneratedProtobufs")
file(MAKE_DIRECTORY ${PROTOBUF_OUTDIR})

foreach(FILE ${PROTOBUF_FILES})
	string(REPLACE "${PROTOBUF_DIR}" "${PROTOBUF_OUTDIR}" GEN_FILE_A "${FILE}")
	string(REGEX REPLACE ".proto\$" ".pb.cc" GEN_FILE_B "${GEN_FILE_A}")
	list(APPEND GENERATED_PB_FILES "${GEN_FILE_B}")
endforeach()

# TODO Can this be prevented from rebuilding every single time?
add_custom_target(
	ProtobufGenerate
	COMMAND "$<TARGET_FILE:protoc>" "--cpp_out" "${PROTOBUF_OUTDIR}" "--proto_path" "${protobuf_SOURCE_DIR}/src/:${PROTOBUF_DIR}" ${PROTOBUF_FILES}
	DEPENDS protoc
	COMMENT "Generating Protocol buffer implementation files"
	BYPRODUCTS ${GENERATED_PB_FILES})

add_dependencies(${CMAKE_PROJECT_NAME} ProtobufGenerate)

target_sources(${CMAKE_PROJECT_NAME} PRIVATE "${GENERATED_PB_FILES}")
target_include_directories(${CMAKE_PROJECT_NAME} PUBLIC "${PROTOBUF_OUTDIR}")

embed_license("Protocol Buffers" protobuf "${protobuf_SOURCE_DIR}/LICENSE")
