cmake_minimum_required(VERSION 3.20)

if(NOT DEFINED CMAKE_BUILD_TYPE)
	message("No build type specified, assuming 'Debug'")
	set(CMAKE_BUILD_TYPE Debug)
endif()

file(STRINGS "ProjectName" CMAKE_PROJECT_NAME) # Custom project name using this file
project(${CMAKE_PROJECT_NAME})

find_program(CCACHE_FOUND ccache)
if(CCACHE_FOUND)
	message("Using ccache for build")
	set(CMAKE_CXX_COMPILER_LAUNCHER "ccache")
endif()

set(CMAKE_CXX_STANDARD 23)

set(CMAKE_POSITION_INDEPENDENT_CODE ON)
set(CMAKE_SKIP_RPATH ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

file(GLOB_RECURSE SOURCE_FILES "${CMAKE_SOURCE_DIR}/Source/*.cpp")
add_library(${CMAKE_PROJECT_NAME} SHARED ${SOURCE_FILES})

include("Buildscripts/BuildParameters.cmake")

# Export the current project name as a preprocessor definition
target_compile_definitions(${CMAKE_PROJECT_NAME} PRIVATE PROJECT_NAME="${CMAKE_PROJECT_NAME}")

include(FetchContent)
set(FETCHCONTENT_QUIET FALSE)

include("Buildscripts/Licenses.cmake") # Macro to generate license headers

# Dependencies
include("Buildscripts/BCRL.cmake")
include("Buildscripts/ReturnAddressSpoofer.cmake")
find_package(Vulkan REQUIRED)
include("Buildscripts/ImGUI.cmake")
include("Buildscripts/SDL3.cmake")
include("Buildscripts/DetourHooking.cmake")
include("Buildscripts/NlohmannJson.cmake")
include("Buildscripts/GenericESP.cmake")
include("Buildscripts/GLM.cmake")
include("Buildscripts/MagicEnum.cmake")
include("Buildscripts/ImNodes.cmake")
include("Buildscripts/OrderedMap.cmake")
include("Buildscripts/Protobuf.cmake")
include("Buildscripts/Octree.cmake")
include("Buildscripts/MurmurHash.cmake")

add_subdirectory("${MemoryManager_SOURCE_DIR}/Modules/Linux" "LinuxMemoryManager")
target_link_libraries(${CMAKE_PROJECT_NAME} PRIVATE LinuxMemoryManager)

include("Buildscripts/Obfuscation.cmake")

target_link_libraries(${CMAKE_PROJECT_NAME} PRIVATE dl ${Vulkan_LIBRARIES})

define_licenses()
