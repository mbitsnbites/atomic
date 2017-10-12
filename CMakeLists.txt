cmake_minimum_required(VERSION 3.5)

project(atomic CXX)

# Optionally enable C++11 (we use C++11 as a fallback for some operations).
set(CMAKE_CXX_STANDARD 11)

# Add the atomic library.
add_library(atomic INTERFACE)
target_include_directories(atomic INTERFACE include)

# Add the unit tests.
enable_testing()
add_subdirectory(third_party)
add_subdirectory(test)
