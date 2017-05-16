#ifndef _HELPERS_TEST_H_
#define _HELPERS_TEST_H_

#include <list>
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
    template< typename Test, typename... TestArgs  >
    friend void TEST_CASE( const std::string&, Test&&, TestArgs&&... );

    template< typename Test, typename... TestArgs >
    friend void TEST_CASE_SAFE( const std::string&, std::function< void() >, Test&&, TestArgs&&... );

public:
    TEST_RESULTS_STORAGE() = delete;
    static const TEST_RESULTS& get() noexcept;
    static bool has_fails() noexcept;
    static void reset();

private:
    static void add_result( const std::string& test_name, bool res, const std::string& err = std::string{} );

private:
    static bool m_has_fails;
    static TEST_RESULTS m_results;
};

#define TEST_NAME std::string{ __FUNCTION__ }
#define CASE_NAME( case_name ) std::string{ TEST_NAME  + ":" + case_name }

void TEST_DYNAMIC_ASSERT( bool val, const std::string& error );

enum class SHOULD_THROW{ YES, NO };

template< typename Func, typename... Args,
          typename = type_traits::enable_if_returns_type< void, Func, Args... > >
void TEST_EXEC_FUNC( const std::string& testname, const SHOULD_THROW& cond, Func&& f, Args&&... args );

template< typename Func, typename... Args,
          typename = type_traits::disable_if_returns_type< void, Func, Args... > >
typename std::result_of< Func( Args... ) >::type
TEST_EXEC_FUNC( const std::string& testname, const SHOULD_THROW& cond, Func&& f, Args&&... args );

template< typename Test, typename... TestArgs >
void TEST_CASE( const std::string& test_name, Test&& f, TestArgs&&... args );

template< typename Test, typename... TestArgs >
void TEST_CASE_SAFE( const std::string& test_name, std::function< void() > OnFail, Test&& f, TestArgs&&... args );

// details

}// test

}// helpers

#endif

#include "details/test_helpers_template_impl.h"
