#ifndef _HELPERS_POLYMORPH_TESTS_H_
#define _HELPERS_POLYMORPH_TESTS_H_

#include <iostream>
#include <functional>

#include "../polymorph/polymorph.h"
#include "../test.h"

using namespace helpers;
using namespace helpers::test;

namespace polymorph_tests
{

void test_poly_ctor_copy_move()
{
    polymorph p1;
    polymorph p2 = std::string{ "p2" };
    polymorph p3{ int{ 1 } };


    TEST_DYNAMIC_ASSERT( p1.empty(), "p1.empty() assert failed" );
    TEST_DYNAMIC_ASSERT( !p2.empty(), "!p2.empty() assert failed" );

    p1 = p2;
    TEST_DYNAMIC_ASSERT( !p1.empty(), "!p1.empty() copy assert failed" );
    TEST_DYNAMIC_ASSERT( !p2.empty(), "!p2.empty() copy assert failed" );

    p3.deep_copy< std::string >( p1 );
    p1.get< std::string >() = "ololo";

    TEST_DYNAMIC_ASSERT( p3.check_type< std::string >(), "p3.check_type() deep copy assert failed" );
    TEST_EXEC_FUNC( TEST_NAME, SHOULD_THROW::NO, [ &p3 ]{ p3.get< std::string >() == "p2"; } );

    p1 = std::move( p2 );
    TEST_DYNAMIC_ASSERT( !p1.empty(), "!p1.empty() move assert failed" );
    TEST_DYNAMIC_ASSERT( p2.empty(), "p2.empty() move assert failed" );
}

void test_poly_type()
{
    polymorph p1;
    polymorph p2 = std::string{ "p2" };

    TEST_DYNAMIC_ASSERT( p1.empty(), "p1.empty() assert failed" );
    TEST_DYNAMIC_ASSERT( !p2.empty(), "!p2.empty() assert failed" );

    TEST_DYNAMIC_ASSERT( !p1.check_type< int >(), "!p1.check_type() assert failed" );
    TEST_DYNAMIC_ASSERT( p2.check_type< std::string >(), "p2.check_type() assert failed" );
    TEST_DYNAMIC_ASSERT( !p2.check_type< int >(), "!p2.check_type() assert failed" );

    TEST_DYNAMIC_ASSERT( p1.type_info() == typeid( void ), "p1.type_info() assert failed" );
    TEST_DYNAMIC_ASSERT( p2.type_info() == typeid( std::string ), "p2.type_info() assert failed" );
    TEST_DYNAMIC_ASSERT( p2.type_info() != typeid( int ), "!p2.type_info() assert failed" );

    TEST_EXEC_FUNC( TEST_NAME, SHOULD_THROW::YES, [ &p1 ]{ const int i = p1.get< int >(); } );
    TEST_EXEC_FUNC( TEST_NAME, SHOULD_THROW::YES, [ &p1 ]{ int i = p1.get< int >(); } );

    TEST_EXEC_FUNC( TEST_NAME, SHOULD_THROW::NO, [ &p2 ]{ const std::string& i = p2.get< std::string >(); } );
    TEST_EXEC_FUNC( TEST_NAME, SHOULD_THROW::NO, [ &p2 ]{ std::string& i = p2.get< std::string >(); } );

    TEST_DYNAMIC_ASSERT( p2.get< std::string >() == "p2", "p2.get() string value assert failed" );

    p2 = int{ 1 };
    TEST_DYNAMIC_ASSERT( p2.get< int >(), "p2.get() int value assert failed" );
}

void run_all_tests()
{
    TEST_CASE( "test_poly_ctor_copy_move", test_poly_ctor_copy_move );
    TEST_CASE( "test_poly_type", test_poly_type );
}

}

#endif
