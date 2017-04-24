#include "polymorph_tests.h"

int main( int argc, char* argv[] )
{
    polymorph_tests::run_all_tests();

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
    }
    else
    {
        std::cout << "Tests ok!" << std::endl;
    }
}
