#include "../test_helpers.h"

namespace helpers
{

namespace test
{

namespace details
{

static uint32_t ___total_tests___{ 0 };
static std::list< std::string > ___failed___;

#define ___SEPARATOR___ "-----------------"
#define ___ERROR_TEXT___( error ) ( std::string{ __FILE__ } + ":" + std::to_string( __LINE__ ) + " | " + error )
#define ___TEST_START_TEXT___( test_name ) std::cout << ___SEPARATOR___ << "\nRunning " << test_name << std::endl;
#define ___CASE_OK___( case_name ) std::cout << case_name << " passed\n" << ___SEPARATOR___ << std::endl;
#define ___CASE_FAIL___( case_name, error ) \
helpers::test::details::___failed___.emplace_back( case_name );\
std::cerr << case_name << " failed: " << error << "\n" << ___SEPARATOR___ << std::endl; return;\

class test_error : public std::runtime_error{ using std::runtime_error::runtime_error; };

template< class derived >
struct test_base
{
public:
    void ___run___( const char* test_name ) noexcept
    {
        ++___total_tests___;
        std::cout << "Running " << test_name << std::endl;
        try{ static_cast< derived* >( this )->___test_impl___(); }
        catch( const test_error& e ){ ___CASE_FAIL___( test_name, e.what() )  }
        catch( ... ){ ___CASE_FAIL___( test_name, "Unhandled exception" ) }
        ___CASE_OK___( test_name )
    }
};

#define ___NS_HDET___ helpers::test::details

}// details

}// test

}// helpers
