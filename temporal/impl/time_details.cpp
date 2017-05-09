#include "time_details.h"

namespace temporal
{

namespace details
{

time_moment::time_moment( time_moment&& other ) noexcept :
                          m_julian_sec( other.m_julian_sec ),
                          m_nsec( other.m_nsec )
{
    other.m_julian_sec = other.m_nsec = 0;
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

void time_moment::set_jsec( time_type sec ) noexcept
{
    m_julian_sec = sec;
}

void time_moment::set_sec_epoch( time_type sec ) noexcept
{
    m_julian_sec = sec + details::julian_sec_before_epoch;
}

void time_moment::set_nsec( time_type nsec ) noexcept
{
    m_nsec = nsec;
    if( m_nsec > ratio::ns_ratio_sec )
    {
        ++m_julian_sec;
        m_nsec -= ratio::ns_ratio_sec;
    }
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

}// details

}// temporal
