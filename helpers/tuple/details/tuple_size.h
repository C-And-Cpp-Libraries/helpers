#ifndef _HELPERS_TUPLE_SIZE_DETAILS_H_
#define _HELPERS_TUPLE_SIZE_DETAILS_H_

#include <tuple>
#include "../args_size.h"

namespace helpers
{

namespace tuple
{

namespace details
{

template< template< typename... >  class T, class...Args >
constexpr size_t tuple_size( const T< Args... >& )
{
    return args_size< Args... >();
}

template< template< typename... >  class T, class...Args >
constexpr size_t tuple_size( T< Args... >&& )
{
    return args_size< Args... >();
}

}// details

}// tuple

}// helpers

#endif
