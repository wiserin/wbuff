cmake_minimum_required(VERSION 3.12)

include(FetchContent)
FetchContent_Declare(
    googletest
    GIT_REPOSITORY https://github.com/google/googletest.git
    GIT_TAG v1.14.0
)

set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)

FetchContent_MakeAvailable(googletest)

project(
    WBuffer
    VERSION 1.0
    DESCRIPTION "Smart buffer for WiseIO"
    LANGUAGES CXX
)

set(CMAKE_CXX_STANDARD 23)

enable_testing()

add_subdirectory(lib)
