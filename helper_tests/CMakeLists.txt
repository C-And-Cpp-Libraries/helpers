cmake_minimum_required(VERSION 3.2)
add_definitions( "-std=c++11" )

set (PROJECT helper_tests)
project( ${PROJECT} )

SET( CMAKE_BUILD_TYPE Debug )

file( GLOB_RECURSE SOURCES "*.h" "*.cpp" )

set(TEST_SOURCES ../test/test_helpers.h
                 ../test/details/test_helpers_impl.cpp)

set(POLY_SOURCES ../polymorph/polymorph.h
                 ../polymorph/details/polymorph.cpp
                 ../polymorph/details/polymorph.impl)

add_executable (${PROJECT} ${SOURCES} ${TEST_SOURCES} ${POLY_SOURCES})
