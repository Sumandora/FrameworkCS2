function(apply_obfuscation_parameters TARGET)
	if("${TARGET}" STREQUAL "libprotobuf")
		return()
	endif()

	target_compile_options(${TARGET} PRIVATE "-fno-rtti") # Force a compile time error in case the target is using any RTTI features
	if(NOT CMAKE_BUILD_TYPE STREQUAL "Debug")
		target_compile_options(${TARGET} PRIVATE "-fvisibility=hidden" "-fvisibility-inlines-hidden")
	endif()
endfunction()

function(apply_obfuscation_recursively TARGET)
	if(NOT TARGET ${TARGET})
		return() # This target is not part of our code base
	endif()

	# If not a header-only library, apply parameters
	get_target_property(LIB_TYPE ${TARGET} TYPE)
	get_property(IS_ALIAS TARGET ${TARGET} PROPERTY ALIASED_TARGET)
	if((NOT LIB_TYPE STREQUAL "INTERFACE_LIBRARY") AND (NOT IS_ALIAS))
		apply_obfuscation_parameters(${TARGET})
	endif()

	# Get dependencies
	get_target_property(DEPENDENCIES ${TARGET} LINK_LIBRARIES)
	if(NOT DEPENDENCIES) # If no dependencies then return
		return()
	endif()

	foreach(DEPENDENCY ${DEPENDENCIES}) # Call this method recursively for every dependency
		apply_obfuscation_recursively(${DEPENDENCY})
	endforeach()

endfunction()

apply_obfuscation_recursively(${CMAKE_PROJECT_NAME})
