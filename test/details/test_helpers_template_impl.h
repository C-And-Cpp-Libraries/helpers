#ifndef TESTS_HELPERS_TEMPLATE_IMPL
#define TESTS_HELPERS_TEMPLATE_IMPL

#include <stdexcept>
#include "../../type_traits/type_traits.h"

namespace helpers
{

namespace test
{

namespace _details
{

void CHECK_THROW_COND( const SHOULD_THROW& cond, const std::string& testname, bool throw_occured, const std::string& error );

}

class test_error : public std::runtime_error{ using std::runtime_error::runtime_error; };

template< typename Func, typename... Args,
          typename = type_traits::enable_if_return_void< Func, Args... > >
void TEST_EXEC_FUNC( const std::string& testname, const SHOULD_THROW& cond, Func&& f, Args&&... args )
{
    bool throw_occured{ false };
    std::string error;

    try
    {
        f( std::forward< Args >( args )... );
    }
    catch( const std::exception& e )
    {
        error = e.what();
        throw_occured = true;
    }

    _details::CHECK_THROW_COND( cond, testname, throw_occured, error );
}

template< typename Func, typename... Args,
          typename = type_traits::disable_if_return_void< Func, Args... > >
typename std::result_of< Func( Args... ) >::type
TEST_EXEC_FUNC( const std::string& testname, const SHOULD_THROW& cond, Func&& f, Args&&... args )
{
    bool throw_occured{ false };
    std::string error;

    try
    {
        auto res = f( std::forward< Args >( args )... );
        _details::CHECK_THROW_COND( cond, testname, throw_occured, error );
        return res;

    }
    catch( const std::exception& e )
    {
        error = e.what();
        throw_occured = true;

        _details::CHECK_THROW_COND( cond, testname, throw_occured, error );
    }
}

template< typename Test, typename... TestArgs >
void TEST_CASE_SAFE( const std::string& test_name, std::function< void() > OnFail, Test&& f, TestArgs&&... args )
{
    bool result{ true };
    std::string error;

    try
    {
        f( std::forward< TestArgs >( args )... );
    }
    catch( const std::exception& e )
    {
        OnFail();
        result = false;
        error = e.what();
    }

    TEST_RESULTS_STORAGE::add_result( test_name, result, error );
}

template< typename Test, typename... TestArgs >
void TEST_CASE( const std::string& test_name, Test&& f, TestArgs&&... args )
{
    bool result{ true };
    std::string error;

    try
    {
        f( std::forward< TestArgs >( args )... );
    }
    catch( const std::exception& e )
    {
        result = false;
        error = e.what();
    }

    TEST_RESULTS_STORAGE::add_result( test_name, result, error );
}

namespace _details
{

void CHECK_THROW_COND( const SHOULD_THROW& cond, const std::string& testname, bool throw_occured, const std::string& error );

}

// details

}// test

}// helpers

#endif
