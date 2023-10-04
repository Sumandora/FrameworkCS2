# Add BCRL to the project
FetchContent_Declare(bcrl
        GIT_REPOSITORY https://github.com/Sumandora/bcrl
        GIT_PROGRESS TRUE
        GIT_TAG c3a997c)
FetchContent_MakeAvailable(bcrl)
target_link_libraries(${CMAKE_PROJECT_NAME} PRIVATE bcrl)
