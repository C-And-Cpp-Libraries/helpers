#ifndef SAMPLE_STORAGE_DETAILS
#define SAMPLE_STORAGE_DETAILS

#include <type_traits>
#include "../../type_traits/type_traits.h"

namespace helpers
{

namespace benchmarking
{

namespace details
{

template< typename T >
using decay_t = typename std::decay< T >::type;

template< typename T >
using val_or_ref = typename std::conditional< std::is_arithmetic< decay_t< T > >::value,
                                              decay_t< T >,
                                              const decay_t< T >& >::type;

}// details

}// benchmarking

}// helpers

#endif
