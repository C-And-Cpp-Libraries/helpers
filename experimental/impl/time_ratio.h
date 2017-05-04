#ifndef _TIME_RATIO_
#define _TIME_RATIO_

#include "time_details.h"
#include "date_time.h"

namespace temporal
{

namespace details
{

template< time_type tick_cnt, time_type sec_period >
class time_ratio final
{
    using curr_time_info = time_ratio< tick_cnt, sec_period >;
    template< time_type, time_type > friend class time_ratio;

    template< time_type ltk, time_type lp, time_type rtk, time_type rp >
    constexpr time_ratio< ltk, lp >
    friend operator+( const time_ratio< ltk, lp >& l, const time_ratio< rtk, rp >& r ) noexcept;

    template< time_type ltk, time_type lp, time_type rtk, time_type rp >
    constexpr time_ratio< ltk, lp >
    friend operator-( const time_ratio< ltk, lp >& l, const time_ratio< rtk, rp >& r ) noexcept;

public:
    // ctors
    constexpr time_ratio() = default;
    constexpr explicit time_ratio( time_type count ) noexcept;
    constexpr explicit time_ratio( const time_moment& tm ) noexcept;
    constexpr explicit time_ratio( const date_time& dt ) noexcept;

    constexpr explicit time_ratio( const timeval& tv ) noexcept;
    constexpr explicit time_ratio( const timespec& ts ) noexcept;

    template< typename std_duration_type, typename = enable_if_std_duration< std_duration_type > >
    constexpr explicit time_ratio( const std_duration_type& duration ) noexcept;

    template< time_type other_tick_cnt, time_type other_sec_period >
    constexpr time_ratio( const time_ratio< other_tick_cnt, other_sec_period >& other ) noexcept;

    template< time_type other_tick_cnt, time_type other_sec_period >
    constexpr time_ratio( time_ratio< other_tick_cnt, other_sec_period >&& other ) noexcept;

    // assignment
    time_ratio& operator=( time_type count ) noexcept;
    time_ratio& operator=( const time_moment& tm ) noexcept;
    time_ratio& operator=( const date_time& dt ) noexcept;

    template< time_type other_tick_cnt, time_type other_sec_period >
    time_ratio& operator=( const time_ratio< other_tick_cnt, other_sec_period >& other ) noexcept;

    template< time_type other_tick_cnt, time_type other_sec_period >
    time_ratio& operator=( time_ratio< other_tick_cnt, other_sec_period >&& other ) noexcept;

    // conversions from
    void from_time_t( time_t time ) noexcept;
    void from_timeval( const timeval& time ) noexcept;
    void from_timespec( const timespec& time ) noexcept;

    template< typename std_duration_type, typename = enable_if_std_duration< std_duration_type > >
    void from_std_duration( const std_duration_type& duration ) noexcept;

    void from_nsec_since_epoch() noexcept;

    // conversions to
    constexpr time_t to_time_t() const noexcept;
    constexpr timeval to_timeval() const noexcept;
    constexpr timespec to_timespec() const noexcept;

    template< typename std_duration_type = std::chrono::nanoseconds,
              typename = enable_if_std_duration< std_duration_type > >
    constexpr std_duration_type to_std_duration() const noexcept;

    constexpr time_type count() const noexcept;
    constexpr time_type count_since_epoch() const noexcept;
    constexpr date_time to_date_time() const noexcept;

    // limits
    static constexpr curr_time_info max() noexcept;

    // operators
    template< time_type other_tick_cnt, time_type other_sec_period >
    constexpr bool operator==( const time_ratio< other_tick_cnt, other_sec_period >& other ) const noexcept;

    template< time_type other_tick_cnt, time_type other_sec_period >
    constexpr bool operator!=( const time_ratio< other_tick_cnt, other_sec_period >& other ) const noexcept;

    time_ratio& operator+=( time_type count ) noexcept;
    time_ratio& operator-=( time_type count ) noexcept;

    template< time_type other_tick_cnt, time_type other_sec_period >
    time_ratio& operator+=( const time_ratio< other_tick_cnt, other_sec_period >& other ) noexcept;

    template< time_type other_tick_cnt, time_type other_sec_period >
    time_ratio& operator-=( const time_ratio< other_tick_cnt, other_sec_period >& other ) noexcept;

private:
    time_moment m_time;
    time_type m_count{ 0 };
    static constexpr curr_time_info m_max{ std::numeric_limits< time_type >::max() };
};

template< time_type ltk, time_type lp, time_type rtk, time_type rp >
constexpr time_ratio< ltk, lp >
operator+( const time_ratio< ltk, lp >& l, const time_ratio< rtk, rp >& r ) noexcept;

template< time_type ltk, time_type lp, time_type rtk, time_type rp >
constexpr time_ratio< ltk, lp >
operator-( const time_ratio< ltk, lp >& l, const time_ratio< rtk, rp >& r ) noexcept;

}// details

using nanosec_t  = details::time_ratio< ratio::ns_ratio_sec,    1 >;
using microsec_t = details::time_ratio< ratio::micro_ratio_sec, 1 >;
using millisec_t = details::time_ratio< ratio::milli_ratio_sec, 1 >;
using seconds_t  = details::time_ratio< ratio::sec_ratio_sec,   1 >;
using minutes_t  = details::time_ratio< 1, ratio::min_ratio_sec >;
using hours_t    = details::time_ratio< 1, ratio::hour_ratio_sec >;
using days_t     = details::time_ratio< 1, ratio::day_ratio_sec >;


}// temporal

#endif

#include "time_ratio_template.impl"