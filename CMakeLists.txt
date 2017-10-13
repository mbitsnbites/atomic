cmake_minimum_required(VERSION 3.2)

project(atomic CXX)

# Optionally enable C++11 (we use C++11 as a fallback for some operations).
set(CMAKE_CXX_STANDARD 11)

# Work around a bug in CMake 3.2 find_package(Threads).
if(${CMAKE_VERSION} VERSION_LESS "3.5.1")
  enable_language(C)
endif()

# Add the atomic library.
add_library(atomic INTERFACE)
target_sources(atomic INTERFACE
    ${CMAKE_CURRENT_SOURCE_DIR}/include/atomic/atomic.h
    ${CMAKE_CURRENT_SOURCE_DIR}/include/atomic/spinlock.h
    )
target_include_directories(atomic INTERFACE include)

# Add the unit tests.
enable_testing()
add_subdirectory(third_party)
add_subdirectory(test)
