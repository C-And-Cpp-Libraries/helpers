#ifndef _TIME_DETAILS_
#define _TIME_DETAILS_

#include <chrono>

namespace time_info
{

using time_type = uintmax_t;
class date_time;

class time_moment final
{
public:
    constexpr time_moment() = default;
	time_moment(const time_moment&) = default;
    explicit time_moment( time_type nsec ) noexcept;
    time_moment( time_type sec, time_type nsec ) noexcept;

    bool operator<( const time_moment& other ) const noexcept;
    bool operator>( const time_moment& other ) const noexcept;
    bool operator<=(const time_moment& other ) const noexcept;
    bool operator>=( const time_moment& other ) const noexcept;
    bool operator==( const time_moment& other ) const  noexcept;
    bool operator!=( const time_moment& other ) const  noexcept;
    time_moment& operator+=( const time_moment& other ) noexcept;
    time_moment& operator-=( const time_moment& other ) noexcept;

    constexpr time_type sec() const noexcept{ return m_sec; }
    constexpr time_type nsec() const noexcept{ return m_nsec; }

    void set_sec( time_type sec ) noexcept;
    void set_nsec( time_type nsec );
    void set( time_type sec, time_type nsec );

    date_time to_date_time() const noexcept;    

private:
    time_type m_sec{ 0 };
    time_type m_nsec{ 0 };
};

time_moment operator+( const time_moment& l, const time_moment& r ) noexcept;
time_moment operator-( const time_moment& l, const time_moment& r ) noexcept;

class date_time final
{
public:
    enum dt_month{ jan = 1, feb, mar, apr, may, jun, jul, aug, sep, oct, nov, dec };

    constexpr date_time() = default;
    date_time( time_type year, dt_month month, time_type day, time_type hour,
               time_type min, time_type sec, time_type nsec = 0 );

    time_moment to_time_moment() const noexcept;

    bool operator<( const date_time& other ) const noexcept;
    bool operator>( const date_time& other ) const noexcept;
    bool operator<=( const date_time& other ) const noexcept;
    bool operator>=( const date_time& other ) const noexcept;
    bool operator==( const date_time& other ) const noexcept;
    bool operator!=( const date_time& other ) const noexcept;

    constexpr time_type year() const noexcept{ return m_year; }
    constexpr dt_month month() const noexcept{ return m_month; }
    constexpr time_type day() const noexcept{ return m_day; }
    constexpr time_type hour() const noexcept{ return m_hour; }
    constexpr time_type min() const noexcept{ return m_min; }
    constexpr time_type sec() const noexcept{ return m_sec; }
    constexpr time_type nsec() const noexcept{ return m_nsec; }

    void set_year( time_type year );
    void set_month( dt_month month );
    void set_day( time_type day );
    void set_hour( time_type hour );
    void set_min( time_type min );
    void set_sec( time_type sec );
    void set_nsec( time_type nsec );

    void set( time_type m_year, dt_month m_month, time_type m_day, time_type m_hour,
              time_type m_min, time_type m_sec, time_type m_nsec = 0 );

private:
    time_type m_year{ 1970 };
    dt_month m_month{ jan };
    time_type m_day{ 1 };
    time_type m_hour{ 0 };
    time_type m_min{ 0 };
    time_type m_sec{ 0 };
    time_type m_nsec{ 0 };
};

namespace ratio
{

static constexpr time_type micro_ratio_ns{ 1000 };
static constexpr time_type ns_ratio_sec{ 1000000000 };
static constexpr time_type micro_ratio_sec{ 1000000 };
static constexpr time_type milli_ratio_sec{ 1000 };
static constexpr time_type sec_ratio_sec{ 1 };
static constexpr time_type min_ratio_sec{ 60 };
static constexpr time_type hour_ratio_sec{ 3600 };
static constexpr time_type day_ratio_sec{ 86400 };

}// ratio

namespace details
{

template< typename T > struct is_duration : std::false_type {};

template< typename rep, typename period >
struct is_duration< std::chrono::duration< rep, period > > : std::true_type {};

template< typename T >
using enable_if_std_duration = typename std::enable_if< is_duration< T >::value >::type;

class time_info_base
{
	struct access {};
	template< time_type, time_type > friend class time_info;

public:
	explicit time_info_base(const access&) {}
};

template< typename T >
using enable_if_time_info = typename std::enable_if< std::is_base_of< time_info_base, T >::value >::type;

}// details

}// time_info

#endif
