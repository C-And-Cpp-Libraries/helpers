#ifndef _HELPERS_TEST_TESTS_H_
#define _HELPERS_TEST_TESTS_H_

#include <cassert>
#include "../test.h"

std::pair< bool, bool > call_asserts( bool value, const std::string& text )
{
    std::pair< bool, bool > thrown{ false, false };

    try{ DYNAMIC_ASSERT( value ); }
    catch( const helpers::test::details::test_error& ){
        thrown.first = true;
    }

    try{ DYNAMIC_ASSERT_TEXT( value, text ); }
    catch( const helpers::test::details::test_error& ){
        thrown.second = true;
    }

    return thrown;
}

bool regular_test_started{ false };
bool fixture_test_started{ false };

struct fixture_struct
{
    void test_call()
    {
        fixture_test_started = true;
        throw std::runtime_error{ "ololo" };
    }
};

TEST_CASE( test_case_regular )
{
    regular_test_started = true;
}

TEST_CASE_FIXTURE( test_case_fixture, fixture_struct )
{
    test_call();
}

void tests_test()
{
    auto thrown = call_asserts( true, "ololo" );
    assert( thrown.first == false && thrown.second == false );
    thrown = call_asserts( false, "ololo" );
    assert( thrown.first == true && thrown.second == true );

    auto throw_f = [](){ throw std::runtime_error{ "" }; };
    auto nothrow_f = [](){};

    try{ CHECK_NOTHROW( nothrow_f() ); }
    catch( ... ){ assert( false ); }

    try{ CHECK_NOTHROW( throw_f() ); assert( false ); }
    catch( const helpers::test::details::test_error& ){}
    catch( ... ){ assert( false ); }

    try{ CHECK_THROW( throw_f() ); }
    catch( ... ){ assert( false ); }

    try{ CHECK_THROW( nothrow_f() ); assert( false ); }
    catch( const helpers::test::details::test_error& ){}
    catch( ... ){ assert( false ); }
    assert( regular_test_started == true && fixture_test_started == true );
    assert( helpers::test::details::___total_tests___ == 2 );
    assert( helpers::test::details::___failed___.size() == 1 &&
            helpers::test::details::___failed___.front() == "test_case_fixture" );
}

#endif
