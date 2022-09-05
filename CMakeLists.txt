cmake_minimum_required(VERSION 3.24)

project(
    "NES-emulator"
    VERSION 0.1
    LANGUAGES CXX
)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED 20)

include(cmake/CompilerWarnings.cmake)
add_library(project_warnings INTERFACE)
set_project_warnings(project_warnings)

find_package(fmt REQUIRED)
find_package(SDL2)

add_subdirectory(src)
add_subdirectory(tests)

