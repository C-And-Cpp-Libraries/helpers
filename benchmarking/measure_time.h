#ifndef MEASURE_TIME
#define MEASURE_TIME

#include "details/measure_time.h"
#include "../../helpers/chrono/type_traits.h"
#include "../../helpers/functional/type_traits.h"

namespace helpers
{

namespace benchmarking
{

template< typename Duration_Result_Type, typename Func, typename... Args,
          typename = typename std::enable_if< helpers::chrono::is_duration< Duration_Result_Type >::value >::type,
          typename = typename std::enable_if< helpers::functional::returns_void< Func, Args... >::value >::type >
auto measure_exec_time( Func&& f, Args&&... args ) -> Duration_Result_Type
{
  auto start = std::chrono::system_clock::now();

  details::_exec_func( std::forward< Func >( f ), std::forward< Args >( args )... );

  auto result = std::chrono::system_clock::now() - start;

  return std::chrono::duration_cast< Duration_Result_Type >( result );
}

template< typename Duration_Result_Type, typename Func, typename... Args,
          typename = typename std::enable_if< helpers::chrono::is_duration< Duration_Result_Type >::value >::type,
          typename = typename std::enable_if< !helpers::functional::returns_void< Func, Args... >::value >::type >
auto measure_exec_time( Func&& f, Args&&... args ) -> std::pair< Duration_Result_Type, typename std::result_of< Func(Args...) >::type >
{
  auto start = std::chrono::system_clock::now();

  auto return_val = details::_exec_func_result( std::forward< Func >( f ), std::forward< Args >( args )... );

  auto diff = std::chrono::system_clock::now() - start;
  Duration_Result_Type result = std::chrono::duration_cast< Duration_Result_Type >( diff );

  return std::make_pair( result, std::forward< decltype( return_val ) >( return_val ) );
}

}// benchmarking

}// helpers

#endif
