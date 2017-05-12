#ifndef SAMPLE_STORAGE
#define SAMPLE_STORAGE

#include <map>
#include <mutex>
#include <deque>
#include <numeric>

#include "details/sample_storage_details.h"

namespace helpers
{

namespace benchmarking
{

template< typename _key,
          typename _sample_duration = std::chrono::nanoseconds,
          typename _clock = std::chrono::system_clock,
          typename = details::enable_if_duration< _sample_duration >,
          typename = details::enable_if_clock< _clock > >
class sample_storage
{
    using key_type = details::decay_t< _key >;
    using time_point = typename _clock::time_point;
    using clock_type = _clock;
    using sample_type = _sample_duration;

    using _key_in = details::val_or_ref< _key >;

private:
    struct time_data
    {
        sample_type sum{ sample_type{ 0 } };
        std::deque< sample_type > samples;

        void update( const sample_type& s, size_t max_samples ) noexcept
        {
            if( std::numeric_limits< sample_type >::max() - sum < s )
            {
                throw std::overflow_error{ "Adding the timestamp would result in average calc overflow" };
            }

            samples.emplace_back( s );

            if( max_samples && samples.size() == max_samples )
            {
                sum -= samples.front();
                samples.pop_front();
            }

            sum += s;
        }

        sample_type average() const noexcept{ return sum / samples.size(); }
    };

public:
    explicit sample_storage( size_t max_samples = 0 ) noexcept : m_max_samples( max_samples ){}

    void add_timestamp( _key_in key )
    {
        auto t = clock_type::now();

        std::lock_guard< std::mutex >{ m_mutex };

        auto waiting_it = m_waiting_samples.find( key );
        if( waiting_it == m_waiting_samples.end() )
        {
            m_waiting_samples.emplace( key, t );
        }
        else
        {
            const time_point& start = waiting_it->second;
            if( t < start )
            {
                throw std::invalid_argument{ "End time should be >= start time" };
            }

            sample_type sample{ std::chrono::duration_cast< sample_type >( t - start ) };
            m_waiting_samples.erase( waiting_it );
            m_data[ key ].update( sample, m_max_samples );
        }
    }

    bool remove_key_data( _key_in key ) noexcept
    {
        std::lock_guard< std::mutex >{ m_mutex };
        return ( m_waiting_samples.erase( key ) || m_data.erase( key ) );
    }

    sample_type average_time( _key_in key ) const
    {
        std::lock_guard< std::mutex >{ m_mutex };

        auto it = m_data.find( key );
        if( it == m_data.end() )
        {
            throw std::out_of_range{ "Key not found" };
        }

        return it->second.average();
    }

    const std::deque< sample_type >& samples( _key_in key ) const noexcept
    {
        std::lock_guard< std::mutex >{ m_mutex };

        auto it = m_data.find( key );
        if( it == m_data.end() )
        {
            throw std::out_of_range{ "Key not found" };
        }

        return it->second.samples;
    }

    bool key_present( _key_in key ) const noexcept
    {
        std::lock_guard< std::mutex >{ m_mutex };
        return ( m_waiting_samples.count( key ) || m_data.count( key ) );
    }

    static sample_storage< key_type, sample_type, clock_type >* instance() noexcept
    {
        static sample_storage< key_type, sample_type, clock_type > s;
        return &s;
    }

private:
    mutable std::mutex m_mutex;
    size_t m_max_samples{ 0 };
    std::map< key_type, time_data > m_data;
    std::map< key_type, time_point > m_waiting_samples;
};

}// benchmarking

}// helpers

#endif
