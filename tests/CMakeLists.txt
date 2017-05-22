cmake_minimum_required(VERSION 3.2)
add_definitions( "-std=c++11" )

set (PROJECT helper_tests)
project( ${PROJECT} )

SET( CMAKE_BUILD_TYPE Debug )

file( GLOB_RECURSE SOURCES
"*.h"
"*.cpp" )

set( SRC_FOLDER "../helpers" )

file( GLOB_RECURSE TEST_SOURCES "${SRC_FOLDER}/test/*" )
file( GLOB_RECURSE POLY_SOURCES "${SRC_FOLDER}/polymorph/*" )
file( GLOB_RECURSE THREAD_POOL_SOURCES "${SRC_FOLDER}/concurrency/*" )
file( GLOB_RECURSE TEMPORAL_SOURCES "${SRC_FOLDER}/temporal/*" )
file( GLOB_RECURSE BENCHMARKING_SOURCES "${SRC_FOLDER}/benchmarking/*" )
file( GLOB_RECURSE TUPLE_SOURCES "${SRC_FOLDER}/tuple/*" )

add_executable (${PROJECT}
                ${SOURCES}
                ${TEST_SOURCES}
                ${POLY_SOURCES}
                ${THREAD_POOL_SOURCES}
                ${TEMPORAL_SOURCES}
                ${BENCHMARKING_SOURCES}
                ${TUPLE_SOURCES}
                                )
if (UNIX)
    target_link_libraries( ${PROJECT} pthread )
endif (UNIX)
