#ifndef FIRST_OF_TYPE_HELPER
#define FIRST_OF_TYPE_HELPER

#include <type_traits>

#include "details/first_of_type.h"
#include "args_size.h"

namespace helpers
{

namespace tuple
{

template< typename Target, size_t startIndex = 0, template< typename... > class T, typename... Args >
constexpr size_t first_of_type( const T< Args... >& t )
{      
    static_assert( startIndex <= sizeof...(Args) + 1, "Start index in invalid" );
    return details::_first_of_type< Target, startIndex, 0, Args... >();
}

template< typename Target, size_t startIndex = 0, template< typename... > class T, typename... Args >
constexpr size_t first_of_type( const T< Args... >&& t )
{
    static_assert( startIndex <= sizeof...(Args) + 1, "Start index in invalid" );
    return details::_first_of_type< Target, startIndex, 0, Args... >();
}

}

}

#endif
