#ifndef FOR_EACH_OF_TYPE_DETAILS
#define FOR_EACH_OF_TYPE_DETAILS

#include <tuple>
#include <utility>

#include "first_of_type.h"

namespace helpers
{

namespace tuple
{

namespace details
{

template< typename Target, typename Functor, size_t index, size_t size, class T,
          typename std::enable_if< index >= size >::type* = nullptr >
void _for_each_of_type( T&&, Functor&& )
{

}

template< typename Target, typename Functor, size_t index, size_t size, class T,
          typename std::enable_if< index <= size - 1  >::type* = nullptr >
void _for_each_of_type( T&& t, Functor&& f )
{
    f( std::get< index >( t ) );

    constexpr size_t next_index{ first_of_type< Target, index + 1 >( std::forward< T >( t ) ) };
    _for_each_of_type< Target, Functor, next_index, size >( std::forward< T >( t ), std::forward< Functor >( f ) );
}

}

}

}

#endif
