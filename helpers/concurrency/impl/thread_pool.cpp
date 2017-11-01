#include "thread_pool.h"
#include <algorithm>

namespace helpers
{

namespace concurrency
{

thread_pool::thread_pool( size_t thread_number )
{
    add_workers( thread_number );
}

thread_pool::~thread_pool()
{
    m_is_running = false;

    clean_pending_tasks();

    // Wake all waiting threads
    m_worker_cv.notify_all();

    for( auto& t : m_worker_pool )
    {
        if( t.joinable() )
        {
            t.join();
        }
    }

    m_tasks_status_cv.notify_all();
}

void thread_pool::clean_pending_tasks()
{
    std::lock_guard< std::mutex >{ m_tasks_mutex };

    if( !m_pending_tasks.empty() )
    {
        m_tasks_number -= m_pending_tasks.size();
        m_pending_tasks.clear();
    }
}

size_t thread_pool::total_tasks_number() const noexcept
{
    std::lock_guard< std::mutex >{ m_tasks_mutex };
    return m_tasks_number;
}

size_t thread_pool::queue_size() const noexcept
{
    std::lock_guard< std::mutex >{ m_tasks_mutex };
    return m_pending_tasks.size();
}

void thread_pool::add_workers( uint32_t number )
{
    if( number )
    {
        std::lock_guard< std::mutex >{ m_workers_mutex };

        clean_removed_workers();

        for( size_t thread{ 0 }; thread < number; ++thread )
        {
            add_worker();
        }

        m_workers_number += number;
    }
}

void thread_pool::schedule_remove_workers( uint32_t number )
{
    if( number )
    {
        std::lock_guard< std::mutex >{ m_workers_mutex };

        clean_removed_workers();

        if( number >= m_workers_number )
        {
            throw std::invalid_argument{ "Number of workers to remove is greater or equal to the avalable workers' number" };
        }

        m_workers_number -= number;
        m_workers_to_remove += number;

        // Wake possibly waiting threads to stop them
        for( size_t thread{ 0 }; thread < number; ++thread )
        {
            m_worker_cv.notify_one();
        }
    }
}

size_t thread_pool::workers_number() const noexcept
{
    std::lock_guard< std::mutex >{ m_workers_mutex };
    return m_workers_number;
}

size_t thread_pool::workers_to_remove() const noexcept
{
    std::lock_guard< std::mutex >{ m_workers_mutex };
    return m_workers_to_remove;
}

thread_pool &thread_pool::get_instance()
{
    static thread_pool p;
    return p;
}

void thread_pool::add_worker()
{
    std::thread t
    {
        [ this ]()
        {
            while( m_is_running )
            {
                std::function< void() > task;

                {
                    std::unique_lock< std::mutex > l{ m_tasks_mutex };

                    // Check before potentially waiting for a task
                    if( !m_is_running )
                    {
                        break;
                    }

                    {
                        std::lock_guard< std::mutex >{ m_workers_mutex };
                        if( m_workers_to_remove )
                        {
                            --m_workers_to_remove;
                            break;
                        }
                    }

                    // If there's no pending tasks, wait for them/for a signal to be removed/for object destruction
                    if( m_pending_tasks.empty() )
                    {
                        m_worker_cv.wait( l, [ this ](){
                            std::lock_guard< std::mutex >{ m_workers_mutex };
                            return !m_pending_tasks.empty() || m_workers_to_remove || !m_is_running; } );

                        // Check after waiting
                        if( !m_is_running )
                        {
                            break;
                        }

                        {
                            std::lock_guard< std::mutex >{ m_workers_mutex };
                            if( m_workers_to_remove )
                            {
                                --m_workers_to_remove;
                                break;
                            }
                        }
                    }

                    task = std::move( m_pending_tasks.front() );
                    m_pending_tasks.pop_front();
                }

                // Execute task
                try
                {
                    task();
                }
                catch( const std::bad_function_call& ){}

                // If there are no tasks left, notify threads waiting for the end of execution
                std::lock_guard< std::mutex >{ m_tasks_mutex };
                if( !m_tasks_number )
                {
                    m_tasks_status_cv.notify_all();
                }
            }
        }
    };

    m_worker_pool.push_back( std::move( t ) );
}

void thread_pool::clean_removed_workers()
{
    auto end = std::remove_if( m_worker_pool.begin(),
                               m_worker_pool.end(),
                               []( const std::thread& t ){ return !t.joinable(); } );

    m_worker_pool.erase( end, m_worker_pool.end() );
}

} // concurrency

} // helpers
