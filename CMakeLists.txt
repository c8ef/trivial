cmake_minimum_required(VERSION 3.20)

set(CMAKE_C_COMPILER clang)
set(CMAKE_CXX_COMPILER clang++)

project(trivial)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS ON)

add_compile_options(-Wall -stdlib=libc++)
add_link_options(-stdlib=libc++ -fuse-ld=lld)

include(FetchContent)

FetchContent_Declare(
    argparse
    GIT_REPOSITORY https://github.com/p-ranav/argparse.git
)
FetchContent_MakeAvailable(argparse)

include_directories(src)
file(GLOB_RECURSE SOURCES "src/*.cpp")

add_executable(trivial ${SOURCES})
target_link_libraries(trivial argparse)
