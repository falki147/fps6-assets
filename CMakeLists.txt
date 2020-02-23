cmake_minimum_required(VERSION 3.16)
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

project(fps6builder VERSION 1.0.0 LANGUAGES CXX)

file(GLOB_RECURSE FPS6BUILDERSRC "src/*.cpp" "src/*.hpp")
add_executable(fps6-builder ${FPS6BUILDERSRC})
target_include_directories(fps6-builder PRIVATE "src")
