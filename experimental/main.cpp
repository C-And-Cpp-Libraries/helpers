#include "stdafx.h"

#include <iostream>
#include <string>
#include <stdexcept>
#include <chrono>
#include <cassert>
#include "time_info/time_ratio.h"

void time_moment_date_time_test()
{
	using namespace time_info;
    using namespace time_info::details;

	time_moment t1{ 1, 999999999 };
	time_moment t2{ 1, 999999999 };
	time_moment t3{ 2, 1 };

	assert(t1 == t2);
	assert(t1 != t3);
	assert(t1 < t3);
	assert(t3 > t1);
	assert(t1 <= t1 && t1 <= t2 && t1 <= t3);
	assert(t3 >= t1 && t3 >= t2 && t3 >= t1);
	assert((t1 + t2 == time_moment{ 3, 999999998 }));
	assert((t1 - t2 == time_moment{ 0, 0 }));
	assert((t3 - t2 == time_moment{ 0, 2 }));

	date_time d1{ 2015, date_time::apr, 24, 13, 13, 13, 13 };
	date_time d2{ 2015, date_time::apr, 24, 13, 13, 13, 13 };
	date_time d3{ 2016, date_time::apr, 25, 13, 13, 13, 13 };
	date_time d4{ 2016, date_time::jan, 26, 13, 13, 13, 13 };
	date_time d5{ 2016, date_time::dec, 27, 13, 13, 13, 13 };

	assert(d1 == d2);
	assert(d1 < d3 && d1 < d4 && d1 < d5);
	assert(d1 <= d1 && d1 <= d2 && d1 <= d3 && d1 <= d4 && d1 <= d5);
	assert(d5 > d1 && d5 > d2 && d5 > d3 && d5 > d4);
	assert(d5 >= d1 && d5 >= d2 && d5 >= d3 && d5 >= d4 && d5 >= d5);
}

void time_info_test()
{
	using namespace time_info;
    using namespace time_info::details;

	{
		days_t d{ 2 };
		hours_t h{ d };
		minutes_t m{ d };
		seconds_t s{ d };
		millisec_t ms{ d };
		microsec_t mic{ d };
		nanosec_t ns{ d };				

		days_t d1{ d };
		hours_t h1{ d };
		minutes_t m1{ d };
		seconds_t s1{ d };
		millisec_t ms1{ d };
		microsec_t mic1{ d };
		nanosec_t ns1{ d };

		d1 -= h1;
		d1 += h1;

		assert(d == d1 && h == h1 && m == m1 && s == s1 && ms == ms1 && mic == mic1 && ns == ns1);
		assert(d.count() == 2 && d1.count() == 2);
		assert(h.count() == 48 && h1.count() == 48);
		assert(m.count() == 2880 && m1.count() == 2880);
		assert(s.count() == 172800 && s1.count() == 172800);
		assert(ms.count() == 172800000 && ms1.count() == 172800000);
		assert(mic.count() == 172800000000 && mic1.count() == 172800000000);
		assert(ns.count() == 172800000000000 && ns1.count() == 172800000000000);
	}

	{
		time_t tt = 172800;
		millisec_t ms;
		ms.from_time_t(tt);
		assert(tt == ms.to_time_t());

		timeval tv{ 172800, 999 };
		hours_t h{ tv };
		assert(h.to_timeval().tv_sec == tv.tv_sec && h.to_timeval().tv_usec == tv.tv_usec);

		timespec ts{ 172800, 999 };
		minutes_t m{ ts };
		auto tsss = m.to_timespec();
		assert(m.to_timespec().tv_sec == ts.tv_sec && m.to_timespec().tv_nsec == ts.tv_nsec);
	}

	{
		time_moment tm{ 172800, 10 };
		days_t d{ tm };
		assert(d.count() == 2);		
	}

	{        
		time_type ms_since_ep{ 1493307502629 };        
		time_type ns{ 629000000 };

		time_moment tm{ 0, ns };
        tm.set_sec_epoch( ms_since_ep / 1000 );
		date_time dt_right{ 2017, date_time::apr, 27, 15, 38, 22, 629000000 };

		
		millisec_t ms{ tm };
		date_time a = ms.to_date_time();
		auto count = ms.count();

        static constexpr time_type julian_sec_before_epoch{ 2440588 * ratio::day_ratio_sec };

		assert(ms.count() == ms_since_ep + julian_sec_before_epoch * 1000 );
		assert(ms.to_date_time() == dt_right);

        {
            auto dayt = ms.to_date_time();
            assert( dayt.year() == 2017 );
            assert( dayt.month() == date_time::apr );
            assert( dayt.day() == 27 );
            assert( dayt.hour() == 15 );
            assert( dayt.minute() == 38 );
            assert( dayt.sec() == 22 );
            assert( dayt.nsec() == 629000000 );
        }

        ms += days_t{ 365 };
        auto dayt = ms.to_date_time();

        nanosec_t tt{ dayt };
        assert( dayt.year() == 2018 );
        assert( dayt.month() == date_time::apr );
        assert( dayt.day() == 27 );
        assert( dayt.hour() == 15 );
        assert( dayt.minute() == 38 );
        assert( dayt.sec() == 22 );
        assert( dayt.nsec() == 629000000 );
               
        {
            nanosec_t nnns;
            nnns.from_nsec_since_epoch();
            auto nss3 = nnns.to_std_duration< std::chrono::nanoseconds >();
            time_type ss1 = nnns.count() - julian_sec_before_epoch * 1000000000;
            time_type ss2 = std::chrono::duration_cast< std::chrono::nanoseconds >( std::chrono::system_clock::now().time_since_epoch() ).count();
            time_type ss3 = nss3.count();
            assert( ( ss1 == ss2 && ss1 == ss3 ) );
        }

        {
            std::chrono::nanoseconds mns{ 999999999 };
            nanosec_t ns1;
            ns1.from_std_duration( mns );
            time_type traj = ns1.count_since_epoch();
            auto mns1 = ns1.to_std_duration< std::chrono::nanoseconds >();
            assert( traj == mns.count() && mns == mns1 );
        }
	}
}
int main()
{		
	time_info_test();
	time_moment_date_time_test();	
	return 0;
}
