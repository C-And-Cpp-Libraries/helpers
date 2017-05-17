#include "../test_helpers.h"

namespace helpers
{

namespace test
{

namespace details
{

static uint32_t total{ 0 };
static std::list< std::string > failed;

#define SEPARATOR "-----------------"
#define ERROR_TEXT( error ) std::string{ __FILE__ } + ":" + std::to_string( __LINE__ ) + " | " + error
#define TEST_START_TEXT( test_name ) std::cout << SEPARATOR << "\nRunning " << test_name << std::endl;
#define CASE_OK( case_name ) std::cout << case_name << " passed\n" << SEPARATOR << std::endl;
#define CASE_FAIL( case_name, error ) \
helpers::test::details::failed.emplace_back( case_name );\
std::cerr << case_name << " failed: " << error << "\n" << SEPARATOR << std::endl; return;\

class test_error : public std::runtime_error{ using std::runtime_error::runtime_error; };

struct test_base
{
public:
    virtual void test(){}
    void run( const char* test_name )
    {
        ++total;

        std::cout << "Running " << test_name << std::endl;
        try{ test(); }
        catch( const test_error& e ){ CASE_FAIL( test_name, e.what() )  }
        catch( ... ){ CASE_FAIL( test_name, "Unhandled exception" ) }
        CASE_OK( test_name )
    }
};

#define DETAILS helpers::test::details

}// details

}// test

}// helpers
