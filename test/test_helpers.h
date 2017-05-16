#ifndef _HELPERS_TEST_H_
#define _HELPERS_TEST_H_

#include <list>
#include <string>
#include <functional>

#include "../class/non_copyable.h"
#include "../type_traits/type_traits.h"

namespace helpers
{

namespace test
{

struct TEST_RESULT : public classes::non_copyable
{
    friend class TEST_RESULTS_STORAGE;

    bool success{ false };
    std::string test_name;
    std::string error;

private:
    TEST_RESULT( const std::string& testname, bool res, const std::string& err = std::string{} );
};

using TEST_RESULTS = std::list< TEST_RESULT >;

class TEST_RESULTS_STORAGE : public classes::non_copyable_non_movable
{
public:
    TEST_RESULTS_STORAGE() = delete;
    static const TEST_RESULTS& get() noexcept;
    static bool has_fails() noexcept;
    static void reset();
    static void add_result( const std::string& test_name, bool res, const std::string& err = std::string{} );

private:
    static bool m_has_fails;
    static TEST_RESULTS m_results;
};

class test_error : public std::runtime_error{ using std::runtime_error::runtime_error; };

#define STRING( s ) #s
#define TEST_NAME __FUNCTION__
#define ERROR_TEXT(error) std::string{ __FILE__ } + ":" + std::to_string( __LINE__ ) + " | " + error

//

#define DYNAMIC_ASSERT( val )\
if( !( val ) ) throw test_error{ ERROR_TEXT( "Assert failed: " + STRING(val) ) };\

#define DYNAMIC_ASSERT_TEXT( val, error )\
if( !( val ) ) throw test_error{ ERROR_TEXT( "Assert failed: " +  error ) };\

//

#define CHECK_NOTHROW( Func )\
try{ Func; }\
catch( const std::exception& ) \
{ \
    throw helpers::test::test_error{ ERROR_TEXT( "Exception thrown" ) }; \
}\

#define CHECK_THROW( Func )\
try{ Func; throw helpers::test::test_error{ "Exception not thrown" }; }\
catch( const helpers::test::test_error& e )\
{\
    throw helpers::test::test_error{ ERROR_TEXT( e.what() ) }; \
}\
catch(...){} \

#define TEST_CASE( test_name )\
inline void test_name()\
{\
    try{\

#define TEST_CASE_END( test_name ) \
}\
catch( const helpers::test::test_error& e ){ return helpers::test::TEST_RESULTS_STORAGE::add_result( TEST_NAME, false, e.what() ); }\
catch( const std::runtime_error& e ){ return helpers::test::TEST_RESULTS_STORAGE::add_result( TEST_NAME, false, std::string{ "Unhandled exception: " } + e.what() ); }\
catch( ... ){ return helpers::test::TEST_RESULTS_STORAGE::add_result( TEST_NAME, false, "Unhandled exception" ); }\
helpers::test::TEST_RESULTS_STORAGE::add_result( TEST_NAME, true ); } \

// details

}// test

}// helpers

#endif
