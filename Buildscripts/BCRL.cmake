# Add BCRL to the project
FetchContent_Declare(bcrl
        GIT_REPOSITORY https://github.com/Sumandora/bcrl
        GIT_PROGRESS TRUE
        GIT_TAG 60390f0)
FetchContent_MakeAvailable(bcrl)
target_link_libraries(${CMAKE_PROJECT_NAME} PRIVATE bcrl)
