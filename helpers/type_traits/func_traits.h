#pragma once

#include <type_traits>
#include <tuple>

namespace func_traits {
namespace detail {

template<typename...>
using void_t = void;

template<typename T, typename = void_t<>> 
struct is_callable_object : std::false_type {};

template<typename T>
struct is_callable_object<T, void_t<decltype(&T::operator())>> : std::true_type {};

template<typename T>
using decay_no_ptr_t = std::remove_pointer_t<std::decay_t<T>>;

enum class const_{ yes, no };
enum class volatile_{ yes, no };
using non_member_func = void_t<>;

template<const_ IsConst, volatile_ IsVolatile, typename Owner, typename Ret, typename... Args>
struct meta
{
    static constexpr bool is_const{ IsConst == const_::yes };
    static constexpr bool is_volatile{ IsVolatile == volatile_::yes };
    using owner_type = Owner;
    using result_type = Ret;
    using args_types = std::tuple<Args...>;
};

template<typename T>
struct parser;

template<typename Ret, typename... Args>
struct parser<Ret(Args...)> :
    meta<const_::no, volatile_::no, non_member_func, Ret, Args...> {};

template<typename T, typename Ret, typename... Args>
struct parser<Ret(T::*)(Args...)> :
    meta<const_::no, volatile_::no, T, Ret, Args...> {};

template<typename T, typename Ret, typename... Args>
struct parser<Ret(T::*)(Args...) const> :
    meta<const_::yes, volatile_::no, T, Ret, Args...> {};

template<typename T, typename Ret, typename... Args>
struct parser<Ret(T::*)(Args...) volatile> :
    meta<const_::no, volatile_::yes, T, Ret, Args...> {};

template<typename T, typename Ret, typename... Args>
struct parser<Ret(T::*)(Args...) const volatile> :
    meta<const_::yes, volatile_::yes, T, Ret, Args...> {};

template<typename CallableObject>
struct parser :parser<decltype(&CallableObject::operator())> {};

template<typename T, typename StrippedType = decay_no_ptr_t<T>>
struct traits : parser<StrippedType>
{
    static_assert(
        std::is_function_v<StrippedType> ||
        std::is_member_function_pointer_v<StrippedType> ||
        is_callable_object<StrippedType>::value,
        "Type is not a function or callable, or no suitable overload for calltype is provided");
};

}// detail

template<typename Func>
using result_type = typename detail::traits<Func>::result_type;

template<typename Func>
using args_types = typename detail::traits<Func>::args_types;

template<size_t Pos, typename Func>
using arg_type_at = std::tuple_element_t<Pos, args_types<Func>>;

template<typename Func>
using first_arg_type = arg_type_at<0, Func>;

template<typename Func>
constexpr size_t args_num = std::tuple_size<args_types<Func>>::value;

template<typename Func>
using mem_func_owner_type = typename detail::traits<Func>::owner_type;

template<typename Func>
constexpr bool is_const = detail::traits<Func>::is_const;

template<typename Func>
constexpr bool is_volatile = detail::traits<Func>::is_volatile;

template<typename Func>
constexpr bool is_member_func = !std::is_same_v<mem_func_owner_type<Func>, detail::non_member_func>;

}// func_traits

#define DEFINE_func_traits_FOR_CALLOPT_WIN_X32(CALL_OPT)                               \
    template<typename Ret, typename... Args>                                           \
    struct func_traits::detail::parser<Ret(CALL_OPT *)(Args...)> :                     \
                    func_traits::detail::parser<Ret(Args...)> {};                      \
    template<typename T, typename Ret, typename... Args>                               \
    struct func_traits::detail::parser<Ret(CALL_OPT T::*)(Args...)> :                  \
                    func_traits::detail::parser<Ret(T::*)(Args...)>{};                 \
    template<typename T, typename Ret, typename... Args>                               \
    struct func_traits::detail::parser<Ret(CALL_OPT T::*)(Args...) const> :            \
                    func_traits::detail::parser<Ret(T::*)(Args...) const>{};           \
    template<typename T, typename Ret, typename... Args>                               \
    struct func_traits::detail::parser<Ret(CALL_OPT T::*)(Args...) volatile> :         \
                    func_traits::detail::parser<Ret(T::*)(Args...) volatile>{};        \
    template<typename T, typename Ret, typename... Args>                               \
    struct func_traits::detail::parser<Ret(CALL_OPT T::*)(Args...) const volatile> :   \
                    func_traits::detail::parser<Ret(T::*)(Args...) const volatile>{};  \
