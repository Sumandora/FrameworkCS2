# Add BCRL to the project
set(SIGNATURESCANNER_OPTIMIZE ON CACHE BOOL "" FORCE)
FetchContent_Declare(BCRL
        GIT_REPOSITORY https://github.com/Sumandora/bcrl
        GIT_PROGRESS TRUE
        GIT_TAG 3.4.3)
FetchContent_MakeAvailable(BCRL)
target_link_libraries(${CMAKE_PROJECT_NAME} PRIVATE BCRL)
