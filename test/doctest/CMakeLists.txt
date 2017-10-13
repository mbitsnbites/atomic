add_library(doctest
            include/doctest.h
            doctest_main.cpp
            )
target_include_directories(doctest PUBLIC include)

