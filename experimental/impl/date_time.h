#ifndef _DATE_TIME_
#define _DATE_TIME_

#include "time_details.h"

namespace time_info
{

namespace details
{

template< time_type, time_type > class time_ratio;

}

class date_time final
{
    template< time_type, time_type > friend class details::time_ratio;

public:
    enum dt_month{ jan = 1, feb, mar, apr, may, jun, jul, aug, sep, oct, nov, dec };

    constexpr date_time() = default;
    explicit date_time( const details::time_moment& tm );
    date_time( int64_t year, dt_month month, time_type day, 
               time_type hour, time_type min, time_type sec, time_type nsec );

    time_type year() const noexcept;
    dt_month month() const noexcept;
    time_type day() const noexcept;
    time_type hour() const noexcept;
    time_type minute() const noexcept;
    time_type sec() const noexcept;
    time_type nsec() const noexcept;

    bool is_leap() const noexcept;
    time_type day_of_week() const noexcept;

    bool operator<( const date_time& other ) const noexcept;
    bool operator>( const date_time& other ) const noexcept;
    bool operator<=( const date_time& other ) const noexcept;
    bool operator>=( const date_time& other ) const noexcept;
    bool operator==( const date_time& other ) const noexcept;
    bool operator!=( const date_time& other ) const noexcept;

private:
    const details::time_moment& get_time_moment() const noexcept;

private:
    details::time_moment m_tm;
};

}// time_info

#endif
