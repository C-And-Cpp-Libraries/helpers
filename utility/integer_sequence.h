#ifndef _HELPERS_INTEGER_SEQUENCE_H_
#define _HELPERS_INTEGER_SEQUENCE_H_

///\brief std::integer_sequence implementation for C++11

#include <cstddef>
#include <type_traits>

namespace std
{

template< class T, T... Ints >
struct integer_sequence;

namespace details
{

template< typename T, size_t N, T... Ints>
struct make_integer_sequence_impl : make_integer_sequence_impl< T, N - 1, N - 1, Ints... >{};

template< typename T, T... Ints>
struct make_integer_sequence_impl< T, 0u, Ints... > : integer_sequence< T, Ints... >
{ using integer_sequence_type = integer_sequence< T, Ints... >; };

}

template< typename T, T... values >
struct integer_sequence{};

template< typename T, size_t N >
using make_integer_sequence = typename details::make_integer_sequence_impl< T, N >::integer_sequence_type;

template< size_t N >
using make_index_sequence = make_integer_sequence< size_t, N >;

template< class... T >
using index_sequence_for = std::make_index_sequence< sizeof...( T ) >;

}
