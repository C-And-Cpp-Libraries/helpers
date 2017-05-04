#ifndef _TIME_DETAILS_
#define _TIME_DETAILS_

#include <chrono>

namespace temporal
{

using time_type = uintmax_t;
class date_time;

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

static constexpr time_type julian_sec_before_epoch{ 2440588 * ratio::day_ratio_sec };

template< typename T > struct is_duration : std::false_type {};

template< typename rep, typename period >
struct is_duration< std::chrono::duration< rep, period > > : std::true_type {};

template< typename T >
using enable_if_std_duration = typename std::enable_if< is_duration< T >::value >::type;

}// details

namespace details
{

class time_moment final
{
public:
    constexpr time_moment() = default;
    constexpr time_moment( time_type julian_sec, time_type nsec ) :
        m_julian_sec( julian_sec + nsec / ratio::ns_ratio_sec ),
        m_nsec( nsec - ( nsec / ratio::ns_ratio_sec ) * ratio::ns_ratio_sec ){}

    time_moment( const time_moment& other ) = default;
    time_moment( time_moment&& other ) noexcept;
    time_moment& operator=( const time_moment& other ) noexcept;
    time_moment& operator=( time_moment&& other ) noexcept;

    void set_jsec( time_type sec ) noexcept;
    void set_sec_epoch( time_type sec ) noexcept;
    void set_nsec( time_type nsec ) noexcept;

    constexpr time_type jsec() const noexcept{ return m_julian_sec; }
    constexpr time_type nsec() const noexcept{ return m_nsec; }
    constexpr time_type sec_epoch() const noexcept
    {
        return m_julian_sec > details::julian_sec_before_epoch?
                    m_julian_sec - details::julian_sec_before_epoch : 0;
    }

    constexpr time_type nsec_epoch() const noexcept
    {
        return m_julian_sec >= details::julian_sec_before_epoch? m_nsec : 0;
    }

    constexpr bool operator<( const time_moment& other ) const noexcept
    {
        return m_julian_sec < other.m_julian_sec ?
            true : ( m_julian_sec == other.m_julian_sec ? m_nsec < other.m_nsec : false );
    }

    constexpr bool operator>( const time_moment& other ) const noexcept
    {
        return ( !( *this < other ) && !( *this == other ) );
    }

    constexpr bool operator<=(const time_moment& other ) const noexcept
    {
        return ( *this < other || *this == other );
    }

    constexpr bool operator>=(const time_moment& other ) const noexcept
    {
        return ( *this > other || *this == other );
    }

    constexpr bool operator==( const time_moment& other ) const noexcept
    {
        return ( m_julian_sec == other.m_julian_sec && m_nsec == other.m_nsec );
    }

    constexpr bool operator!=( const time_moment& other ) const noexcept
    {
        return !( *this == other );
    }

    time_moment& operator+=( const time_moment& other ) noexcept;
    time_moment& operator-=( const time_moment& other ) noexcept;

private:
    time_type m_julian_sec{ 0 };
    time_type m_nsec{ 0 };
};

constexpr time_moment operator+( const time_moment& l, const time_moment& r ) noexcept
{
    return{ l.jsec() + r.jsec() + ( ( l.nsec() + r.nsec() ) / ratio::ns_ratio_sec ),
            ( ( l.nsec() + r.nsec() ) % ratio::ns_ratio_sec ) };
}

constexpr time_moment operator-( const time_moment& l, const time_moment& r ) noexcept
{
    return{ l.jsec() - r.jsec() - ( ( l.nsec() < r.nsec() )? 1 : 0 ),
            ( ( l.nsec() < r.nsec() )? ratio::ns_ratio_sec + l.nsec() - r.nsec() : l.nsec() - r.nsec() ) };
}

}// details

}// temporal

#endif
