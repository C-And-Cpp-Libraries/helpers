#include <iostream>
#include <string>
#include <stdexcept>
#include <chrono>
#include <cassert>
#include "time_info/time_info.h"

void time_moment_date_time_test()
{
    using namespace time_info;

    time_moment t1{ 1, 0 };
    time_moment t2{ 1, 0 };
    time_moment t3{ 1, 1 };
    time_moment t4{ 2, 1 };

    assert( t1 == t1.to_date_time().to_time_moment() );

    assert( t1 == t2 );
    assert( t1 != t3 && t1 != t4 );
    assert( t1 < t3 && t1 < t4 );
    assert( t4 > t3 && t4 > t2 && t4 > t1 );
    assert( t1 <= t2 && t1 <= t3 && t1 <= t4 );
    assert( t4 >= t1 && t4 >= t2 && t4 >= t3 && t4 >= t4 );

    date_time d1{ 2015, date_time::apr, 24, 13, 13, 13, 13, 13 };
    date_time d2{ 2015, date_time::apr, 24, 13, 13, 13, 13, 13 };
    date_time d3{ 2016, date_time::apr, 25, 13, 13, 13, 13, 13 };
    date_time d4{ 2016, date_time::jan, 26, 13, 13, 13, 13, 13 };
    date_time d5{ 2016, date_time::dec, 27, 13, 13, 13, 13, 13 };

    assert( d1 == d1.to_time_moment().to_date_time() );

    assert( d1 == d2 );
    assert( d1 < d3 && d1 < d4 && d1 < d5 );
    assert( d1 <= d1 && d1 <= d2 && d1 <= d3 && d1 <= d4 && d1 <= d5 );
    assert( d5 > d1 && d5 > d2 && d5 > d3 && d5 > d4 );
    assert( d5 >= d1 && d5 >= d2 && d5 >= d3 && d5 >= d4 && d5 >= d5 );
}

void time_info_test()
{
    using namespace time_info;

    {
        day d{ 2 };
        hour h = d;
        min m = d;
        sec s = d;
        msec ms = d;

        day d1{ 2 };
        hour h1{ d };
        min m1{ d };
        sec s1{ d };
        msec ms1{ d };

        assert( d.count() == 2 && d1.count() == 2 );
        assert( h.count() == 48 && h1.count() == 48 );
        assert( m.count() == 2880 && m1.count() == 2880 );
        assert( s.count() == 172800 && s1.count() == 172800 );
        assert( ms.count() == 172800000 && ms1.count() == 172800000 );
    }

    {
        time_moment tm{ 172800000, 10 };
        day d{ tm };
        assert( d.count() == 2 );
        assert( d.to_time_moment() == tm );
    }

    {
        time_type ms_since_ep{ 1493307502629 };

        time_moment tm{ ms_since_ep, 10 };
        date_time dt_right{ 2017, date_time::apr, 27, 15, 38, 22, 629, 10 };

        msec ms{ tm };
        assert( ms.to_time_moment() == tm );
        assert( ms.count() == ms_since_ep );
        assert( ms.to_date_time() == dt_right );

        ms = dt_right;
    }
}


int main(int argc, char **argv)
{
    time_moment_date_time_test();
    time_info_test();

    time_info::day day{ 1 };
    time_info::hour hours = std::move( day );

    hours = time_info::day{ 2 };
    time_info::time_type hours_count{ hours.count() };

    using namespace std::chrono;
    milliseconds ms = duration_cast< milliseconds >(
                system_clock::now().time_since_epoch());

    auto countt = ms.count();

    time_info::time_moment tp{ ms.count(), 0 };

    time_info::date_time ddt = tp.to_date_time();
    time_info::time_moment tp1 = ddt.to_time_moment();

    time_info::hour h{ 1 };

    int d = h.to_std_duration< std::chrono::minutes >().count();


    std::tm tm = {};
    const char* snext = ::strptime("1980-Jul-1 00:00:00.000000", "%Y-%b-%d %H:%M:%S", &tm);
    auto time_point = std::chrono::system_clock::from_time_t(std::mktime(&tm));
    long long  duration_ms = time_point.time_since_epoch() / std::chrono::milliseconds(1);
    std::cout << duration_ms << std::endl;

    time_info::date_time dt{ 2017, time_info::date_time::apr, 26, 16, 42, 52, 0, 0 };

    time_info::hour hinfo{ dt };
    size_t iii = hinfo.count();

    time_info::hour ddd = time_info::hour::from_time_t< time_info::hour >( 3600 );
    int t =ddd.count();

    return EXIT_SUCCESS;
}

//void check_overflow( count ) const
//{
//    if( std::numeric_limits< time_type >::max() / count < m_msec_ratio )
//    {
//        throw std::invalid_argument{ "Overflow error" };
//    }
//}
