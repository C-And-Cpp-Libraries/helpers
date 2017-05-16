#ifndef _HELPERS_THREAD_POOL_H_
#define _HELPERS_THREAD_POOL_H_

#include <atomic>
#include <thread>
#include <future>
#include <mutex>
#include <condition_variable>
#include <queue>
#include <algorithm>
#include <functional>
#include <set>

#include "../type_traits/type_traits.h"

namespace helpers
{

namespace concurrency
{

using task_id = uint64_t;
enum class task_priority{ low, medium, high };

template< typename Result >
using task_result = std::pair< task_id, std::future< Result > >;

namespace details
{

class task_handler;

class task final
{
public:
    task() = default;
    task( task_id id, const task_priority& priority, std::function< void() >&& func ) :
        m_id( id ), m_priority( priority ), m_task_func( std::move( func ) ){}

    task( task&& other ) : m_id( other.m_id ), m_priority( other.m_priority )
    {
        other.m_task_func.swap( m_task_func );
        other.m_id = 0;
        other.m_priority = task_priority::low;
    }

    task& operator=( task&& other )
    {
        m_id = other.m_id;
        m_priority = other.m_priority;
        other.m_task_func.swap( m_task_func );

        other.m_id = 0;

        return *this;
    }

    void run()
    {
        try
        {
        m_task_func();
        }
        catch( const std::bad_function_call& ){}
    }

    task_id id() const { return m_id; }
    const task_priority& priority() const { return m_priority; }

    inline bool operator<( const task& other ) const
    {
        return ( m_priority < other.m_priority ) ? true : ( m_id < other.m_id );
    }

    inline bool operator==( const task_id& other_id ) const { return m_id == other_id; }

private:
    task_id m_id;
    task_priority m_priority;
    std::function< void() > m_task_func;
};

class task_queue final : public std::priority_queue< task >
{
public:
    bool erase( task_id id )
    {
        auto it = std::find( c.begin(), c.end(), id );
        if( it != this->c.end() )
        {
            c.erase( it );
            std::make_heap( c.begin(), c.end(), comp );
            return true;
        }

        return false;
    }

    reference top()
    {
        return c.front();
    }
};

class task_handler final
{
    friend class task;

public:
    ~task_handler()
    {
        // Notify all waiting threads upon destruction
        m_tasks_status_cv.notify_all();
    }

    void run_next_task()
    {
        task t;

        {
            std::lock_guard< std::mutex >{ m_tasks_mutex };
            if( m_task_queue.empty() )
            {
                return;
            }

            t = std::move( m_task_queue.top() );
            m_task_queue.pop();
        }

        t.run();
        notify_task_finished( t.id() );
    }

    task_id add_task( const task_priority& priority, std::function< void() >&& func )
    {
        std::lock_guard< std::mutex >{ m_tasks_mutex };

        task_id id{ next_id() };
        m_task_queue.emplace( id, priority, std::move( func ) );
        m_present_task_ids.emplace( id );

        return id;
    }

    bool erase_task( task_id id )
    {
        std::lock_guard< std::mutex >{ m_tasks_mutex };

        if( m_task_queue.erase( id ) )
        {
            m_present_task_ids.erase( id );
            return true;
        }

        return false;
    }

    void clean()
    {
        std::lock_guard< std::mutex >{ m_tasks_mutex };

        while( !m_task_queue.empty() )
        {
            m_present_task_ids.erase( m_task_queue.top().id() );
            m_task_queue.pop();
        }
    }

    size_t total_tasks() const
    {
        std::lock_guard< std::mutex >{ m_tasks_mutex };
        return m_present_task_ids.size();
    }

    size_t queued_tasks() const
    {
        std::lock_guard< std::mutex >{ m_tasks_mutex };
        return m_task_queue.size();
    }

    template< typename TimeoutType >
    bool wait_until_finished( const TimeoutType& timeout ) const
    {
        bool result{ true };

        std::unique_lock< std::mutex > l{ m_tasks_mutex };

        if( m_present_task_ids.size() )
        {
            if( timeout.count() )
            {
                result = m_tasks_status_cv.wait_for( l, timeout, [ this ](){ return !m_present_task_ids.size(); } );
            }
            else
            {
                m_tasks_status_cv.wait( l, [ this ](){ return !m_present_task_ids.size(); } );
            }
        }

        return result;
    }

private:
    task_id next_id()
    {
        task_id last_id{  m_present_task_ids.size()?
                        *m_present_task_ids.rbegin() : 0 };

        return ++last_id;
    }

    void notify_task_finished( task_id id )
    {
        std::lock_guard< std::mutex >{ m_tasks_mutex };
        m_present_task_ids.erase( id );

        if( m_present_task_ids.empty() )
        {
            m_tasks_status_cv.notify_all();
        }
    }

private:
    std::set< task_id > m_present_task_ids;
    task_queue m_task_queue;
    mutable std::mutex m_tasks_mutex;
    mutable std::condition_variable m_tasks_status_cv;
};

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
    task_result< typename std::result_of< Func( Args... ) >::type >
    add_task( const task_priority& priority, Func&& func, Args&&... args  );

    // Remove all pending tasks, those being already executed are not affected
    void clean_pending_tasks() ;

    // Remove queued task with specific id. Returns false in there is no such task_id in queue
    bool unqueue_task( task_id id ) ;

    // Total number of tasks, both pending and being executed
    size_t total_tasks() const ;

    // Number of pending tasks
    size_t queue_size() const ;

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
    size_t workers_number() const ;

    // Number of threads scheduled to be removed
    size_t workers_to_remove() const ;

    // Static thread_pool created with default constructor
    static thread_pool& get_instance();

private:
    void add_worker();
    void clean_removed_workers();
    bool thread_needs_to_break() ;

private:
    std::atomic_bool m_is_running{ true };
    uint64_t m_workers_number{ 0 };
    uint64_t m_workers_to_remove{ 0 };

    details::task_handler m_task_handler;
    std::vector< std::thread > m_worker_pool;

    mutable std::mutex m_workers_mutex;
    mutable std::condition_variable m_worker_cv;
};

///// implementation

template< typename Func, typename... Args >
task_result< typename std::result_of< Func( Args... ) >::type >
thread_pool::add_task( const task_priority& priority, Func&& func, Args&&... args  )
{
    if( !m_is_running )
    {
        throw std::logic_error{ "Object is being destroyed" };
    }

    {
        std::lock_guard< std::mutex >{ m_workers_mutex };
        clean_removed_workers();
    }

    using PackTask = std::packaged_task< typename std::result_of<Func( Args... ) >::type() >;
    auto pack_task = std::make_shared< PackTask >( std::bind( std::forward< Func >( func ), std::forward< Args >( args )... ) );
    auto result = pack_task->get_future();

    task_id id{ m_task_handler.add_task( priority, [ pack_task ]
        {
            ( *pack_task )();
        } ) };

    m_worker_cv.notify_one();

    return { id, std::move( result ) };
}

template< typename TimeoutType >
bool thread_pool::wait_until_finished( const TimeoutType& timeout ) const
{
    static_assert( type_traits::is_duration< TimeoutType >::value, "Timeout should be of type std::chrono" );
    return m_task_handler.wait_until_finished( timeout );
}

} // concurrency

} // helpers

#endif
