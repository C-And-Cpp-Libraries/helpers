#ifndef _HELPERS_LIMITS_H_
#define _HELPERS_LIMITS_H_

#include <limits>
#include <type_traits>

namespace hlp {

// More concise initialization syntax
// constexpr int i = hlp::numeric_limits::max();

struct numeric_limits
{
    struct max
    {
        template <typename T, std::enable_if_t<std::is_arithmetic_v<T>>...>
        constexpr operator T() const noexcept
        {
            return std::numeric_limits<T>::max();
        }
    };

    struct min
    {
        template <typename T, std::enable_if_t<std::is_arithmetic_v<T>>...>
        constexpr operator T() const noexcept
        {
            return std::numeric_limits<T>::min();
        }
    };
    
    // TODO: add the rest of them
};

}// namespace hlp

#endif
