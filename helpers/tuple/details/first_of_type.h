#ifndef _HELPERS_FIRST_OF_TYPE_DETAILS_H_
#define _HELPERS_FIRST_OF_TYPE_DETAILS_H_

#include <type_traits>
#include "../args_size.h"

namespace helpers
{

namespace tuple
{

namespace details
{

template< typename Target,
          size_t start_index = 0,
          size_t index,
          typename T, typename... Tail,
          typename std::enable_if< args_size< Tail... >() == 0 >::type* = nullptr >
constexpr size_t _first_of_type()
{
    return std::is_same< T, Target >::value && index >= start_index?
                index : index + 1;
}

template< typename Target,
          size_t start_index = 0,
          size_t index = 0,
          typename T, typename... Tail,
          typename std::enable_if< args_size< Tail... >() != 0 >::type* = nullptr >
constexpr size_t _first_of_type()
{
    return std::is_same< T, Target >::value &&  index >= start_index?
                index : _first_of_type< Target, start_index, index + 1, Tail... >();
}

}// details

}// tuple

}// helpers

#endif
