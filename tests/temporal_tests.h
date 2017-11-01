#ifndef _HELPERS_TEMPORAL_TESTS_H_
#define _HELPERS_TEMPORAL_TESTS_H_

#include <iostream>
#include <functional>
#include <iostream>
#include <string>
#include <stdexcept>
#include <chrono>
#include <cassert>
#include <map>

#include "test.h"
#include "temporal.h"

using namespace helpers::temporal;
using namespace helpers::temporal::details;

namespace temporal_tests
{

TEST_CASE( time_moment_date_time_test )
{
    // time moment
    time_moment t1{ 1, 999999999 };
    time_moment t2{ 1, 999999999 };
    time_moment t3{ 2, 1 };
    time_moment t4{ 2, 2000000001 };
    time_moment t5{ julian_sec_before_epoch, 2000000001 };

    time_moment t1a = t1;

    DYNAMIC_ASSERT(t1 == time_moment{ t1 } && t1 == t1);
    DYNAMIC_ASSERT(t1 == t2);
    DYNAMIC_ASSERT(t1 != t3);
    DYNAMIC_ASSERT(t1 < t3);
    DYNAMIC_ASSERT(t3 > t1);
    DYNAMIC_ASSERT(t1 <= t1 && t1 <= t2 && t1 <= t3);
    DYNAMIC_ASSERT(t3 >= t1 && t3 >= t2 && t3 >= t1);
    DYNAMIC_ASSERT((t1 + t2 == time_moment{ 3, 999999998 }));
    DYNAMIC_ASSERT((t1 - t2 == time_moment{ 0, 0 }));
    DYNAMIC_ASSERT((t3 - t2 == time_moment{ 0, 2 }));
    DYNAMIC_ASSERT( ( t4 == time_moment{ 4, 1 } ) );
    DYNAMIC_ASSERT( t4.jsec() == 4 && t4.sec_epoch() == 0 && t4.nsec() == 1 && t4.nsec_epoch() == 0 );
    DYNAMIC_ASSERT( t5.jsec() == julian_sec_before_epoch + 2 &&
            t5.sec_epoch() == 2 &&
            t5.nsec() == 1 && t5.nsec_epoch() == 1 );

    t1.set_jsec( 1 );
    DYNAMIC_ASSERT( t1.jsec() == 1 );
    t1.set_sec_epoch( 1 );
    DYNAMIC_ASSERT( t1.jsec() == julian_sec_before_epoch + 1 );
    t1.set_nsec( 1 );
    DYNAMIC_ASSERT( t1.nsec() == t1.nsec_epoch() == 1 );

    // date time
    date_time d1{ 2015, date_time::apr, 24, 13, 13, 13, 13 };
    date_time d2{ 2015, date_time::apr, 24, 13, 13, 13, 13 };
    date_time d3{ 2016, date_time::apr, 25, 13, 13, 13, 13 };
    date_time d4{ 2016, date_time::jan, 26, 13, 13, 13, 13 };
    date_time d5{ 2016, date_time::dec, 27, 13, 13, 13, 13 };

    DYNAMIC_ASSERT( d1.to_string() == "Fri Apr 24 13:13:13:0 2015");

    DYNAMIC_ASSERT(d1 == d2);
    DYNAMIC_ASSERT(d1 < d3 && d1 < d4 && d1 < d5);
    DYNAMIC_ASSERT(d1 <= d1 && d1 <= d2 && d1 <= d3 && d1 <= d4 && d1 <= d5);
    DYNAMIC_ASSERT(d5 > d1 && d5 > d2 && d5 > d3 && d5 > d4);
    DYNAMIC_ASSERT(d5 >= d1 && d5 >= d2 && d5 >= d3 && d5 >= d4 && d5 >= d5);

    DYNAMIC_ASSERT( d1.day_of_week() == 5 );
    DYNAMIC_ASSERT( d5.is_leap() );
    DYNAMIC_ASSERT( !d1.is_leap() );

    date_time dayt{ 2017, date_time::apr, 27, 15, 38, 22, 629000000 };
    DYNAMIC_ASSERT( dayt.year() == 2017 );
    DYNAMIC_ASSERT( dayt.month() == date_time::apr );
    DYNAMIC_ASSERT( dayt.day() == 27 );
    DYNAMIC_ASSERT( dayt.hour() == 15 );
    DYNAMIC_ASSERT( dayt.minute() == 38 );
    DYNAMIC_ASSERT( dayt.sec() == 22 );
    DYNAMIC_ASSERT( dayt.nsec() == 629000000 );
}

TEST_CASE( time_ratio_test )
{
    { // ctors assignment and general values
        nanosec_t ns0;
        days_t d{ 2 };
        hours_t h{ d };
        minutes_t m{ d };
        seconds_t s{ d };
        millisec_t ms{ d };
        microsec_t mic{ d };
        nanosec_t ns{ d };

        days_t d1{ d };
        days_t d11 = d;
        hours_t h1{ d };
        minutes_t m1{ d };
        seconds_t s1{ d };
        millisec_t ms1{ d };
        microsec_t mic1{ d };
        nanosec_t ns1{ d };

        d1 -= h1;
        d1 += h1;

        DYNAMIC_ASSERT( ns0.count() == 0 );
        DYNAMIC_ASSERT(d == d1 && d==d11 && h == h1 && m == m1 && s == s1 && ms == ms1 && mic == mic1 && ns == ns1);
        DYNAMIC_ASSERT(d.count() == 2 && d1.count() == 2);
        DYNAMIC_ASSERT(h.count() == 48 && h1.count() == 48);
        DYNAMIC_ASSERT(m.count() == 2880 && m1.count() == 2880);
        DYNAMIC_ASSERT(s.count() == 172800 && s1.count() == 172800);
        DYNAMIC_ASSERT(ms.count() == 172800000 && ms1.count() == 172800000);
        DYNAMIC_ASSERT(mic.count() == 172800000000 && mic1.count() == 172800000000);
        DYNAMIC_ASSERT(ns.count() == 172800000000000 && ns1.count() == 172800000000000);
    }

    { // from -> to
        time_t tt{ 172800 };
        millisec_t ms;
        ms.from_time_t(tt);
        DYNAMIC_ASSERT(tt == ms.to_time_t());

        timeval tv{ 172800, 999 };
        hours_t h{ tv };
        hours_t h1;
        h1.from_timeval( tv );
        DYNAMIC_ASSERT(h.to_timeval().tv_sec == tv.tv_sec && h.to_timeval().tv_usec == tv.tv_usec);
        DYNAMIC_ASSERT(h1.to_timeval().tv_sec == tv.tv_sec && h1.to_timeval().tv_usec == tv.tv_usec);

        timespec ts{ 172800, 999 };
        minutes_t m{ ts };
        minutes_t m1;
        m1.from_timespec(ts);
        DYNAMIC_ASSERT(m.to_timespec().tv_sec == ts.tv_sec && m.to_timespec().tv_nsec == ts.tv_nsec);
        DYNAMIC_ASSERT(m1.to_timespec().tv_sec == ts.tv_sec && m1.to_timespec().tv_nsec == ts.tv_nsec);

        std::chrono::nanoseconds mns{ 999999999 };
        nanosec_t ns1{ mns, since::julian };
        nanosec_t ns2;
        ns2.from_std_duration( mns, since::julian );
        DYNAMIC_ASSERT( mns == ns1.to_std_duration() &&
                mns == ns2.to_std_duration());
    }

    {
        time_moment tm{ 172800, 10 };
        days_t d{ tm };
        DYNAMIC_ASSERT(d.count() == 2);
    }

    {
        date_time dt_right{ 2017, date_time::apr, 27, 15, 38, 22, 629000000 };
        millisec_t ms{ dt_right };

        ms += days_t{ 365 };
        auto dayt = ms.to_date_time();

        DYNAMIC_ASSERT( dayt.year() == 2018 );
        DYNAMIC_ASSERT( dayt.month() == date_time::apr );
        DYNAMIC_ASSERT( dayt.day() == 27 );
        DYNAMIC_ASSERT( dayt.hour() == 15 );
        DYNAMIC_ASSERT( dayt.minute() == 38 );
        DYNAMIC_ASSERT( dayt.sec() == 22 );
        DYNAMIC_ASSERT( dayt.nsec() == 629000000 );

        verbose_date_time vdt{ dayt.to_verbose_date_time() };
        DYNAMIC_ASSERT( vdt.year == 2018 );
        DYNAMIC_ASSERT( vdt.month == date_time::apr );
        DYNAMIC_ASSERT( vdt.day == 27 );
        DYNAMIC_ASSERT( vdt.hour == 15 );
        DYNAMIC_ASSERT( vdt.min == 38 );
        DYNAMIC_ASSERT( vdt.sec == 22 );
        DYNAMIC_ASSERT( vdt.nsec == 629000000 );
    }

    {
        using namespace std::chrono;
        nanosec_t nnns{ nanosec_t::now() };
        time_type ss1 = nnns.count_since_epoch();
        time_type ss2 = duration_cast< nanoseconds >( system_clock::now().time_since_epoch() ).count();
        DYNAMIC_ASSERT( ( ss2 - ss1) < ratio::ns_ratio_sec );

        seconds_t s = seconds_t::now();
        DYNAMIC_ASSERT( s.count() == julian_sec_before_epoch +
                duration_cast< seconds >( system_clock::now().time_since_epoch() ).count() );
    }

    {
        date_time dt_right{ 2017, date_time::apr, 27, 15, 38, 22, 629000001 };
        millisec_t ms{ dt_right };

        DYNAMIC_ASSERT(ms.to_date_time() == dt_right );
        DYNAMIC_ASSERT(ms.count() == 1493307502629 + julian_sec_before_epoch * 1000 );
        nanosec_t ns = ms - millisec_t{ 1493307502629 + julian_sec_before_epoch * 1000 };
        DYNAMIC_ASSERT( ns.count() == 1 && ms.to_date_time().nsec() == 629000001 );
    }

    auto t = nanosec_t::max();
}

}

#endif
