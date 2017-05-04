#include <array>
#include <stdexcept>

#include "time_details.h"

namespace time_info
{

namespace details
{

// time moment
time_moment::time_moment( time_type julian_sec, time_type nsec ) :
                          m_julian_sec( julian_sec ),
                          m_nsec( nsec )
{    
    if( nsec > ns_before_sec )
    {
        throw std::invalid_argument{ "Invalid nanosecond" };
    }
}

time_moment::time_moment( time_moment&& other ) noexcept  : 
                          m_julian_sec( other.m_julian_sec ),
                          m_nsec( other.m_nsec )
{
    other.m_julian_sec = other.m_nsec = 0;
}

void time_moment::set_jsec( time_type sec ) noexcept
{
    m_julian_sec = sec;
}

void time_moment::set_sec_epoch( time_type sec ) noexcept
{
    m_julian_sec = sec + details::julian_sec_before_epoch;
}

void time_moment::set_nsec( time_type nsec )
{
    if( nsec > ns_before_sec )
    {
        throw std::invalid_argument{ "Invalid nanosecond" };
    }

    m_nsec = nsec;
}

time_type time_moment::sec_epoch() const noexcept
{
    return m_julian_sec - details::julian_sec_before_epoch;
}

time_moment& time_moment::operator=( const time_moment& other ) noexcept
{
    m_julian_sec = other.m_julian_sec;
    m_nsec = other.m_nsec;
    return *this;
}

time_moment& time_moment::operator=( time_moment&& other ) noexcept
{
    m_julian_sec = other.m_julian_sec;
    m_nsec = other.m_nsec;
    other.m_julian_sec = other.m_nsec = 0;
    return *this;
}

time_moment& time_moment::operator+=( const time_moment& other ) noexcept
{
    m_julian_sec += other.m_julian_sec;
    m_nsec += other.m_nsec;

    if( m_nsec > ratio::ns_ratio_sec )
    {
        ++m_julian_sec;
        m_nsec -= ratio::ns_ratio_sec;
    }

    return *this;
}

time_moment& time_moment::operator-=( const time_moment& other ) noexcept
{
    m_julian_sec -= other.m_julian_sec;

    if( m_nsec < other.m_nsec )
    {
        --m_julian_sec;
        m_nsec = ratio::ns_ratio_sec + m_nsec - other.m_nsec;
    }
    else
    {
        m_nsec -= other.m_nsec;
    }
  
    return *this;
}

bool time_moment::operator<( const time_moment& other ) const noexcept
{
    return m_julian_sec < other.m_julian_sec ?
        true : ( m_julian_sec == other.m_julian_sec ? m_nsec < other.m_nsec : false );
}

bool time_moment::operator>( const time_moment& other ) const noexcept
{
    return  ( !( *this < other ) && !( *this == other ) );
}

bool time_moment::operator<=( const time_moment& other ) const noexcept
{
    return  ( *this < other || *this == other );
}

bool time_moment::operator>=( const time_moment& other ) const noexcept
{
    return  ( *this > other || *this == other );
}

bool time_moment::operator==( const time_moment& other ) const noexcept
{
    return ( m_julian_sec == other.m_julian_sec ) && ( m_nsec == other.m_nsec );
}

bool time_moment::operator!=( const time_moment& other ) const noexcept
{
    return !( *this == other );
}

time_moment operator+( const time_moment& l, const time_moment& r ) noexcept
{
    time_moment tm{ l };
    return tm.operator+=( r );
}

time_moment operator-( const time_moment& l, const time_moment& r ) noexcept
{
	time_moment tm{ l };
	return tm.operator-=( r );
}

}// details

}// time_info
