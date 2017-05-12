#ifndef TYPE_TRAITS
#define TYPE_TRAITS

#include <chrono>
#include <type_traits>

namespace helpers
{

namespace type_traits
{

template< class T, class U = void >
struct enable_if_type{ using type = U; };

// chrono

template< typename >
struct is_duration : std::false_type{};

template< typename Rep, typename Period >
struct is_duration< std::chrono::duration< Rep, Period > > : std::true_type{};

template< typename T >
using enable_if_duration = typename std::enable_if< is_duration< T >::value >::type;

template<class T, class enable = void>
struct is_clock : std::false_type {};

template<class T>
struct is_clock< T, typename enable_if_type< typename T::time_point >::type > : std::true_type{};

template< typename T >
using enable_if_clock = typename std::enable_if< is_clock< T >::value >::type;

//

template< typename Func, typename... Args >
struct returns_void
{
    static constexpr bool value = std::is_same< typename std::result_of< Func( Args... ) >::type, void >::value;
};

template< typename Func, typename... Args >
using enable_if_return_void = typename std::enable_if< returns_void< Func, Args... >::value >::type;

template< typename Func, typename... Args >
using disable_if_return_void = typename std::enable_if< !returns_void< Func, Args... >::value >::type;


}// type_traits

}// helpers

#endif
