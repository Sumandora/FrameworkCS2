# This is mostly written as GCC requires linking against libatomic, while clang does not.

option(USE_SHARED_ATOMIC "Force shared libatomic" OFF)

if(USE_SHARED_ATOMIC)
	target_link_libraries(${CMAKE_PROJECT_NAME} PRIVATE atomic)
	return()
endif()

include(CheckCXXSourceCompiles)

check_cxx_source_compiles("
#include <atomic>
#include <cstddef>

struct S { std::size_t a,b,c,d,e,f; };
std::atomic<S> s{};

int main() {
	S s2 = s.load();
	s.store(s2);
}" HAS_BUILTIN_LIBATOMIC)

if(HAS_BUILTIN_LIBATOMIC)
	return()
endif()

execute_process(
    COMMAND ${CMAKE_CXX_COMPILER} -print-file-name=libatomic.a
    OUTPUT_VARIABLE ATOMIC_LIBRARY_PATH
    OUTPUT_STRIP_TRAILING_WHITESPACE)

if(EXISTS ${ATOMIC_LIBRARY_PATH})
	message("Compiler doesn't have builtin any-size atomics, using atomic library: ${ATOMIC_LIBRARY_PATH}")

	target_link_libraries(${CMAKE_PROJECT_NAME} PRIVATE ${ATOMIC_LIBRARY_PATH})

	return()
endif()

message("Your system is missing a static libatomic. Using the dynamic one as a fallback.")

target_link_libraries(${CMAKE_PROJECT_NAME} PRIVATE atomic)
