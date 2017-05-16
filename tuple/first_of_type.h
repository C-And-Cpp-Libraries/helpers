#ifndef _HELPERS_FIRST_OF_TYPE_H_
#define _HELPERS_FIRST_OF_TYPE_H_

#include <type_traits>

#include "details/first_of_type.h"
#include "args_size.h"

namespace helpers
{

namespace tuple
{

template< typename Target, size_t start_index = 0, template< typename... > class T, typename... Args >
constexpr size_t first_of_type( const T< Args... >& t )
{
    static_assert( start_index <= sizeof...(Args) + 1, "Start index in invalid" );
    return details::_first_of_type< Target, start_index, 0, Args... >();
}

template< typename Target, size_t start_index = 0, template< typename... > class T, typename... Args >
constexpr size_t first_of_type( const T< Args... >&& t )
{
    static_assert( start_index <= sizeof...(Args) + 1, "Start index in invalid" );
    return details::_first_of_type< Target, start_index, 0, Args... >();
}

}

}

#endif
