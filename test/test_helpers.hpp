#ifndef TESTS_HELPERS
#define TESTS_HELPERS

#include <list>
#include <stdexcept>
#include <functional>

#define TEST_NAME std::string{ __FUNCTION__ }
#define CASE_NAME( case_name ) std::string{ TEST_NAME  + ":" + case_name }

namespace tests
{

struct TEST_RESULT
{
    friend class TEST_RESULTS_STORAGE;

    TEST_RESULT() = delete;

    bool success{ false };
    std::string test_name;
    std::string error;

private:
    TEST_RESULT( const std::string& testname, bool res, const std::string& err = std::string{} ) :
        test_name( testname ), success( res ), error( err ){}
};

using TEST_RESULTS = std::list< TEST_RESULT >;

class TEST_RESULTS_STORAGE
{
    template< typename Test, typename... TestArgs  >
    friend void TEST_CASE( const std::string&, Test&&, TestArgs&&... );

    template< typename Test, typename... TestArgs >
    friend void TEST_CASE_SAFE( const std::string&, std::function< void() >, Test&&, TestArgs&&... );

public:
    TEST_RESULTS_STORAGE() = delete;
    TEST_RESULTS_STORAGE( const TEST_RESULTS_STORAGE& ) = delete;
    TEST_RESULTS_STORAGE& operator=( const TEST_RESULTS_STORAGE& ) = delete;
    TEST_RESULTS_STORAGE( TEST_RESULTS_STORAGE&& ) = delete;
    TEST_RESULTS_STORAGE& operator=( TEST_RESULTS_STORAGE&& ) = delete;

    static const TEST_RESULTS& get() noexcept{ return m_results; }
    static bool has_fails() noexcept{ return m_has_fails; }
    static void reset()
    {
        m_results.clear();
        m_has_fails = false;
    }

private:
    static void add_result( const std::string& test_name, bool res, const std::string& err = std::string{} )
    {
        TEST_RESULT r{ test_name, res, err };
        m_results.emplace_back( r );

        if( !m_has_fails && !res )
        {
            m_has_fails = true;
        };
    }

private:
    static bool m_has_fails;
    static TEST_RESULTS m_results;
};

bool TEST_RESULTS_STORAGE::m_has_fails{ false };
TEST_RESULTS TEST_RESULTS_STORAGE::m_results;

enum THROW
{
    SHOULD_THROW,
    SHOULD_NOT_THROW,
};

namespace _details
{

void CHECK_THROW_COND( THROW cond, const std::string& testname, bool throw_occured, const std::string& error );

}

class test_error : public std::runtime_error{ using std::runtime_error::runtime_error; };

void TEST_ASSERT( bool val, const std::string& error )
{
    if( !val )
    {
        throw test_error( error );
    }
}

std::string ERR_MSG( const std::string& testname, const std::string err_msg )
{
    return { testname + " failed : " + err_msg };
}

template< typename Func, typename... Args >
void TEST_EXEC_FUNC( const std::string& testname, THROW cond, Func&& f, Args&&... args )
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

template< typename Func, typename... Args >
typename std::result_of< Func( Args... ) >::type
TEST_EXEC_FUNC_RESULT( const std::string& testname, THROW cond, Func&& f, Args&&... args )
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

void CHECK_THROW_COND( THROW cond, const std::string& testname, bool throw_occured, const std::string& error )
{
    if( ( cond == SHOULD_NOT_THROW && throw_occured ) ||
        ( cond == SHOULD_THROW && !throw_occured ) )
    {
        throw test_error{ ERR_MSG( testname, error ) };
    }
}

}// details

}// tests

#endif
