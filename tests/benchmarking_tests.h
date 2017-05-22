#ifndef _HELPERS_BENCHMARKING_TESTS_H_
#define _HELPERS_BENCHMARKING_TESTS_H_

#include <thread>

#include "../test.h"
#include "../benchmarking.h"

using namespace helpers::benchmarking;

namespace benchmarking_tests
{

TEST_CASE( measure_exec_time_test )
{
    auto test_func_void = [](){ std::this_thread::sleep_for( std::chrono::milliseconds{ 100 } ); };
    auto test_func_int = []( int i ){ std::this_thread::sleep_for( std::chrono::milliseconds{ 100 } ); return i + 1; };
    using dur_type = std::chrono::milliseconds;
    dur_type dur;
    int i{ 0 };
    std::pair< dur_type, int > result;

    CHECK_NOTHROW( dur = measure_exec_time< dur_type >( test_func_void ) )
            DYNAMIC_ASSERT( dur.count() >= 99 && dur.count() <= 101 )

    CHECK_NOTHROW( result = measure_exec_time< dur_type >( test_func_int, i ) )
    DYNAMIC_ASSERT( result.first.count() >= 99 && result.first.count() <= 101 )
    DYNAMIC_ASSERT( result.second == i + 1 )
}

TEST_CASE( scope_time_handle_test )
{
    using dur_type = std::chrono::milliseconds;
    dur_type result;

    auto test_func = [ &result ]()
    {
        millisec_scope_time_handle m{ [ &result ]( const dur_type& d ){ result = d; } };
        std::this_thread::sleep_for( std::chrono::milliseconds{ 100 } );
    };

    millisec_scope_time_handle::pred_type p_uninit;

    CHECK_THROW( millisec_scope_time_handle h1{ p_uninit } )
    CHECK_NOTHROW( test_func() )
    DYNAMIC_ASSERT( result.count() >= 99 && result.count() <=101 )
}

TEST_CASE( sample_storage_test )
{
    using sample_storage_type = sample_storage< int, std::chrono::milliseconds >;
    sample_storage_type s;

    CHECK_THROW( s.samples( 2 ) )
    CHECK_THROW( s.average_time( 2 ) )
    DYNAMIC_ASSERT( !s.key_present( 2 ) )

    for( int i{ 0 }; i < 5; ++i )
    {
        CHECK_NOTHROW( s.add_timestamp( 0 ) )
        CHECK_NOTHROW( s.add_timestamp( 1 ) )
        std::this_thread::sleep_for( sample_storage_type::sample_type{ 100 } );
        CHECK_NOTHROW( s.add_timestamp( 0 ) )
        CHECK_NOTHROW( s.add_timestamp( 1 ) )
    }

    sample_storage_type::sample_type avr1, avr2;
    DYNAMIC_ASSERT( s.key_present( 0 ) && s.key_present( 1 ) )
    CHECK_NOTHROW( s.samples( 0 ); s.samples( 1 ); )
    DYNAMIC_ASSERT( s.samples( 0 ).size() == 5 && s.samples( 1 ).size() == 5 )
    CHECK_NOTHROW( avr1 = s.average_time( 0 ); avr2 = s.average_time( 1 ); )
    DYNAMIC_ASSERT( avr1.count() >= 99 && avr1.count() <= 101 )
    DYNAMIC_ASSERT( avr2.count() >= 99 && avr2.count() <= 101 )
}

}

#endif
