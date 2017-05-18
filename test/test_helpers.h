#ifndef _HELPERS_TEST_H_
#define _HELPERS_TEST_H_

#include <list>
#include <string>
#include <iostream>
#include <functional>
#include <memory>

#include "details/test_helpers_details.h"
#include "../class/non_copyable.h"
#include "../type_traits/type_traits.h"

namespace helpers
{

namespace test
{

// Asserts
#define DYNAMIC_ASSERT( val )\
if( !( val ) ) throw helpers::test::details::test_error{ ERROR_TEXT( "Assert failed: " + #val ) };\

#define DYNAMIC_ASSERT_TEXT( val, error )\
if( !( val ) ) throw helpers::test::test_error{ ERROR_TEXT( "Assert failed: " +  error ) };\

// Exception policy
#define CHECK_NOTHROW( Func )\
try{ Func; }\
catch( ... ){ throw DETAILS::test_error{ ERROR_TEXT( "Exception thrown" ) }; }\

#define CHECK_THROW( Func )\
try{ Func; throw DETAILS::test_error{ "Exception not thrown" }; }\
catch( const DETAILS::test_error& e ){ throw DETAILS::test_error{ ERROR_TEXT( e.what() ) }; }\
catch(...){} \

// Test case
#define TEST_CASE( test_name ) \
struct test_name : public helpers::test::details::test_base\
{\
    test_name(){ run( #test_name ); }\
    void test() override;\
};\
test_name test_name##object{};\
void test_name::test()\

}// test

}// helpers

// main()  ifdef
#ifdef HELPERS_MAIN
int main( int argc, char* argv[] ){
    struct printer{
        ~printer(){ std::cout << "Total tests: " << DETAILS::total << std::endl
                              << "Failed tests: " <<  DETAILS::failed.size() << std::endl;
                    for( const auto& failed_test : DETAILS::failed ){ std::cout << failed_test << std::endl; }
                  }
    };
    printer p;

    std::cout << SEPARATOR << std::endl;
    return DETAILS::failed.empty()? 0 : 1;
}
#endif

#endif
