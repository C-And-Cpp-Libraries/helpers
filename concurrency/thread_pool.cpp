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
}

void thread_pool::clean_pending_tasks()
{
    m_task_handler.clean();
}

bool thread_pool::unqueue_task( task_id id )
{
    return m_task_handler.erase_task( id );
}

size_t thread_pool::total_tasks() const
{
    return m_task_handler.total_tasks();
}

size_t thread_pool::queue_size() const
{
    return m_task_handler.queued_tasks();
}

void thread_pool::add_workers( uint32_t number )
{
    if( !m_is_running )
    {
        throw std::logic_error{ "Object is being destroyed" };
    }

    if( number )
    {
        std::lock_guard< std::mutex >{ m_workers_mutex };

        clean_removed_workers();

        for( size_t thread{ 0 }; thread < number; ++thread )
        {
            add_worker();
            ++m_workers_number;
        }
    }
}

void thread_pool::schedule_remove_workers( uint32_t number )
{
    if( !m_is_running )
    {
        throw std::logic_error{ "Object is being destroyed" };
    }

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

size_t thread_pool::workers_number() const
{
    std::lock_guard< std::mutex >{ m_workers_mutex };
    return m_workers_number;
}

size_t thread_pool::workers_to_remove() const
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
                {
                    std::unique_lock< std::mutex > l{ m_workers_mutex };

                    // Check after waiting
                    if( thread_needs_to_break() )
                    {
                        break;
                    }

                    // If there's no pending tasks, wait for them/for a signal to be removed/for object destruction
                    if( !m_task_handler.queued_tasks() )
                    {
                        m_worker_cv.wait( l, [ this ]()
                        {
                            if( m_task_handler.queued_tasks() )
                            {
                                return true;
                            }

                            return  m_workers_to_remove || !m_is_running;
                        } );

                        // Check after waiting
                        if( thread_needs_to_break() )
                        {
                            break;
                        }
                    }
                }

                // Execute next task
                m_task_handler.run_next_task();
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

bool thread_pool::thread_needs_to_break()
{
    if( !m_is_running )
    {
        return true;
    }

    if( m_workers_to_remove )
    {
        --m_workers_to_remove;
        return true;
    }

    return false;
}

} // concurrency

} // helpers
