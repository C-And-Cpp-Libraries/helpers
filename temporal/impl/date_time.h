#ifndef _DATE_TIME_
#define _DATE_TIME_

#include <string>
#include "time_details.h"

namespace temporal
{

namespace details
{

template< time_type, time_type > class time_ratio;

}// details

struct verbose_date_time;

class date_time final
{
    template< time_type, time_type > friend class details::time_ratio;

public:
    enum dt_month{ jan = 1, feb, mar, apr, may, jun, jul, aug, sep, oct, nov, dec };

    constexpr date_time() = default;
    constexpr explicit date_time( const details::time_moment& tm ) : m_time( tm ){}
    date_time( int64_t year, dt_month month, time_type day,
               time_type hour, time_type min, time_type sec, time_type nsec );

    explicit date_time( const verbose_date_time& vdt );

    verbose_date_time to_verbose_date_time() const noexcept;

    int64_t year() const noexcept;
    dt_month month() const noexcept;
    time_type day() const noexcept;
    time_type hour() const noexcept;
    time_type minute() const noexcept;
    time_type sec() const noexcept;
    time_type nsec() const noexcept;

    bool is_leap() const noexcept;
    int day_of_week() const noexcept;

    // Avaliable patterns:
    // yr - year, mn - month, d - day, w -day of weak, hr - hour,
    // min - minute, sec - second, ms - millisec, us - microsec, ns - nanosec
    std::string to_string( const std::string& pattern = {} ) const noexcept;

    constexpr bool operator<( const date_time& other ) const noexcept
    {
        return m_time < other.m_time;
    }

    constexpr bool operator>( const date_time& other ) const noexcept
    {
        return m_time > other.m_time;
    }

    bool operator<=( const date_time& other ) const noexcept
    {
        return m_time <= other.m_time;
    }

    bool operator>=( const date_time& other ) const noexcept
    {
        return m_time >= other.m_time;
    }
    bool operator==( const date_time& other ) const noexcept
    {
        return m_time == other.m_time;
    }

    bool operator!=( const date_time& other ) const noexcept
    {
        return m_time != other.m_time;
    }

private:
    constexpr const details::time_moment& get_time_moment() const noexcept{ return m_time; }

private:
    details::time_moment m_time;
};

struct verbose_date_time
{
    time_type day{ 0 };
    date_time::dt_month month{ date_time::jan };
    int64_t year{ 0 };
    time_type hour{ 0 };
    time_type min{ 0 };
    time_type sec{ 0 };
    time_type nsec{ 0 };
};

}// temporal

inline std::ostream& operator<<( std::ostream& os, const temporal::date_time& dt )
{
    os << dt.to_string();
    return os;
}

#endif
