# This function only writes to a file if the contents differs,
# this is useful because it avoids updating the modification date and thus doesn't trigger a rebuild.
function(write_file_if_changed file content)
	if(NOT EXISTS "${file}")
		file(WRITE "${file}" "${content}")
		return()
	endif()
	file(READ "${file}" CURRENT_CONTENT)
	string(COMPARE NOTEQUAL "${content}" "${CURRENT_CONTENT}" NEEDS_UPDATE)
	if(NEEDS_UPDATE)
		file(WRITE "${file}" "${content}")
	endif()
endfunction()

function(embed_license pretty_name name file)
	file(READ ${file} license_text)
	write_file_if_changed("${CMAKE_CURRENT_BINARY_DIR}/${name}_license.txt" "R\"(${license_text})\"")
	write_file_if_changed("${CMAKE_CURRENT_BINARY_DIR}/${name}_license.cpp" "
namespace GUI::Tabs::About {
	extern const char ${name}_license[] = {
	#include \"${CMAKE_CURRENT_BINARY_DIR}/${name}_license.txt\"
	};
}")
	target_sources(${CMAKE_PROJECT_NAME} PRIVATE "${CMAKE_CURRENT_BINARY_DIR}/${name}_license.cpp")
	set(LICENSES "${LICENSES} LICENSE(\"${pretty_name}\", ${name})" PARENT_SCOPE)
endfunction()

function(define_licenses)
	target_compile_definitions(${CMAKE_PROJECT_NAME} PRIVATE "ITERATE_LICENSES=${LICENSES}")
endfunction()
