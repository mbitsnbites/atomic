# We require C++11 for the tests (to be able to create threads etc).
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# We also need the platform threads package.
find_package(Threads REQUIRED)

# Add the unit test executable.
add_executable(atomic_test atomic_test.cpp)
target_link_libraries(atomic_test atomic doctest ${CMAKE_THREAD_LIBS_INIT})
add_test(atmoic_test atmoic_test)
