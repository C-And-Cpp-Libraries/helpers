cmake_minimum_required(VERSION 3.2)
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set (PROJECT helper_tests)
project( ${PROJECT} )

SET( CMAKE_BUILD_TYPE Debug )
set( SRC_FOLDER "../helpers" )

include_directories(${SRC_FOLDER}
                    ${SRC_FOLDER}/test
                    ${SRC_FOLDER}/polymorph
                    ${SRC_FOLDER}/concurrency
                    ${SRC_FOLDER}/temporal
                    ${SRC_FOLDER}/benchmarking
                    ${SRC_FOLDER}/tuple
                    ${SRC_FOLDER}/type_traits)

file( GLOB_RECURSE SOURCES "*.h" "*.cpp" )
file( GLOB_RECURSE TEST_SOURCES "${SRC_FOLDER}/test/*" )
file( GLOB_RECURSE POLY_SOURCES "${SRC_FOLDER}/polymorph/*" )
file( GLOB_RECURSE CONCURRENCY_SOURCES "${SRC_FOLDER}/concurrency/*" )
file( GLOB_RECURSE TEMPORAL_SOURCES "${SRC_FOLDER}/temporal/*" )
file( GLOB_RECURSE BENCHMARKING_SOURCES "${SRC_FOLDER}/benchmarking/*" )
file( GLOB_RECURSE TUPLE_SOURCES "${SRC_FOLDER}/tuple/*" )
file( GLOB_RECURSE TYPE_TRAITS_SOURCES "${SRC_FOLDER}/type_traits/*" )

source_group( tests FILES ${SOURCES} )
source_group( test_module FILES ${TEST_SOURCES} )
source_group( polymorph FILES ${POLY_SOURCES} )
source_group( concurrency FILES ${CONCURRENCY_SOURCES} )
source_group( benchmarking FILES ${BENCHMARKING_SOURCES} )
source_group( temporal FILES ${TEMPORAL_SOURCES} )
source_group( tuple FILES ${TUPLE_SOURCES} )
source_group( type_traits FILES ${TYPE_TRAITS_SOURCES} )

add_executable( ${PROJECT}
                ${SOURCES}
                ${TEST_SOURCES}
                ${POLY_SOURCES}
                ${CONCURRENCY_SOURCES}
                ${TEMPORAL_SOURCES}
                ${BENCHMARKING_SOURCES}
                ${TUPLE_SOURCES}
				${TYPE_TRAITS_SOURCES} )

if (UNIX)
    target_link_libraries( ${PROJECT} pthread )
endif (UNIX)
