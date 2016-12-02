#ifndef CHRONO_HELPERS
#define CHRONO_HELPERS

#include <type_traits>
#include <chrono>

namespace misc_tools
{

namespace helpers
{

namespace chrono
{

template< typename >
struct is_duration : std::false_type{};

template< typename Rep, typename Period >
struct is_duration< std::chrono::duration< Rep, Period > > : std::true_type{};

}

}

}

#endif
