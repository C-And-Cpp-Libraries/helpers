#ifndef _HELPERS_POLYMORPH_TESTS_H_
#define _HELPERS_POLYMORPH_TESTS_H_

#include <iostream>
#include <functional>

#include "test.h"
#include "polymorph.h"

using namespace helpers;

namespace polymorph_tests
{

TEST_CASE( test_poly_ctor_copy_move )
{
    polymorph p1;
    polymorph p2 = std::string{ "p2" };
    polymorph p3{ int{ 1 } };


    DYNAMIC_ASSERT( p1.empty() )
    DYNAMIC_ASSERT( !p2.empty() )

    p1 = p2;
    DYNAMIC_ASSERT( !p1.empty() )
    DYNAMIC_ASSERT( !p2.empty() )

    p3.deep_copy< std::string >( p1 );
    p1.get< std::string >() = "ololo";

    DYNAMIC_ASSERT( p3.check_type< std::string >() )

    std::string temp;
    CHECK_NOTHROW( temp = p3.get< std::string >() )
    DYNAMIC_ASSERT( temp == "p2" )

    p1 = std::move( p2 );
    DYNAMIC_ASSERT( !p1.empty() )
    DYNAMIC_ASSERT( p2.empty() )
}

TEST_CASE( test_poly_type )
{
    polymorph p1;
    polymorph p2 = std::string{ "p2" };

    DYNAMIC_ASSERT( p1.empty() )
    DYNAMIC_ASSERT( !p2.empty() )

    DYNAMIC_ASSERT( !p1.check_type< int >() )
    DYNAMIC_ASSERT( p2.check_type< std::string >() )
    DYNAMIC_ASSERT( !p2.check_type< int >() )

    DYNAMIC_ASSERT( p1.type_info() == typeid( void ) )
    DYNAMIC_ASSERT( p2.type_info() == typeid( std::string ) )
    DYNAMIC_ASSERT( p2.type_info() != typeid( int ) )

    CHECK_THROW( const int i = p1.get< int >() )
    CHECK_THROW( int i = p1.get< int >() )

    CHECK_NOTHROW( const std::string& i = p2.get< std::string >() )
    CHECK_NOTHROW( std::string& i = p2.get< std::string >() )

    DYNAMIC_ASSERT( p2.get< std::string >() == "p2" )

    p2 = int{ 1 };
    DYNAMIC_ASSERT( p2.get< int >() > 0 )
}

}

#endif
