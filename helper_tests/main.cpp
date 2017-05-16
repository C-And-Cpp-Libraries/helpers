#include "polymorph_tests.h"
#include "thread_pool_tests.h"
#include "temporal_tests.h"

#include <string>
#include <vector>

int main( int argc, char* argv[] )
{
    std::vector< std::string > failed_tests;
    std::vector< std::pair< std::function< void() >, std::string > > tests
    {
         { polymorph_tests::run_all_tests, "polymorph_tests" }
        //,{ thread_pool_tests::run_all_tests, "thread_pool_tests" }
        ,{ temporal_tests::run_all_tests, "temporal_tests" }
    };

    for( auto& test : tests )
    {
        std::cout << test.second << std::endl;
        test.first();

        if( TEST_RESULTS_STORAGE::has_fails() )
        {
          for( const TEST_RESULT& test_result : TEST_RESULTS_STORAGE::get() )
          {
            if( !test_result.success )
            {
                std::cerr << "Test " << test_result.test_name << " failed: "
                          << test_result.error << std::endl;
            }
          }

          failed_tests.emplace_back( test.second );
        }
        else
        {
            std::cout << "Ok!\n--------------\n" << std::endl;
        }

        TEST_RESULTS_STORAGE::reset();
    }

    if( failed_tests.size() )
    {
        std::cout << "Failed tests:" << std::endl;
        for( const std::string& test_name : failed_tests ){ std::cout << test_name << std::endl; };
    }
    else
    {
        std::cout << "All tests OK!" << std::endl;
    }
}
