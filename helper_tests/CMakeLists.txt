cmake_minimum_required(VERSION 3.2)
add_definitions( "-std=c++11" )

set (PROJECT helper_tests)
project( ${PROJECT} )

SET( CMAKE_BUILD_TYPE Debug )

file( GLOB_RECURSE SOURCES
"*.h"
"*.cpp" )

file( GLOB_RECURSE TEST_SOURCES "../test/*" )
file( GLOB_RECURSE POLY_SOURCES "../polymorph/*" )
file( GLOB_RECURSE THREAD_POOL_SOURCES "../concurrency/*" )
file( GLOB_RECURSE TEMPORAL_SOURCES "../temporal/*" )
file( GLOB_RECURSE BENCHMARKING_SOURCES "../benchmarking/*" )
file( GLOB_RECURSE TUPLE_SOURCES "../tuple/*" )

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
