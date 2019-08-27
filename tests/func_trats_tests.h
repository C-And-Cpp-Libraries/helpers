#pragma once

#include <functional>
#include "func_traits.h"

#ifdef WIN32
  #define CALL_CONV __cdecl
  DEFINE_FUNC_TRAITS_FOR_CALLOPT_WIN_X32(CALL_CONV)
#else
  #define CALL_CONV
#endif

#include <functional>

namespace func_traits_tests
{

int doo(int, double&, const size_t*) { return 0; }
int CALL_CONV foo(int, double&, const size_t*) noexcept { return 0; }

struct Functions
{
    virtual int doo(int, double&, const size_t*) noexcept = 0;
    virtual int doo_const_volatile(int, double&, const size_t*) const volatile noexcept = 0;
    virtual int CALL_CONV foo(int, double&, const size_t*) noexcept = 0;
    virtual int CALL_CONV foo_const(int, double&, const size_t*) const noexcept = 0;
};

struct CallableStruct
{
    int CALL_CONV operator()(int, double&, const size_t*) const volatile;
};

struct PlatformSpecific;

template<bool IsConst, bool IsVolatile, typename ClassType, typename T>
void Check(T&&)
{
    using args = func_traits::args_types<T>;
    using result = func_traits::result_type<T>;
    using first_arg = func_traits::first_arg_type<T>;
    using first_arg1 = func_traits::arg_type_at<0, T>;
    using second_arg = func_traits::arg_type_at<1, T>;
    using third_arg = func_traits::arg_type_at<2, T>;
    using owner_type = func_traits::mem_func_owner_type<T>;
    constexpr bool is_member_func{ 
        !std::is_same<owner_type, func_traits::detail::non_member_func>::value };

    static_assert(std::is_same<result, int>::value, "result");
    static_assert(func_traits::args_num<T> == 3, "args num");
    static_assert(std::is_same<first_arg, int>::value, "first arg");
    static_assert(std::is_same<first_arg1, int>::value, "first arg typedef");
    static_assert(std::is_same<second_arg, double&>::value, "second arg");
    static_assert(std::is_same<third_arg, const size_t*>::value, "third arg");
    static_assert(func_traits::is_const<T> == IsConst, "const");
    static_assert(func_traits::is_volatile<T> == IsVolatile, "volatile");
    static_assert(
        std::is_same<func_traits::mem_func_owner_type<T>, ClassType>::value ||
        std::is_same<ClassType, PlatformSpecific>::value, "owner type");
    static_assert(func_traits::is_member_func<T> == is_member_func, "is member func");
}

void RunTests()
{
    const auto const_foo_ptr = foo;

    Check<true, false, PlatformSpecific>([](int, double&, const size_t*)->int {return 1; });
    Check<false, false, PlatformSpecific>([](int, double&, const size_t*) mutable->int {return 1; });
    Check<true, false, PlatformSpecific>(std::function<int(int, double&, const size_t*)>{});
    Check<true, false, PlatformSpecific>(std::function<int CALL_CONV(int, double&, const size_t*)>{});
    Check<false, false, func_traits::detail::non_member_func>(doo);
    Check<false, false, func_traits::detail::non_member_func>(foo);
    Check<false, false, func_traits::detail::non_member_func>(&doo);
    Check<false, false, func_traits::detail::non_member_func>(&foo);
    Check<false, false, func_traits::detail::non_member_func>(std::move(foo));
    Check<false, false, func_traits::detail::non_member_func>(const_foo_ptr);
    Check<false, false, Functions>(&Functions::doo);
    Check<true, true, Functions>(&Functions::doo_const_volatile);
    Check<false, false, Functions>(&Functions::foo);
    Check<true, false, Functions>(&Functions::foo_const);
    Check<true, true, CallableStruct>(CallableStruct{});
}

}
