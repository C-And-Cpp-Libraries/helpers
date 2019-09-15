#ifndef _HELPERS_TYPE_TRAITS_H_
#define _HELPERS_TYPE_TRAITS_H_

#include <chrono>
#include <type_traits>

namespace helpers
{

namespace type_traits
{

namespace details
{

template< typename T, typename U = void >
struct enable_if_type{ using type = U; };
          
template<typename T> struct wrap{};
template<typename... T> struct inherit : wrap<T>...{};

}
          
template<typename T, typename... Types>
struct is_any_of
{
    static constexpr bool any_of{ std::is_base_of<
              detail::wrap<T>,
              detail::inherit<Types...>>::value };
};

template< typename >
struct is_duration : std::false_type{};

template< typename rep, typename period >
struct is_duration< std::chrono::duration< rep, period > > : std::true_type{};

template< typename clock_type,
          typename enable_time_point = void,
          typename enable_rep = void,
          typename enable_period = void,
          typename enable_duration = void >
struct is_clock : std::false_type{};

template< typename clock_type >
struct is_clock< clock_type,
                 typename details::enable_if_type< typename clock_type::time_point >::type,
                 typename details::enable_if_type< typename clock_type::rep >::type,
                 typename details::enable_if_type< typename clock_type::period >::type,
                 typename details::enable_if_type< typename clock_type::duration >::type >
{
private:
    template< typename > static constexpr std::false_type check( ... );

    template< typename blank >
    static constexpr auto check( blank* ) -> typename
    std::is_same< decltype( std::declval< clock_type >().now() ), typename clock_type::time_point >::type;

    using type = decltype( check< clock_type >( nullptr ) );

public:
    static constexpr bool value{ type::value };
};

template< typename result_type, typename Func, typename... Args >
struct returns_type
{
    static constexpr bool value{ std::is_same< typename std::result_of< Func( Args... ) >::type, result_type >::value };
};

//

template< bool val, typename type = void >
using disable_if = std::enable_if< !val, type >;

template< typename T >
using enable_if_duration = typename std::enable_if< is_duration< T >::value >::type;

template< typename T >
using enable_if_clock = typename std::enable_if< is_clock< T >::value >::type;

template< typename result_type, typename Func, typename... Args >
using enable_if_returns_type = typename std::enable_if< returns_type< result_type, Func, Args... >::value >::type;

template< typename result_type, typename Func, typename... Args >
using disable_if_returns_type = typename disable_if< returns_type< result_type, Func, Args... >::value >::type;


}// type_traits

}// helpers

#endif
