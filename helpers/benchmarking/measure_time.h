#ifndef _HELPERS_MEASURE_TIME_H_
#define _HELPERS_MEASURE_TIME_H_

#include <functional>

#include "../class/non_copyable.h"
#include "../type_traits/type_traits.h"

namespace helpers
{

namespace benchmarking
{
/// \brief A wrapper around called function,
/// returns elapsed time in specified std::duration.
/// Uses std::chrono::high_resolution_clock under the hood

template< typename duration_result_type,
          typename Func, typename... Args,
          typename = type_traits::enable_if_returns_type< void, Func, Args... > >
auto measure_exec_time( Func&& f, Args&&... args ) -> duration_result_type
{
    static_assert( type_traits::is_duration< duration_result_type >::value, "Invalid duration type" );

    auto start = std::chrono::high_resolution_clock::now();
    f( std::forward< Args >( args )... );
    auto result = std::chrono::high_resolution_clock::now() - start;
    return std::chrono::duration_cast< duration_result_type >( result );
}

/// \brief Same as above, but for functions with non-void return values.

template< typename duration_result_type,
          typename Func, typename... Args,
          typename = type_traits::disable_if_returns_type< void, Func, Args... > >
auto measure_exec_time( Func&& f, Args&&... args ) -> std::pair< duration_result_type, typename std::result_of< Func(Args...) >::type >
{
    static_assert( type_traits::is_duration< duration_result_type >::value, "Invalid duration type" );

    auto start = std::chrono::high_resolution_clock::now();
    auto return_val = f( std::forward< Args >( args )... );
    auto diff = std::chrono::high_resolution_clock::now() - start;
    duration_result_type result = std::chrono::duration_cast< duration_result_type >( diff );
    return std::make_pair( result, std::forward< decltype( return_val ) >( return_val ) );
}

/// \brief A non-copyable and non-movable RAII class used
/// to call execute_on_destroy callback  upon it's destruction.
/// callback must not throw any exceptions, as it's called in destructor

template< typename _duration_type, typename _clock_type >
class scoped_time_handle : public classes::non_copyable_non_movable
{
    static_assert( type_traits::is_duration< _duration_type >::value, "Invalid duration type" );
    static_assert( type_traits::is_clock< _clock_type >::value, "Invalid clock type" );

public:
    using duration_type = _duration_type;
    using clock_type = _clock_type;
    using pred_type = std::function< void( const duration_type& ) >;

    explicit scoped_time_handle( const pred_type& execute_on_destroy ) :
        m_pred( execute_on_destroy ),
        m_start( clock_type::now() )
    {
        if( !execute_on_destroy )
        {
            throw std::invalid_argument{ "Predicate not initialized" };
        }
    }

    ~scoped_time_handle()
    {
        m_pred( std::chrono::duration_cast< duration_type >( clock_type::now() - m_start ) );
    }

private:
    pred_type m_pred;
    typename clock_type::time_point m_start;
};

/// \brief Convenience typedefs

using nanosec_scoped_time_handle  = scoped_time_handle< std::chrono::nanoseconds,  std::chrono::high_resolution_clock >;
using microsec_scoped_time_handle = scoped_time_handle< std::chrono::microseconds, std::chrono::high_resolution_clock >;
using millisec_scoped_time_handle = scoped_time_handle< std::chrono::milliseconds, std::chrono::high_resolution_clock >;
using sec_scoped_time_handle      = scoped_time_handle< std::chrono::seconds,      std::chrono::high_resolution_clock >;
using min_scoped_time_handle      = scoped_time_handle< std::chrono::minutes,      std::chrono::high_resolution_clock >;
using hour_scoped_time_handle     = scoped_time_handle< std::chrono::hours,        std::chrono::high_resolution_clock >;

}// benchmarking

}// helpers

#endif
