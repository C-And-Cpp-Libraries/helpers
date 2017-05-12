#ifndef TUPLE_TYPE_PRESENT_DETAILS
#define TUPLE_TYPE_PRESENT_DETAILS

#include <type_traits>

namespace helpers
{

namespace tuple
{


namespace details
{

template< typename Target, typename Curr, typename... Tail>
struct _type_present : std::conditional< std::is_same< Target, Curr >::value,
                                         std::true_type,
                                         _type_present< Target, Tail... > >::type
{

};

template< typename Target, typename Curr >
struct _type_present< Target, Curr > : std::conditional< std::is_same< Target, Curr >::value,
                                                         std::true_type,
                                                         std::false_type >::type
{

};

}// details

}// tuple

}// helpers

#endif
