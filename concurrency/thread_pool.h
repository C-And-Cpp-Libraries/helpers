#ifndef HELPERS_THREAD_POOL
#define HELPERS_THREAD_POOL

#include <deque>
#include <atomic>
#include <thread>
#include <future>
#include <random>
#include <mutex>
#include <condition_variable>

#include "../type_traits/type_traits.h"

namespace helpers
{

namespace concurrency
{

namespace details
{

} //details

class thread_pool
{
public:
    explicit thread_pool( size_t workers_number = std::thread::hardware_concurrency() );
    thread_pool( const thread_pool& ) = delete;
    thread_pool( thread_pool&& ) = delete;
    thread_pool& operator==( const thread_pool& ) = delete;
    thread_pool& operator==( thread_pool&& ) = delete;
    ~thread_pool();

    template< typename Func, typename... Args >
    auto add_task( Func&& func, Args&&... args  ) -> std::future< typename std::result_of< Func( Args... ) >::type >;

    // Remove all pending tasks, those being already executed are not affected
    void clean_pending_tasks();

    // Total number of tasks, both pending and being executed
    size_t total_tasks_number() const noexcept;

    // Number of pending tasks
    size_t queue_size() const noexcept;

    // Block until all tasks are finished. On timeout returns false
    template< typename TimeoutType = std::chrono::milliseconds >
    bool wait_until_finished( const TimeoutType& timeout = TimeoutType{ 0 } ) const;

    // Add threads to the pool
    void add_workers( uint32_t number );

    // Schedule threads to be removed from the pool.
    // Threads will be stopped only after finishing their current job
    // The actual stopped threads' objects will be removed after adding a new job
    void schedule_remove_workers( uint32_t number );

    // Total number of workers
    size_t workers_number() const noexcept;

    // Number of threads scheduled to be removed
    size_t workers_to_remove() const noexcept;

    // Static thread_pool created with default constructor
    static thread_pool& get_instance();

private:
    void add_worker();
    void clean_removed_workers();

private:
    std::atomic_bool m_is_running{ true };
    size_t m_workers_number{ 0 };
    size_t m_workers_to_remove{ 0 };

    size_t m_tasks_number{ 0 };

    std::vector< std::thread > m_worker_pool;
    std::deque< std::function< void() > > m_pending_tasks;

    mutable std::mutex m_tasks_mutex;
    mutable std::mutex m_workers_mutex;
    mutable std::condition_variable m_worker_cv;
    mutable std::condition_variable m_tasks_status_cv;
};

///// implementation

template< typename Func, typename... Args >
auto thread_pool::add_task( Func&& func, Args&&... args  ) -> std::future< typename std::result_of< Func( Args... ) >::type >
{
    if( !m_is_running )
    {
        throw std::logic_error{ "Object is being destroyed" };
    }

    using Task = std::packaged_task< typename std::result_of<Func( Args... ) >::type() >;
    auto task = std::make_shared< Task >( std::bind( std::forward< Func >( func ), std::forward< Args >( args )... ) );
    auto result = task->get_future();

    {
        std::lock_guard< std::mutex >{ m_tasks_mutex };

        // Remove possibly stopped threads from pool
        clean_removed_workers();

        m_pending_tasks.emplace_back( [ task, this ]
                                      {
                                          if( task->valid() )
                                            ( *task )();

                                           std::lock_guard< std::mutex >{ m_tasks_mutex };
                                          --m_tasks_number;
                                      } );

        ++m_tasks_number;
    }

    m_worker_cv.notify_one();

    return result;
}

template< typename TimeoutType >
bool thread_pool::wait_until_finished( const TimeoutType& timeout ) const
{
    static_assert( type_traits::is_duration< TimeoutType >::value, "Timeout should be of type std::chrono" );

    bool result{ true };

    std::unique_lock< std::mutex > l{ m_tasks_mutex };

    if( m_tasks_number )
    {
        if( timeout.count() )
        {
            result = m_tasks_status_cv.wait_for( l, timeout, [ this ](){ return !m_tasks_number; } );
        }
        else
        {
            m_tasks_status_cv.wait( l, [ this ](){ return !m_tasks_number; } );
        }
    }

    return result;
}

} // concurrency

} // helpers

#endif
