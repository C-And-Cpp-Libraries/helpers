#ifndef _HELPERS_MEASURE_TIME_DETAILS_H_
#define _HELPERS_MEASURE_TIME_DETAILS_H_

#include <utility>

namespace helpers
{

namespace benchmarking
{

namespace details
{

template< typename Func, typename... Args >
void _exec_func( Func&& f, Args&&... args )
{
    f( std::forward< Args >( args )... );
}

template< typename Func, typename... Args >
typename std::result_of< Func( Args... ) >::type _exec_func_result( Func&& f, Args&&... args )
{
    return f( std::forward< Args >( args )... );
}

}// details

}// benchmarking

}// helpers

#endif
