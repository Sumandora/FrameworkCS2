# Add ReturnAddressSpoofer to the project
FetchContent_Declare(ReturnAddressSpoofer
        GIT_REPOSITORY https://github.com/Sumandora/ReturnAddressSpoofer
        GIT_PROGRESS TRUE
        GIT_TAG experimental-32-bit-and-clang-support) # TODO Replace with hash
FetchContent_MakeAvailable(ReturnAddressSpoofer)
target_link_libraries(${CMAKE_PROJECT_NAME} PRIVATE ReturnAddressSpoofer)

target_compile_options(${CMAKE_PROJECT_NAME} PRIVATE "-ffunction-sections") # This is required for the transformation

add_subdirectory("${ReturnAddressSpoofer_SOURCE_DIR}/ObjectFileRewriter")
add_custom_command(
        TARGET ${CMAKE_PROJECT_NAME} PRE_LINK
        COMMAND "$<TARGET_FILE:ObjectFileRewriter>" "-i" "${CMAKE_BINARY_DIR}/CMakeFiles/${CMAKE_PROJECT_NAME}.dir"
        DEPENDS ObjectFileRewriter
)