#ifndef _HELPERS_FIRST_OF_TYPE_DETAILS_H_
#define _HELPERS_FIRST_OF_TYPE_DETAILS_H

#include "../args_size.h"
#include <type_traits>

namespace helpers
{

namespace tuple
{

namespace details
{

template< typename Target,
          size_t startIndex = 0,
          size_t index,
          typename T, typename... Tail,
          typename std::enable_if< args_size< Tail... >() == 0 >::type* = nullptr >
constexpr size_t _first_of_type()
{
    return std::is_same< T, Target >::value && index >= startIndex?
                index : index + 1;
}

template< typename Target,
          size_t startIndex = 0,
          size_t index = 0,
          typename T, typename... Tail,
          typename std::enable_if< args_size< Tail... >() != 0 >::type* = nullptr >
constexpr size_t _first_of_type()
{
    return std::is_same< T, Target >::value &&  index >= startIndex?
                index : _first_of_type< Target, startIndex, index + 1, Tail... >();
}

}// details

}// tuple

}// helpers

#endif
