#include "../test_helpers.h"

namespace helpers
{

namespace test
{

TEST_RESULT::TEST_RESULT( const std::string& testname, bool res, const std::string& err ) :
    non_copyable(),
    test_name( testname ), success( res ), error( err ){}

bool TEST_RESULTS_STORAGE::m_has_fails{ false };
TEST_RESULTS TEST_RESULTS_STORAGE::m_results;

const TEST_RESULTS& TEST_RESULTS_STORAGE::get() noexcept
{
    return m_results;
}

bool TEST_RESULTS_STORAGE::has_fails() noexcept
{
    return m_has_fails;
}

void TEST_RESULTS_STORAGE::reset()
{
    m_results.clear();
    m_has_fails = false;
}

void TEST_RESULTS_STORAGE::add_result( const std::string& test_name, bool res, const std::string& err )
{
    TEST_RESULT r{ test_name, res, err };
    m_results.emplace_back( std::move( r ) );

    if( !m_has_fails && !res )
    {
        m_has_fails = true;
    };
}

void TEST_DYNAMIC_ASSERT( bool val, const std::string& error )
{
    if( !val )
    {
        throw test_error( error );
    }
}

namespace _details
{

#define ERR_MSG( testname, err_msg ) std::string{ testname + " failed : " + err_msg }

void CHECK_THROW_COND( const SHOULD_THROW& cond, const std::string& testname, bool throw_occured, const std::string& error )
{
    if( ( cond == SHOULD_THROW::NO && throw_occured ) ||
        ( cond == SHOULD_THROW::YES && !throw_occured ) )
    {
        throw test_error{ ERR_MSG( testname, error ) };
    }
}

}

}// test

}// helpers
