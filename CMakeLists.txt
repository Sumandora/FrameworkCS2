cmake_minimum_required(VERSION 3.20)

file(STRINGS "ProjectName" CMAKE_PROJECT_NAME)

project(${CMAKE_PROJECT_NAME})

set(CMAKE_CXX_STANDARD 23)

set(CMAKE_POSITION_INDEPENDENT_CODE ON)
set(CMAKE_SKIP_RPATH ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

file(GLOB_RECURSE SOURCE_FILES "${CMAKE_SOURCE_DIR}/Source/*.cpp")
add_library(${CMAKE_PROJECT_NAME} SHARED ${SOURCE_FILES})

function(apply_obfuscation_parameters TARGET)
	target_compile_options(${TARGET} PRIVATE "-fno-rtti") # Force a compile time error in case the target is using any RTTI features
	if(NOT CMAKE_BUILD_TYPE STREQUAL "Debug")
		target_compile_options(${TARGET} PRIVATE "-fvisibility=hidden" "-fvisibility-inlines-hidden")
	endif ()
endfunction()

apply_obfuscation_parameters(${CMAKE_PROJECT_NAME})

target_compile_options(${CMAKE_PROJECT_NAME} PRIVATE "-march=native")

if(CMAKE_BUILD_TYPE STREQUAL "Debug")
	target_compile_options(${CMAKE_PROJECT_NAME} PRIVATE "-Wall" "-ggdb" "-DDEBUG")
else()
	target_compile_options(${CMAKE_PROJECT_NAME} PRIVATE "-Os")
	set(CMAKE_CXX_FLAGS "-D'IM_ASSERT(_EXPR)=((void)(_EXPR))' ${CMAKE_CXX_FLAGS}") # TODO Can you do this without set?
endif()

if (CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
	target_compile_options(${CMAKE_PROJECT_NAME} PRIVATE "--no-gnu-unique")
	if (CMAKE_CXX_COMPILER_VERSION VERSION_GREATER_EQUAL 12)
		target_compile_options(${CMAKE_PROJECT_NAME} PRIVATE "-fimplicit-constexpr")
	endif()
    if(NOT CMAKE_BUILD_TYPE STREQUAL "Debug")
		target_compile_options(${CMAKE_PROJECT_NAME} PRIVATE "-fdata-sections" "-s")
		target_link_options(${CMAKE_PROJECT_NAME} PRIVATE "-Wl,--gc-sections")
    endif()
elseif (CMAKE_CXX_COMPILER_ID STREQUAL "Clang")
    target_compile_options(${CMAKE_PROJECT_NAME} PRIVATE "-Wno-unknown-attributes" "-Wno-unused-private-field" "-stdlib=libc++")
endif ()

target_compile_definitions(${CMAKE_PROJECT_NAME} PRIVATE PROJECT_NAME="${CMAKE_PROJECT_NAME}")

include(FetchContent)

FetchContent_Declare(bcrl
	GIT_REPOSITORY https://github.com/Sumandora/bcrl
	GIT_PROGRESS TRUE
	GIT_TAG c3a997c)
FetchContent_MakeAvailable(bcrl)
target_link_libraries(${CMAKE_PROJECT_NAME} PRIVATE bcrl)

function(apply_obfuscation_recursively TARGET)
	get_target_property(DEPENDENCIES ${TARGET} LINK_LIBRARIES)
	if(NOT DEPENDENCIES)
		return()
	endif()

	foreach(DEPENDENCY ${DEPENDENCIES})
		get_target_property(LIB_TYPE ${DEPENDENCY} TYPE)
		if(NOT LIB_TYPE STREQUAL "INTERFACE_LIBRARY")
			apply_obfuscation_parameters(${DEPENDENCY})
		endif ()
		apply_obfuscation_recursively(${DEPENDENCY})
	endforeach()

endfunction()

apply_obfuscation_recursively(${CMAKE_PROJECT_NAME})

target_link_libraries(${CMAKE_PROJECT_NAME} PRIVATE dl SDL2 GL)
