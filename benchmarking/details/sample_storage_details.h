#ifndef SAMPLE_STORAGE_DETAILS
#define SAMPLE_STORAGE_DETAILS

#include <type_traits>

namespace helpers
{

namespace benchmarking
{

namespace details
{

template< class T, class U = void >
struct enable_if_type{ using type = U; };

template<class T, class enable = void>
struct is_clock : std::false_type {};

template<class T>
struct is_clock< T, typename enable_if_type< typename T::time_point >::type > : std::true_type{};

template< typename T >
using enable_if_clock = typename std::enable_if< is_clock< T >::value >::type;

template< typename T > struct is_duration : std::false_type {};

template< typename rep, typename period >
struct is_duration< std::chrono::duration< rep, period > > : std::true_type {};

template< typename T >
using enable_if_duration = typename std::enable_if< is_duration< T >::value >::type;

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
