//#include "stdafx.h"

#include <iostream>
#include <string>
#include <stdexcept>
#include <chrono>
#include <cassert>
#include <map>
#include "time_info/temporal.h"

void time_moment_date_time_test()
{
    using namespace temporal;
    using namespace temporal::details;

    // time moment
    time_moment t1{ 1, 999999999 };
    time_moment t2{ 1, 999999999 };
    time_moment t3{ 2, 1 };
    time_moment t4{ 2, 2000000001 };
    time_moment t5{ details::julian_sec_before_epoch, 2000000001 };

    time_moment t1a = t1;

    assert(t1 == time_moment{ t1 } && t1 == t1a);
    assert(t1 == t2);
    assert(t1 != t3);
    assert(t1 < t3);
    assert(t3 > t1);
    assert(t1 <= t1 && t1 <= t2 && t1 <= t3);
    assert(t3 >= t1 && t3 >= t2 && t3 >= t1);
    assert((t1 + t2 == time_moment{ 3, 999999998 }));
    assert((t1 - t2 == time_moment{ 0, 0 }));
    assert((t3 - t2 == time_moment{ 0, 2 }));
    assert( ( t4 == time_moment{ 4, 1 } ) );
    assert( t4.jsec() == 4 && t4.sec_epoch() == 0 && t4.nsec() == 1 && t4.nsec_epoch() == 0 );
    assert( t5.jsec() == details::julian_sec_before_epoch + 2 &&
            t5.sec_epoch() == 2 &&
            t5.nsec() == 1 && t5.nsec_epoch() == 1 );

    t1.set_jsec( 1 );
    assert( t1.jsec() == 1 );
    t1.set_sec_epoch( 1 );
    assert( t1.jsec() == details::julian_sec_before_epoch + 1 );
    t1.set_nsec( 1 );
    assert( t1.nsec() == t1.nsec_epoch() == 1 );

    // date time
    date_time d1{ 2015, date_time::apr, 24, 13, 13, 13, 13 };
    date_time d2{ 2015, date_time::apr, 24, 13, 13, 13, 13 };
    date_time d3{ 2016, date_time::apr, 25, 13, 13, 13, 13 };
    date_time d4{ 2016, date_time::jan, 26, 13, 13, 13, 13 };
    date_time d5{ 2016, date_time::dec, 27, 13, 13, 13, 13 };

    assert( d1.to_string() == "Fri Apr 24 13:13:13:0 2015");

    assert(d1 == d2);
    assert(d1 < d3 && d1 < d4 && d1 < d5);
    assert(d1 <= d1 && d1 <= d2 && d1 <= d3 && d1 <= d4 && d1 <= d5);
    assert(d5 > d1 && d5 > d2 && d5 > d3 && d5 > d4);
    assert(d5 >= d1 && d5 >= d2 && d5 >= d3 && d5 >= d4 && d5 >= d5);

    assert( d1.day_of_week() == 5 );
    assert( d5.is_leap() );
    assert( !d1.is_leap() );

    date_time dayt{ 2017, date_time::apr, 27, 15, 38, 22, 629000000 };
    assert( dayt.year() == 2017 );
    assert( dayt.month() == date_time::apr );
    assert( dayt.day() == 27 );
    assert( dayt.hour() == 15 );
    assert( dayt.minute() == 38 );
    assert( dayt.sec() == 22 );
    assert( dayt.nsec() == 629000000 );
}

void time_ratio_test()
{
    using namespace temporal;
    using namespace temporal::details;

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

        assert( ns0.count() == 0 );
        assert(d == d1 && d==d11 && h == h1 && m == m1 && s == s1 && ms == ms1 && mic == mic1 && ns == ns1);
        assert(d.count() == 2 && d1.count() == 2);
        assert(h.count() == 48 && h1.count() == 48);
        assert(m.count() == 2880 && m1.count() == 2880);
        assert(s.count() == 172800 && s1.count() == 172800);
        assert(ms.count() == 172800000 && ms1.count() == 172800000);
        assert(mic.count() == 172800000000 && mic1.count() == 172800000000);
        assert(ns.count() == 172800000000000 && ns1.count() == 172800000000000);
    }

    { // from -> to
        time_t tt{ 172800 };
        millisec_t ms;
        ms.from_time_t(tt);
        assert(tt == ms.to_time_t());

        timeval tv{ 172800, 999 };
        hours_t h{ tv };
        hours_t h1;
        h1.from_timeval( tv );
        assert(h.to_timeval().tv_sec == tv.tv_sec && h.to_timeval().tv_usec == tv.tv_usec);
        assert(h1.to_timeval().tv_sec == tv.tv_sec && h1.to_timeval().tv_usec == tv.tv_usec);

        timespec ts{ 172800, 999 };
        minutes_t m{ ts };
        minutes_t m1;
        m1.from_timespec(ts);
        assert(m.to_timespec().tv_sec == ts.tv_sec && m.to_timespec().tv_nsec == ts.tv_nsec);
        assert(m1.to_timespec().tv_sec == ts.tv_sec && m1.to_timespec().tv_nsec == ts.tv_nsec);

        std::chrono::nanoseconds mns{ 999999999 };
        nanosec_t ns1{ mns, since::julian };
        nanosec_t ns2;
        ns2.from_std_duration( mns, since::julian );
        assert( mns == ns1.to_std_duration() &&
                mns == ns2.to_std_duration());
    }

    {
        time_moment tm{ 172800, 10 };
        days_t d{ tm };
        assert(d.count() == 2);
    }

    {
        date_time dt_right{ 2017, date_time::apr, 27, 15, 38, 22, 629000000 };
        millisec_t ms{ dt_right };

        ms += days_t{ 365 };
        auto dayt = ms.to_date_time();

        assert( dayt.year() == 2018 );
        assert( dayt.month() == date_time::apr );
        assert( dayt.day() == 27 );
        assert( dayt.hour() == 15 );
        assert( dayt.minute() == 38 );
        assert( dayt.sec() == 22 );
        assert( dayt.nsec() == 629000000 );
    }

    {
        using namespace std::chrono;
        nanosec_t nnns{ nanosec_t::now() };
        time_type ss1 = nnns.count_since_epoch();
        time_type ss2 = duration_cast< nanoseconds >( system_clock::now().time_since_epoch() ).count();
        assert( ( ss2 - ss1) < ratio::ns_ratio_sec );

        seconds_t s = seconds_t::now();
        assert( s.count() == details::julian_sec_before_epoch +
                duration_cast< seconds >( system_clock::now().time_since_epoch() ).count() );
    }

    {
        date_time dt_right{ 2017, date_time::apr, 27, 15, 38, 22, 629000001 };
        millisec_t ms{ dt_right };

        assert(ms.to_date_time() == dt_right );
        assert(ms.count() == 1493307502629 + details::julian_sec_before_epoch * 1000 );
        nanosec_t ns = ms - millisec_t{ 1493307502629 + details::julian_sec_before_epoch * 1000 };
        assert( ns.count() == 1 && ms.to_date_time().nsec() == 629000001 );
    }

    auto t = nanosec_t::max();
}

int main()
{
    time_ratio_test();
    time_moment_date_time_test();

    return 0;
}
