#include <stdexcept>

#include "time_details.h"

namespace time_info
{

namespace details
{

static constexpr time_type epoch_year{ 1970 };
static constexpr time_type max_ns{ 999999999 };

constexpr time_type leaps_count( time_type year ) noexcept
{
    return year / 4 - year / 100 + year / 400;
}

static constexpr time_type leaps_before_epoch{ leaps_count( epoch_year ) };

constexpr bool is_leap(time_type year ) noexcept
{
    return ( !( year % 4 ) && ( ( year % 100 ) || !( year % 400 ) ) );
}

constexpr time_type leaps_since_epoch( time_type year ) noexcept
{
    return leaps_count( year ) - leaps_before_epoch;
}

constexpr time_type year_to_days_since_epoch( time_type year ) noexcept
{
    return ( 365 * ( year - ( epoch_year - 1 ) - leaps_since_epoch( year ) ) +
             366 * leaps_since_epoch( year ) );
}

time_type days_in_month( time_type month, time_type year )
{
    time_type days{ 0 };

    switch( month )
    {
    case date_time::jan: days = 31; break;
    case date_time::feb: days = is_leap( year )? 29 : 28; break;
    case date_time::mar: days = 31; break;
    case date_time::apr: days = 30; break;
    case date_time::may: days = 31; break;
    case date_time::jun: days = 30; break;
    case date_time::jul: days = 31; break;
    case date_time::aug: days = 31; break;
    case date_time::sep: days = 30; break;
    case date_time::oct: days = 31; break;
    case date_time::nov: days = 30; break;
    case date_time::dec: days = 31; break;
    default: throw std::invalid_argument{ "Invaid month" };
    }

    return days;
}

void validate_year( time_type year )
{
    if( year < epoch_year )
    {
        throw std::invalid_argument{ "Invalid year" };
    }
}

void validate_month( date_time::dt_month month )
{
    if( ( month < date_time::jan || month > date_time::dec ) )
    {
        throw std::invalid_argument{ "Invalid month" };
    }
}

void validate_day( time_type day, time_type month, time_type year )
{
    if( ( !day || day > days_in_month( month, year ) ) )
    {
        throw std::invalid_argument{ "Invalid day" };
    }
}

void validate_hour( time_type hour )
{
    if( hour > 23 )
    {
        throw std::invalid_argument{ "Invalid hour" };
    }
}

void validate_min( time_type min )
{
    if( min > 59 )
    {
        throw std::invalid_argument{ "Invalid minute" };
    }
}

void validate_sec( time_type sec )
{
    if( sec > 59 )
    {
        throw std::invalid_argument{ "Invalid second" };
    }
}

void validate_nsec( time_type nsec )
{
    if( nsec > max_ns )
    {
        throw std::invalid_argument{ "Invalid nanosecond" };
    }
}

void validate_date_time( const date_time& dt )
{
    validate_year( dt.year() );
    validate_month( dt.month() );
    validate_day( dt.day(), dt.month(), dt.year() );
    validate_hour( dt.hour() );
    validate_min( dt.min() );
    validate_sec( dt.sec() );
    validate_nsec( dt.nsec() );
}

}// details

time_moment::time_moment( time_type nsec ) noexcept : 
    m_sec( nsec / details::max_ns ), m_nsec( nsec % details::max_ns ){}

time_moment::time_moment( time_type sec, time_type nsec ) noexcept :
    m_sec( sec ), m_nsec( nsec )
{
    details::validate_nsec( nsec );

}

date_time time_moment::to_date_time() const noexcept
{
    using namespace details;
    date_time dt;

    time_type days{ m_sec / ratio::day_ratio_sec };
    time_type remain_sec{ m_sec - days * ratio::day_ratio_sec };

    dt.set_year( days / 365 + epoch_year );
    days -= ( ( dt.year() - epoch_year ) * 365 +
              details::leaps_since_epoch( dt.year() - 1 ) );

    for( int month{ date_time::jan }; month < date_time::dec; ++month )
    {
        time_type days_in_month{ details::days_in_month( month, dt.year() ) };
        if( days_in_month > days )
        {
            dt.set_month( static_cast< date_time::dt_month >( month ) );
            break;
        }

        days -= days_in_month;
    }

    dt.set_day( days + 1 );
    dt.set_hour( remain_sec / ratio::hour_ratio_sec);
    remain_sec -= dt.hour() * ratio::hour_ratio_sec;
    dt.set_min( remain_sec / ratio::min_ratio_sec);
    remain_sec -= dt.min() * ratio::min_ratio_sec;
    dt.set_sec( remain_sec );
    dt.set_nsec( m_nsec );

    return dt;
}

time_moment& time_moment::operator+=( const time_moment& other ) noexcept
{
    m_sec += other.m_sec;
    m_nsec += other.m_nsec;

    if( m_nsec > details::max_ns )
    {
        ++m_sec;
        m_nsec -= ( details::max_ns + 1 );
    }

    return *this;
}

time_moment& time_moment::operator-=( const time_moment& other ) noexcept
{
    m_sec -= other.m_sec;
    if( other.m_nsec > m_nsec )
    {
        m_nsec = details::max_ns + 1 + m_nsec - other.m_nsec;
        --m_sec;
    }
    else
    {
        m_nsec -= other.m_nsec;
    }

    return *this;
}

void time_moment::set_sec( time_type sec ) noexcept
{
    m_sec = sec;
}

void time_moment::set_nsec( time_type nsec )
{
	details::validate_nsec( nsec );
    m_nsec = nsec;
}

void time_moment::set( time_type sec, time_type nsec )
{
    set_sec( sec );
    set_nsec( nsec );
}

bool time_moment::operator<( const time_moment& other ) const noexcept
{
    return m_sec < other.m_sec? true : ( m_nsec == other.m_nsec ? m_nsec < other.m_nsec : false );
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
    return ( m_sec == other.m_sec ) && ( m_nsec == other.m_nsec );
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

date_time::date_time( time_type year, dt_month month, time_type day, time_type hour,
                      time_type min, time_type sec, time_type nsec ) :
    m_year( year ), m_month( month ), m_day( day ), m_hour( hour ),
    m_min( min ), m_sec( sec ), m_nsec( nsec )
{
    details::validate_date_time( *this );
}

time_moment date_time::to_time_moment() const noexcept
{
    time_type days{ details::year_to_days_since_epoch( m_year - 1 ) + m_day - 1 };
    for( time_type curr_month{ 1 }; curr_month < m_month; ++curr_month )
    {
        days += details::days_in_month( curr_month, m_year );
    }

    return time_moment{ days * ratio::day_ratio_sec +
                        m_hour * ratio::hour_ratio_sec +
                        m_min * ratio::min_ratio_sec +
                        m_sec,
                        m_nsec };
}

bool date_time::operator<( const date_time& other ) const noexcept
{
    return to_time_moment() < other.to_time_moment();
}

bool date_time::operator>( const date_time& other ) const noexcept
{
    return  ( !( *this < other ) && !( *this == other ) );
}

bool date_time::operator<=( const date_time& other ) const noexcept
{
    return  ( *this < other || *this == other );
}

bool date_time::operator>=( const date_time& other ) const noexcept
{
    return  ( *this > other || *this == other );
}

bool date_time::operator==( const date_time& other ) const noexcept
{
    return m_year == other.m_year && m_month == other.m_month && m_day == other.m_day &&
            m_min == other.m_min && m_sec == other.m_sec && m_nsec == other.m_nsec;
}

bool date_time::operator!=( const date_time& other ) const noexcept
{
    return !( *this == other );
}

void date_time::set_year(time_type year)
{
    details::validate_year( year );
    m_year = year;
}

void date_time::set_month( dt_month month )
{
    details::validate_month( month );
    m_month = month;
}

void date_time::set_day( time_type day )
{
    details::validate_day( day, m_month, m_year );
    m_day = day;
}

void date_time::set_hour( time_type hour )
{
    details::validate_hour( hour );
    m_hour = hour;
}

void date_time::set_min( time_type min )
{
    details::validate_min( min );
    m_min = min;
}

void date_time::set_sec( time_type sec )
{
    details::validate_sec( sec );
    m_sec = sec;
}

void date_time::set_nsec( time_type nsec )
{
    details::validate_nsec( nsec );
    m_nsec = nsec;
}

void date_time::set( time_type year, dt_month month, time_type day, time_type hour,
                     time_type min, time_type sec, time_type nsec )
{
    set_year( year );
    set_month( month );
    set_day( day );
    set_hour( hour );
    set_min( min );
    set_sec( sec );
    set_nsec( nsec );
}

}// time_info
