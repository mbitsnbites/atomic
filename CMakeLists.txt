cmake_minimum_required(VERSION 3.5)

project(atomic CXX)

# Require C++11.
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Add the atomic library.
add_library(atomic INTERFACE)
target_include_directories(atomic INTERFACE include)

# Add the unit tests.
enable_testing()
add_subdirectory(third_party)
add_subdirectory(test)
