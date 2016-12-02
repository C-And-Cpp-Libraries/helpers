#ifndef TUPLE_TYPE_PRESENT_HELPER
#define TUPLE_TYPE_PRESENT_HELPER

#include <type_traits>
#include "details/type_present.h"

namespace helpers
{

namespace tuple
{

// Checks if Target type is among the listed

template< typename Target, template< typename... > class T, typename... Args >
constexpr bool type_present( const T< Args... >& t )
{
    return details::_type_present< Target, Args... >::value;
}

template< typename Target, template< typename... > class T, typename... Args >
constexpr bool type_present( const T< Args... >&& t )
{
    return details::_type_present< Target, Args... >::value;
}

}

}

#endif
