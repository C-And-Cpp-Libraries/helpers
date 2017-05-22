#define TEST_TESTS 0

#if !TEST_TESTS
    #define HELPERS_MAIN
    #include "temporal_tests.h"
    #include "thread_pool_tests.h"
    #include "polymorph_tests.h"
    #include "benchmarking_tests.h"
#else
    #include "test_module_tests.h"
    int main(){ tests_test(); }
#endif
