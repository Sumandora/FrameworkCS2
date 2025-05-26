# Compiler flags for build
target_compile_options(${CMAKE_PROJECT_NAME} PRIVATE "-march=native")

if(CMAKE_BUILD_TYPE STREQUAL "Debug")
    target_compile_options(${CMAKE_PROJECT_NAME} PRIVATE "-Wall" "-Wextra" "-Wpedantic" "-g" "-ggdb3" "-DDEBUG")
else()
    target_compile_options(${CMAKE_PROJECT_NAME} PRIVATE "-Os")
    set(CMAKE_CXX_FLAGS "-D'IM_ASSERT(_EXPR)=((void)(_EXPR))' ${CMAKE_CXX_FLAGS}") # TODO Can you do this without set?
endif()

if (CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
    target_compile_options(${CMAKE_PROJECT_NAME} PRIVATE "--no-gnu-unique")
    if (CMAKE_CXX_COMPILER_VERSION VERSION_GREATER_EQUAL 12) # Versions below 12 didn't have implicit constexpr
        target_compile_options(${CMAKE_PROJECT_NAME} PRIVATE "-fimplicit-constexpr")
    endif()
    if(NOT CMAKE_BUILD_TYPE STREQUAL "Debug") # Since every function is its own section (due to ReturnAddressSpoofer requiring "-ffunction-sections"), we can also use DCE
        target_compile_options(${CMAKE_PROJECT_NAME} PRIVATE "-fdata-sections" "-s")
        target_link_options(${CMAKE_PROJECT_NAME} PRIVATE "-Wl,--gc-sections")
    endif()
elseif (CMAKE_CXX_COMPILER_ID STREQUAL "Clang") # Clang is very verbose about stuff that GCC doesn't care about, disable these warnings
    target_compile_options(${CMAKE_PROJECT_NAME} PRIVATE "-Wno-unknown-attributes" "-Wno-unused-private-field" "-stdlib=libc++")
endif ()
