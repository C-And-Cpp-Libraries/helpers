#ifndef _HELPERS_SAMPLE_STORAGE_H_
#define _HELPERS_SAMPLE_STORAGE_H_

#include <map>
#include <mutex>
#include <deque>
#include <numeric>

#include "details/sample_storage_details.h"

namespace helpers
{

namespace benchmarking
{

/// \brief A thread safe time duration samples storage class,
/// capable of storing time data sorted by keys.
/// Can calucale average time for the specified key.

template< typename _key,
          typename _sample_duration = std::chrono::nanoseconds,
          typename _clock = std::chrono::high_resolution_clock,
          typename = type_traits::enable_if_duration< _sample_duration >,
          typename = type_traits::enable_if_clock< _clock > >
class sample_storage
{
public:
    using key_type = details::decay_t< _key >;
    using time_point = typename _clock::time_point;
    using clock_type = _clock;
    using sample_type = _sample_duration;

private:
    using _key_in = details::val_or_ref< _key >;

    struct time_data
    {
        sample_type sum{ sample_type{ 0 } };
        std::deque< sample_type > samples;

        void update( const sample_type& s, size_t max_samples );

        sample_type average() const noexcept{ return sum / samples.size(); }
    };

public:
    // If max_samples is 0, the number of samples is unlimited, otherwise the circular buffer approach will be used.
    explicit sample_storage( size_t max_samples = 0 ) noexcept : m_max_samples( max_samples ){}

    void add_timestamp( _key_in key );
    bool remove_key_data( _key_in key ) noexcept;

    sample_type average_time( _key_in key ) const;
    bool key_present( _key_in key ) const noexcept;
    const std::deque< sample_type >& samples( _key_in key ) const;

    static sample_storage< key_type, sample_type, clock_type >* instance() noexcept;

private:
    mutable std::mutex m_mutex;
    size_t m_max_samples{ 0 };
    std::map< key_type, time_data > m_data;
    std::map< key_type, time_point > m_waiting_samples;
};

}// benchmarking

}// helpers

#endif

#include "details/sample_storage.impl"
