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
if( !( val ) ) throw ___NS_HDET___::test_error{ ___ERROR_TEXT___( "Assert failed: " + #val ) };\

#define DYNAMIC_ASSERT_TEXT( val, error )\
if( !( val ) ) throw ___NS_HDET___::test_error{ ___ERROR_TEXT___( "Assert failed: " +  error ) };\

// Exception policy
#define CHECK_NOTHROW( Func )\
try{ Func; }\
catch( ... ){ throw ___NS_HDET___::test_error{ ___ERROR_TEXT___( "Exception thrown" ) }; }\

#define CHECK_THROW( Func )\
try{ Func; throw ___NS_HDET___::test_error{ "Exception not thrown" }; }\
catch( const ___NS_HDET___::test_error& e ){ throw ___NS_HDET___::test_error{ ___ERROR_TEXT___( e.what() ) }; }\
catch(...){} \

// Test case
#define TEST_CASE( test_name ) \
struct test_name final : public ___NS_HDET___::test_base< test_name >\
{\
    test_name(){ ___run___( #test_name ); }\
    void ___test_impl___();\
};\
test_name test_name##object{};\
void test_name::___test_impl___()\

// Test case fixture
#define TEST_CASE_FIXTURE( test_name, fixture_name ) \
struct test_name final : public ___NS_HDET___::test_base< test_name >, public fixture_name\
{\
    test_name(){ ___run___( #test_name ); }\
    void ___test_impl___();\
};\
test_name test_name##object{};\
void test_name::___test_impl___()\

}// test

}// helpers

// main() ifdef
#ifdef HELPERS_MAIN
int main( int argc, char* argv[] ){
    struct printer{
        ~printer(){ std::cout << "Total tests: " << ___NS_HDET___::___total_tests___ << std::endl
                              << "Failed tests: " <<  ___NS_HDET___::___failed___.size() << std::endl;
                    for( const auto& failed_test : ___NS_HDET___::___failed___ ){ std::cout << failed_test << std::endl; } }
    };
    printer p;

    std::cout << ___SEPARATOR___ << std::endl;
    return ___NS_HDET___::___failed___.empty()? 0 : 1;
}
#endif

#endif
