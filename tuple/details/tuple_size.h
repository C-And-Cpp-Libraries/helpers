#ifndef TUPLE_SIZE
#define TUPLE_SIZE

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

}

}

}

#endif
