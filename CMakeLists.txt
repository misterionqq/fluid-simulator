cmake_minimum_required(VERSION 3.27)
project(fluid_simulator)

set(CMAKE_CXX_STANDARD 20)

add_executable(fluid-simulator Fixed.h
        vector-field.h
        crutches.h
        fluid-creator.h
        flags-parser.h
        main.cpp
)

target_compile_definitions(fluid-simulator PRIVATE
        DTYPES=FLOAT,DOUBLE,FIXED\(32,7\),FIXED\(32,5\),FAST_FIXED\(52,13\),FAST_FIXED\(37,11\)
        DSIZES=BASESIZE\(10,250\),BASESIZE\(36,84\),BASESIZE\(14,5\)
)

add_executable(cleaner saved-data-cleaner.cpp)