#ifndef TUPLE_FIND_DETAILS
#define TUPLE_FIND_DETAILS

#include <tuple>
#include <utility>

namespace helpers
{

namespace tuple
{

namespace details
{

template< typename Target, size_t index, size_t tuple_size, typename T,
          typename std::enable_if< index >= tuple_size >::type* = nullptr >
constexpr size_t _tuple_find( Target&&, T&& )
{
    return tuple_size;
}

template< typename Target, size_t index, size_t tuple_size, typename T,
          typename std::enable_if< index <= tuple_size -1 >::type* = nullptr >
constexpr size_t _tuple_find( Target&& target, T&& t  )
{
    return std::get< index >( t ) == target || index >= tuple_size?
                index :
                _tuple_find< Target, first_of_type< Target, index + 1 >( t ), tuple_size >
                            ( std::forward< Target >( target ), std::forward< T >( t ) );
}

}

}

}

#endif
