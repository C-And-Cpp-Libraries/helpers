#ifndef FUNCTIONAL_HELPERS
#define FUNCTIONAL_HELPERS

#include <type_traits>

namespace misc_tools
{

namespace helpers
{

namespace functional
{

template< typename Func, typename... Args >
struct returns_void
{
    static constexpr bool value = std::is_same< typename std::result_of< Func( Args... ) >::type, void >::value;
};

}

}

}

#endif
