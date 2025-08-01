# Add protobuf to the project
set(protobuf_INSTALL OFF CACHE BOOL "" FORCE)
set(protobuf_BUILD_TESTS OFF CACHE BOOL "" FORCE)
# set(protobuf_WITH_ZLIB OFF CACHE BOOL "" FORCE)
set(protobuf_DISABLE_RTTI ON CACHE BOOL "" FORCE)
set(protobuf_DEBUG_POSTFIX "" CACHE STRING "" FORCE)

FetchContent_Declare(protobuf
	GIT_REPOSITORY https://github.com/protocolbuffers/protobuf
	GIT_PROGRESS TRUE
	GIT_TAG v3.21.8
	GIT_SHALLOW TRUE
	PATCH_COMMAND git apply ${CMAKE_CURRENT_SOURCE_DIR}/Patches/Protobuf/ProtocOnlyCXX.patch
	UPDATE_DISCONNECTED TRUE)
FetchContent_MakeAvailable(protobuf)

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

add_custom_command(
	OUTPUT ${GENERATED_PB_FILES}
	COMMAND "$<TARGET_FILE:protoc>" "--cpp_out" "${PROTOBUF_OUTDIR}" "--proto_path" "${protobuf_SOURCE_DIR}/src/:${PROTOBUF_DIR}" ${PROTOBUF_FILES}
	DEPENDS ${PROTOBUF_FILES}
)

add_custom_target(
	ProtobufGenerate
	DEPENDS ${GENERATED_PB_FILES}
	COMMENT "Generating Protocol buffer implementation files")

add_dependencies(ProtobufGenerate protoc)
add_dependencies(${CMAKE_PROJECT_NAME} ProtobufGenerate)

target_sources(${CMAKE_PROJECT_NAME} PRIVATE "${GENERATED_PB_FILES}")
target_include_directories(${CMAKE_PROJECT_NAME} PUBLIC "${PROTOBUF_OUTDIR}")

embed_license("Protocol Buffers" protobuf "${protobuf_SOURCE_DIR}/LICENSE")
