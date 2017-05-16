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

}// test

}// helpers
