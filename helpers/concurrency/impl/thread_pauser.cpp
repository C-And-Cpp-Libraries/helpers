#include "thread_pauser.h"

#include <cassert>

namespace helpers
{

namespace concurrency
{

void thread_pauser::trigger_resume()
{
    std::lock_guard< std::mutex >{ m_mutex };

    if( m_status != status::running && m_status != status::waiting_to_resume )
    {
        if( m_status == status::paused )
        {
            m_status = status::waiting_to_resume;
            m_cv.notify_all();
        }
        else if( m_status != status::running )
        {
            m_status = status::running;
        }
    }
}

void thread_pauser::trigger_pause()
{
    std::lock_guard< std::mutex >{ m_mutex };

    if( m_status != status::paused && m_status != status::waiting_to_pause )
    {
        m_status = status::waiting_to_pause;
    }
}

void thread_pauser::pause_point()
{
    std::unique_lock< std::mutex > l{ m_mutex };

    if( m_status == status::waiting_to_pause )
    {
        assert( std::this_thread::get_id() == m_target_thread_id );

        m_status = status::paused;
        m_cv.wait( l, [ & ]{ return m_status != status::paused; } );
        m_status = status::running;
    }
}

auto thread_pauser::get_status() const -> const status&
{
    std::lock_guard< std::mutex >{ m_mutex };
    return m_status;
}

void thread_pauser::set_target_thread_id( const std::thread::id& id )
{
    std::lock_guard< std::mutex >{ m_mutex };
    m_target_thread_id = id;
}

const std::thread::id& thread_pauser::target_thread_id() const
{
    std::lock_guard< std::mutex >{ m_mutex };
    return m_target_thread_id;
}

}// concurrency

}// helpers
