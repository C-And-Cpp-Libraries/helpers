#include <type_traits>
#include <tuple>

namespace func_traits {
namespace detail {

template<typename T>
using decay_no_ptr_t = std::remove_pointer_t<std::decay_t<T>>;

template<typename...>
using void_t = void;

template<typename T, typename = void_t<>>
struct extract_fun
{
    using type = decay_no_ptr_t<T>;
};

template<typename T>
struct extract_fun<T, void_t<decltype(&decay_no_ptr_t<T>::operator())>>
{
    using type = decltype(&decay_no_ptr_t<T>::operator());
};

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
struct parser
{
    template<typename U> 
    struct dependent_false : std::false_type{};

    static_assert(
        dependent_false<T>::value,
        "Type is not a function or callable, or no suitable overload for calltype is provided");
};

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

template<typename Func>
using make_parser = parser<typename extract_fun<Func>::type>;

}// detail

template<typename Func>
using result_type = typename detail::make_parser<Func>::result_type;

template<typename Func>
using args_types = typename detail::make_parser<Func>::args_types;

template<size_t Pos, typename Func>
using arg_type_at = std::tuple_element_t<Pos, args_types<Func>>;

template<typename Func>
using first_arg_type = arg_type_at<0, Func>;

template<typename Func>
constexpr size_t args_num = std::tuple_size<args_types<Func>>::value;

template<typename Func>
using mem_func_owner_type = typename detail::make_parser<Func>::owner_type;

template<typename Func>
constexpr bool is_const = detail::make_parser<Func>::is_const;

template<typename Func>
constexpr bool is_volatile = detail::make_parser<Func>::is_volatile;

template<typename Func>
constexpr bool is_member_func = !std::is_same_v<mem_func_owner_type<Func>, detail::non_member_func>;

}// func_traits

#define DEFINE_FUNC_TRAITS_FOR_CALLOPT_WIN_X32(CALL_OPT)                               \
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

DEFINE_FUNC_TRAITS_FOR_CALLOPT_WIN_X32(CALL_CONV)
