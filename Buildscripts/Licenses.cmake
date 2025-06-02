function(embed_license pretty_name name file)
	set(CONTENT "
namespace GUI::Tabs::About {
	extern const char ${name}_license[] = {
	#embed \"${file}\"
	};
}")
	file(WRITE "${CMAKE_CURRENT_BINARY_DIR}/${name}_license.cpp" "${CONTENT}")
	target_sources(${CMAKE_PROJECT_NAME} PRIVATE "${CMAKE_CURRENT_BINARY_DIR}/${name}_license.cpp")
	set(LICENSES "${LICENSES} LICENSE(\"${pretty_name}\", ${name})" PARENT_SCOPE)
endfunction()

function(define_licenses)
	target_compile_definitions(${CMAKE_PROJECT_NAME} PRIVATE "ITERATE_LICENSES=${LICENSES}")
endfunction()
