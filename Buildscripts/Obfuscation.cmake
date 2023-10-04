
function(apply_obfuscation_parameters TARGET)
    target_compile_options(${TARGET} PRIVATE "-fno-rtti") # Force a compile time error in case the target is using any RTTI features
    if(NOT CMAKE_BUILD_TYPE STREQUAL "Debug")
        target_compile_options(${TARGET} PRIVATE "-fvisibility=hidden" "-fvisibility-inlines-hidden")
    endif ()
endfunction()

function(apply_obfuscation_recursively TARGET)
    # If not an interface library, apply parameters
    get_target_property(LIB_TYPE ${TARGET} TYPE)
    if(NOT LIB_TYPE STREQUAL "INTERFACE_LIBRARY")
        apply_obfuscation_parameters(${TARGET})
    endif ()

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