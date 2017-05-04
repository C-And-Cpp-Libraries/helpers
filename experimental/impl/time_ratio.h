#ifndef _TIME_INFO_
#define _TIME_INFO_

#include "time_details.h"
#include "date_time.h"
#ifdef _WIN32
#include <Winsock2.h> // for timeval
#endif

namespace time_info
{

namespace details
{

template< time_type tick_cnt, time_type period >
class time_ratio final
{
    using curr_time_info = time_ratio< tick_cnt, period >;
    template< time_type, time_type > friend class time_ratio;

public:
    // ctors
    constexpr time_ratio() = default;
    explicit time_ratio( time_type count ) noexcept;
    explicit time_ratio( const time_moment& tm ) noexcept;
    explicit time_ratio( const date_time& dt ) noexcept;

    explicit time_ratio( const timeval& tv ) noexcept;
    explicit time_ratio( const timespec& ts ) noexcept;

    template< typename std_duration_type, typename = enable_if_std_duration< std_duration_type > >
    explicit time_ratio( const std_duration_type& duration ) noexcept;

	template< time_type tick_cnt, time_type period >
    time_ratio( const time_ratio< tick_cnt, period >& other ) noexcept;

	template< time_type tick_cnt, time_type period >
    time_ratio( time_ratio< tick_cnt, period >&& other ) noexcept;

    // assignment
    time_ratio& operator=( time_type count ) noexcept;
    time_ratio& operator=( const time_moment& tm ) noexcept;
    time_ratio& operator=( const date_time& dt ) noexcept;

	template< time_type tick_cnt, time_type period >
    time_ratio& operator=( const time_ratio< tick_cnt, period >& other ) noexcept;

	template< time_type tick_cnt, time_type period >
    time_ratio& operator=( time_ratio< tick_cnt, period >&& other ) noexcept;

    // conversions from
    void from_time_t( time_t time ) noexcept;
    void from_timeval( const timeval& time );
    void from_timespec( const timespec& time );

    template< typename std_duration_type, typename = enable_if_std_duration< std_duration_type > >
    void from_std_duration( const std_duration_type& duration );

    void from_nsec_since_epoch();

    // conversions to
    time_t to_time_t() const noexcept;
    timeval to_timeval() const noexcept;
    timespec to_timespec() const noexcept;

    template< typename std_duration_type = std::chrono::nanoseconds,
              typename = enable_if_std_duration< std_duration_type > >
    std_duration_type to_std_duration() const noexcept;

	time_type count() const noexcept;
    time_type count_since_epoch() const noexcept;	
	inline date_time to_date_time() const noexcept;

	// operators
	template< time_type tick_cnt, time_type period >
	bool operator==( const time_ratio< tick_cnt, period >& other ) const noexcept;

	template< time_type tick_cnt, time_type period >
	bool operator!=( const time_ratio< tick_cnt, period >& other ) const noexcept;

	time_ratio& operator+=( time_type count ) noexcept;
	time_ratio& operator-=( time_type count ) noexcept;

	template< time_type tick_cnt, time_type period >
	time_ratio& operator+=( const time_ratio< tick_cnt, period >& other ) noexcept;

	template< time_type tick_cnt, time_type period >
	time_ratio& operator-=( const time_ratio< tick_cnt, period >& other ) noexcept;

private:
    time_moment m_time;
    time_type m_count{ 0 };
};

template< time_type tick_cnt, time_type period >
time_ratio< tick_cnt, period > operator+( const time_ratio< tick_cnt, period >& l, const time_ratio< tick_cnt, period >& r ) noexcept;

template< time_type tick_cnt, time_type period >
time_ratio< tick_cnt, period > operator-(const time_ratio< tick_cnt, period >& l, const time_ratio< tick_cnt, period >& r) noexcept;

}// details

using nanosec_t  = details::time_ratio< ratio::ns_ratio_sec,    1 >;
using microsec_t = details::time_ratio< ratio::micro_ratio_sec, 1 >;
using millisec_t = details::time_ratio< ratio::milli_ratio_sec, 1 >;
using seconds_t  = details::time_ratio< ratio::sec_ratio_sec,   1 >;
using minutes_t  = details::time_ratio< 1, ratio::min_ratio_sec >;
using hours_t    = details::time_ratio< 1, ratio::hour_ratio_sec >;
using days_t     = details::time_ratio< 1, ratio::day_ratio_sec >;


}// time_info

#endif

#include "time_ratio_template.impl"
