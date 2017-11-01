#ifndef _HELPERS_TUPLE_ALG_TESTS_H_
#define _HELPERS_TUPLE_ALG_TESTS_H_

#include <tuple>

#include "test.h"
#include "tuple.h"

using namespace helpers::tuple;

namespace tuple_alg_tests
{

TEST_CASE( tuple_utils_test )
{
    static_assert( args_size< int, double, int >() == 3, "args_size fail" );

    {
        constexpr std::tuple< int, float, double, int, float, double > t;

        static_assert( first_of_type< float >( t ) == 1, "first_of_type fail" );
        static_assert( first_of_type< float, 2 >( t ) == 4, "first_of_type fail" );
        static_assert( first_of_type< short >( t ) == 6, "first_of_type fail" );
    }

    {
        std::tuple< std::string, double, std::string > v{ std::string{ "qwerty" }, 3.3, std::string{ "qwerty" } };

        auto f = []( std::string& s ){ s += "q"; };
        for_each_of_type< std::string >( v, f );

        DYNAMIC_ASSERT( std::get< 0 >( v ) == "qwertyq" && std::get< 2 >( v ) == "qwertyq" )
    }

    {
        constexpr std::tuple< float, double, int > t;
        static_assert( type_present< int >( t ), " type_present fail" );
        static_assert( !type_present< short >( t ), " type_present fail" );
    }

    {
        constexpr std::tuple< int, float, int, double, int > t{ 0, 1.1f, 2, 2.2, 2  };
        static_assert( tuple_find< int >( 2, t ) == 2, "find fail" );
        static_assert( tuple_find< int, 3 >( 2, t ) == 4, "find fail" );
        static_assert( tuple_find< int >( 5, t ) == 5, "find fail" );
        static_assert( tuple_find< short >( 2, t ) == 5, "find fail" );
    }
}

}

#endif
