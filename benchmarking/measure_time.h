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
          typename = type_traits::enable_if_return_void< Func, Args... > >
auto measure_exec_time( Func&& f, Args&&... args ) -> duration_result_type
{
  auto start = std::chrono::system_clock::now();

  details::_exec_func( std::forward< Func >( f ), std::forward< Args >( args )... );

  auto result = std::chrono::system_clock::now() - start;

  return std::chrono::duration_cast< duration_result_type >( result );
}

template< typename duration_result_type, typename Func, typename... Args,
          typename = type_traits::enable_if_duration< duration_result_type >,
          typename = type_traits::disable_if_return_void< Func, Args... > >
auto measure_exec_time( Func&& f, Args&&... args ) -> std::pair< duration_result_type, typename std::result_of< Func(Args...) >::type >
{
  auto start = std::chrono::system_clock::now();

  auto return_val = details::_exec_func_result( std::forward< Func >( f ), std::forward< Args >( args )... );

  auto diff = std::chrono::system_clock::now() - start;
  duration_result_type result = std::chrono::duration_cast< duration_result_type >( diff );

  return std::make_pair( result, std::forward< decltype( return_val ) >( return_val ) );
}

}// benchmarking

}// helpers

#endif
