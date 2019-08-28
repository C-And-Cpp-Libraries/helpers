#include <type_traits>
#include <tuple>

namespace func_traits {
namespace detail {

template<typename T>
using decay_no_ptr_t = std::remove_pointer_t<std::decay_t<T>>;

template<typename...>
using void_t = void;

template<typename T, typename = void_t<>>
struct extract_func_type
{
    using type = T;
};

template<typename T> 
struct extract_func_type<T, void_t<decltype(&T::operator())>>
{
    using type = decltype(&T::operator());
};

enum func_qualifiers{ no_qual_ = 0, const_ = 1, volatile_ = 2 }; // TODO: noexcept

constexpr func_qualifiers operator|(func_qualifiers l, func_qualifiers r) noexcept
{
    return static_cast<func_qualifiers>(int{ l } | int{ r });
}

using non_member_func = void_t<>;

template<func_qualifiers Qualifiers, typename Owner, typename Ret, typename... Args>
struct meta
{
    static constexpr func_qualifiers qualifiers{ Qualifiers };
    using owner_type = Owner;
    using result_type = Ret;
    using args_types = std::tuple<Args...>;
};

template<typename T>
struct parser
{
    template<typename U>
    struct dependent_false : std::false_type {};

    static_assert(dependent_false<T>::value,
        "Type is not a function or callable, or no suitable overload for calltype is provided");
};

template<typename Ret, typename... Args>
struct parser<Ret(Args...)> : meta<no_qual_, non_member_func, Ret, Args...> {};

template<typename T, typename Ret, typename... Args>
struct parser<Ret(T::*)(Args...)> : meta<no_qual_, T, Ret, Args...> {};

template<typename T, typename Ret, typename... Args>
struct parser<Ret(T::*)(Args...) const> : meta<const_, T, Ret, Args...> {};

template<typename T, typename Ret, typename... Args>
struct parser<Ret(T::*)(Args...) volatile> : meta<volatile_, T, Ret, Args...> {};

template<typename T, typename Ret, typename... Args>
struct parser<Ret(T::*)(Args...) const volatile> : meta<const_ | volatile_, T, Ret, Args...> {};

template<typename Func>
using parse = parser<typename extract_func_type<decay_no_ptr_t<Func>>::type>;

template<typename Func, func_qualifiers Qualifier>
constexpr bool has_qualifier = !!(parse<Func>::qualifiers & Qualifier);

}// detail

template<typename Func>
using result_type = typename detail::parse<Func>::result_type;

template<typename Func>
using args_types = typename detail::parse<Func>::args_types;

template<size_t Pos, typename Func>
using arg_type_at = std::tuple_element_t<Pos, args_types<Func>>;

template<typename Func>
using first_arg_type = arg_type_at<0, Func>;

template<typename Func>
constexpr size_t args_num = std::tuple_size<args_types<Func>>::value;

template<typename Func>
using mem_func_owner = typename detail::parse<Func>::owner_type;

template<typename Func>
constexpr bool is_const = detail::has_qualifier<Func, detail::const_>;

template<typename Func>
constexpr bool is_volatile = detail::has_qualifier<Func, detail::volatile_>;

template<typename Func>
constexpr bool is_member_func = !std::is_same_v<mem_func_owner<Func>, detail::non_member_func>;

}// func_traits

#define DEFINE_FUNC_TRAITS_FOR_CALLOPT(CALL_OPT)                                        \
    template<typename Ret, typename... Args>                                            \
    struct func_traits::detail::parser<Ret(CALL_OPT *)(Args...)> :                      \
        parser<Ret(Args...)> {};                                                        \
    template<typename T, typename Ret, typename... Args>                                \
    struct func_traits::detail::parser<Ret(CALL_OPT T::*)(Args...)> :                   \
        parser<Ret(T::*)(Args...)> {};                                                  \
    template<typename T, typename Ret, typename... Args>                                \
    struct func_traits::detail::parser<Ret(CALL_OPT T::*)(Args...) const> :             \
        parser<Ret(T::*)(Args...) const> {};                                            \
    template<typename T, typename Ret, typename... Args>                                \
    struct func_traits::detail::parser<Ret(CALL_OPT T::*)(Args...) volatile> :          \
        parser<Ret(T::*)(Args...) volatile> {};                                         \
    template<typename T, typename Ret, typename... Args>                                \
    struct func_traits::detail::parser<Ret(CALL_OPT T::*)(Args...) const volatile> :    \
        parser<Ret(T::*)(Args...) const volatile> {};                                   \
