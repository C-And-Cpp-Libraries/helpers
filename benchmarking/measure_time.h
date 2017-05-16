#ifndef MEASURE_TIME
#define MEASURE_TIME

#include "details/measure_time.h"
#include "../type_traits/type_traits.h"

namespace helpers
{

namespace benchmarking
{

template< typename duration_result_type, typename Func, typename... Args,
          typename = type_traits::enable_if_duration< duration_result_type >,
          typename = type_traits::enable_if_returns_type< void, Func, Args... > >
auto measure_exec_time( Func&& f, Args&&... args ) -> duration_result_type
{
  auto start = std::chrono::system_clock::now();

  details::_exec_func( std::forward< Func >( f ), std::forward< Args >( args )... );

  auto result = std::chrono::system_clock::now() - start;

  return std::chrono::duration_cast< duration_result_type >( result );
}

template< typename duration_result_type, typename Func, typename... Args,
          typename = type_traits::enable_if_duration< duration_result_type >,
          typename = type_traits::disable_if_returns_type< void, Func, Args... > >
auto measure_exec_time( Func&& f, Args&&... args ) -> std::pair< duration_result_type, typename std::result_of< Func(Args...) >::type >
{
  auto start = std::chrono::system_clock::now();

  auto return_val = details::_exec_func_result( std::forward< Func >( f ), std::forward< Args >( args )... );

  auto diff = std::chrono::system_clock::now() - start;
  duration_result_type result = std::chrono::duration_cast< duration_result_type >( diff );

  return std::make_pair( result, std::forward< decltype( return_val ) >( return_val ) );
}

template< typename _duration_type,
          typename _clock_type,
          typename = type_traits::enable_if_duration< _duration_type >,
          typename = type_traits::enable_if_clock< _clock_type > >
class time_measure_pred final
{
public:
    using duration_type = _duration_type;
    using clock_type = _clock_type;
    using pred_type = std::function< void( const duration_type& ) >;

    explicit time_measure_pred( const pred_type& execute_on_destroy ) :
        m_pred( execute_on_destroy ),
        m_start( clock_type::now() )
    {
        if( !execute_on_destroy )
        {
            throw std::invalid_argument{ "Predicate not initialized" };
        }
    }

    ~time_measure_pred()
    {
        m_pred( std::chrono::duration_cast< duration_type >( clock_type::now() - m_start ) );
    }

private:
    pred_type m_pred;
    typename clock_type::time_point m_start;
};

using nanosec_time_measure_pred = time_measure_pred< std::chrono::nanoseconds, std::chrono::high_resolution_clock >;
using microsec_time_measure_pred = time_measure_pred< std::chrono::microseconds, std::chrono::high_resolution_clock >;
using millisec_time_measure_pred = time_measure_pred< std::chrono::milliseconds, std::chrono::high_resolution_clock >;
using sec_time_measure_pred = time_measure_pred< std::chrono::seconds, std::chrono::high_resolution_clock >;
using min_time_measure_pred = time_measure_pred< std::chrono::minutes, std::chrono::high_resolution_clock >;
using hour_time_measure_pred = time_measure_pred< std::chrono::hours, std::chrono::high_resolution_clock >;

}// benchmarking

}// helpers

#endif
