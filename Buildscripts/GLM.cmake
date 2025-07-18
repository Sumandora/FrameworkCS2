# Add GLM to the project
FetchContent_Declare(glm
	GIT_REPOSITORY https://github.com/g-truc/glm.git
	GIT_PROGRESS TRUE
	GIT_TAG 1.0.1
	GIT_SHALLOW TRUE)
FetchContent_MakeAvailable(glm)

target_compile_definitions(glm PUBLIC "-DGLM_FORCE_SWIZZLE=1")
target_compile_definitions(glm PUBLIC "-DGLM_ENABLE_EXPERIMENTAL=1")

target_link_libraries(${CMAKE_PROJECT_NAME} PRIVATE glm)

embed_license("OpenGL Mathematics (GLM)" glm "${glm_SOURCE_DIR}/copying.txt")
