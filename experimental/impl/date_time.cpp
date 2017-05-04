﻿#include <map>
#include <array>
#include <stdexcept>

#include "date_time.h"

namespace temporal
{

namespace details
{

static constexpr time_type epoch_year{ 1970 };
static constexpr time_type ns_before_sec{ 999999999 };
static constexpr std::array< uint32_t, 12 > month_days{ 31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31 };

constexpr bool is_leap( time_type year ) noexcept
{
    return ( !( year % 4 ) && ( ( year % 100 ) || !( year % 400 ) ) );
}

constexpr uint32_t days_in_month( time_type month, time_type year ) noexcept
{
    return( month == date_time::feb && is_leap( year ) )?
        month_days[ month - 1 ] + 1 : month_days[ month - 1 ];
}

// validations
void validate_month( time_t month )
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
    if( nsec > ns_before_sec )
    {
        throw std::invalid_argument{ "Invalid nanosecond" };
    }
}

// date calculations
constexpr int64_t floor_div( int64_t a, int b ) noexcept
{
    return ( a - ( a < 0 ? b - 1 : 0 ) ) / b;
}

constexpr int floor_div( int a, int b ) noexcept
{
    return ( a - ( a < 0 ? b - 1 : 0 ) ) / b;
}

struct verbose_date_time
{
    int64_t day{ 0 };
    int64_t month{ 0 };
    int64_t year{ 0 };
    int64_t hour{ 0 };
    int64_t min{ 0 };
    int64_t sec{ 0 };
    int64_t nsec{ 0 };
};

time_type vdate_to_julian_sec( int64_t year, date_time::dt_month month, time_type day,
                               time_type hour, time_type min, time_type sec ) noexcept
{
    validate_month( month );
    validate_day( day, month, year );
    validate_hour( hour );
    validate_min( min );
    validate_sec( sec );

    int64_t year_fix{ year < 0 ? year + 1 : year };

    int64_t a{ floor_div( 14 - month, 12 ) };
    int64_t y{ year_fix + 4800 - a };
    int64_t m{ month + 12 * a - 3 };

    time_type jday =  day +
           floor_div( 153 * m + 2, 5 ) +
           365 * y +
           floor_div( y, 4 ) -
           floor_div( y, 100 ) +
           floor_div( y, 400 ) -
           32045;

    return jday * ratio::day_ratio_sec + hour * ratio::hour_ratio_sec + min * ratio::min_ratio_sec + sec;
}

verbose_date_time julian_sec_to_vdate( time_type julian_sec ) noexcept
{
    int64_t day = julian_sec / ratio::day_ratio_sec;
    int64_t a{ day + 32044 };
    int64_t b = floor_div( 4 * a + 3, 146097 );
    int64_t c = a - floor_div( 146097 * b, 4 );

    int64_t d = floor_div( 4 * c + 3, 1461 );
    int64_t e = c - floor_div( 1461 * d, 4 );
    int64_t m = floor_div( 5 * e + 2, 153 );

    verbose_date_time vdt;
    vdt.day = e - floor_div( 153 * m + 2, 5 ) + 1;
    vdt.month = m + 3 - 12 * floor_div( m, 10 );
    vdt.year = 100 * b + d - 4800 + floor_div( m, 10 );

    if( vdt.year <= 0 )
    {
        --vdt.year;
    }

    time_type sec_left = julian_sec - day * ratio::day_ratio_sec;
    vdt.hour = sec_left / ratio::hour_ratio_sec;
    sec_left -= vdt.hour * ratio::hour_ratio_sec;
    vdt.min = sec_left / ratio::min_ratio_sec;
    sec_left -= vdt.min * ratio::min_ratio_sec;
    vdt.sec = sec_left;

    return vdt;
}

namespace serialize
{

 enum pattern_type
 {
    p_year, p_month, p_day, p_day_of_week, p_hour, p_min, p_sec, p_msec, p_usec, p_nsec
 };

 static std::map< pattern_type, std::string > pattern_map
 {
     { p_year, "Y" },
     { p_month, "M" },
     { p_day, "D" },
     { p_day_of_week, "W" },
     { p_hour, "h" },
     { p_min, "m" },
     { p_sec, "s" },
     { p_msec, "ms" },
     { p_usec, "us" },
     { p_nsec, "ns" }
 };

 static std::map< date_time::dt_month, std::string > month_names
 {
     { date_time::jan, "Jan" },
     { date_time::feb, "Feb" },
     { date_time::mar, "Mar" },
     { date_time::apr, "Apr" },
     { date_time::may, "May" },
     { date_time::jun, "Jun" },
     { date_time::jul, "Jul" },
     { date_time::aug, "Aug" },
     { date_time::sep, "Sep" },
     { date_time::oct, "Oct" },
     { date_time::nov, "Nov" },
     { date_time::dec, "Dec" }
 };

 static std::map< uint, std::string > day_names
 {
     { 1, "Mon" },{ 2, "Tue" },{ 3, "Wed" },{ 4, "Thu" },{ 5, "Fri" },{ 6, "Sat" },{ 7, "Sun" }
 };


 void replace_if_exists( pattern_type p, std::string& target, const std::string& source )
 {
     const std::string& pattern = pattern_map[ p ];

     auto pos = target.find( pattern );
     if( pos != std::string::npos )
     {
         target.replace( pos, pattern.length(), source );
     }
 }

static std::string serialize( verbose_date_time& dtv, int dow, const std::string& pattern ) noexcept
{
    int64_t msec{ dtv.nsec / 1000000 };
    dtv.nsec -= msec * 1000000;
    int64_t usec{ dtv.nsec / 1000 };
    dtv.nsec -= usec * 1000;

    std::string result{ pattern };
    replace_if_exists( p_year, result, std::to_string( dtv.year ) );
    replace_if_exists( p_month, result, month_names[ ( date_time::dt_month )dtv.month ] );
    replace_if_exists( p_day, result, std::to_string( dtv.day ) );
    replace_if_exists( p_day_of_week, result, day_names[ dow ] );
    replace_if_exists( p_hour, result, std::to_string( dtv.hour ) );
    replace_if_exists( p_min, result, std::to_string( dtv.min ) );
    replace_if_exists( p_sec, result, std::to_string( dtv.sec ) );
    replace_if_exists( p_msec, result, std::to_string( msec ) );
    replace_if_exists( p_usec, result, std::to_string( usec ) );
    replace_if_exists( p_nsec, result, std::to_string( dtv.nsec ) );

    return result;
}

}

}// details

// date_time
date_time::date_time( int64_t year, dt_month month, time_type day,
                      time_type hour, time_type min, time_type sec, time_type nsec ) :
    m_time( details::vdate_to_julian_sec( year, month, day, hour, min, sec ), nsec ){}

bool date_time::is_leap() const noexcept
{
    return details::is_leap( details::julian_sec_to_vdate( m_time.jsec() ).year );
}

int date_time::day_of_week() const noexcept
{
    time_type jday = m_time.jsec() / ratio::day_ratio_sec;
    return jday >=0 ? ( jday % 7 ) + 1 : ( ( jday + 1 ) % 7 ) + 7;
}

//year:month:day:hour:min:sec:msec:usec:nsec
std::string date_time::to_string( const std::string& pattern ) const noexcept
{
    static std::string default_pattern{ "W M D h:m:s:ms Y" };

    auto dtv = details::julian_sec_to_vdate( m_time.jsec() );
    dtv.nsec = nsec();

    return details::serialize::serialize( dtv, day_of_week(),
                                          pattern.empty()? default_pattern : pattern );
}

int64_t date_time::year() const noexcept
{
    return details::julian_sec_to_vdate( m_time.jsec() ).year;
}

auto date_time::month() const noexcept -> dt_month
{
    return static_cast< dt_month >( details::julian_sec_to_vdate( m_time.jsec() ).month );
}

time_type date_time::day() const noexcept
{
    return details::julian_sec_to_vdate( m_time.jsec() ).day;
}

time_type date_time::hour() const noexcept
{
    return details::julian_sec_to_vdate( m_time.jsec() ).hour;
}

time_type date_time::minute() const noexcept
{
    return details::julian_sec_to_vdate( m_time.jsec() ).min;
}

time_type date_time::sec() const noexcept
{
    return details::julian_sec_to_vdate( m_time.jsec() ).sec;
}

time_type date_time::nsec() const noexcept
{
    return m_time.nsec();
}

}// temporal