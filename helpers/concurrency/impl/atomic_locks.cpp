#include "../atomic_locks.h"

#include <cassert>

namespace helpers
{

namespace concurrency
{

void spinlock::lock() noexcept
{
    while( m_flag.test_and_set( std::memory_order_acquire ) );
}

bool spinlock::try_lock() noexcept
{
    return !m_flag.test_and_set( std::memory_order_acquire );
}

void spinlock::unlock() noexcept
{
    m_flag.clear( std::memory_order_release );
}

//

void rw_spinlock::lock( const lock_mode& mode ) noexcept
{
    return mode == lock_mode::read? lock_read() : lock_write();
}

bool rw_spinlock::try_lock( const lock_mode& mode, uint32_t attempts ) noexcept
{
    return mode == lock_mode::read? try_lock_read( attempts ) : try_lock_write( attempts );
}

void rw_spinlock::unlock( const lock_mode& mode ) noexcept
{
    return mode == lock_mode::read? unlock_read() : unlock_write();
}

void rw_spinlock::lock_read() noexcept
{
    while( true )
    {
        uint_fast32_t old_lock{ m_lock & 0x7fffffff };
        uint_fast32_t new_lock{ old_lock + 1 };

        if( m_lock.compare_exchange_weak( old_lock, new_lock, std::memory_order_acquire ) )
        {
            return;
        }
    }
}

bool rw_spinlock::try_lock_read( uint32_t attempts ) noexcept
{
    for( uint32_t attempt{ 0 }; attempt < attempts; ++attempt )
    {
        uint_fast32_t old_lock{ m_lock & 0x7fffffff };
        uint_fast32_t new_lock{ old_lock + 1 };

        if( m_lock.compare_exchange_strong( old_lock, new_lock, std::memory_order_acquire ) )
        {
            return true;
        }
    }

    return false;
}

void rw_spinlock::lock_write() noexcept
{
    while( true )
    {
        uint_fast32_t old_lock{ m_lock & 0x7fffffff };
        uint_fast32_t new_lock{ old_lock | 0x80000000 };

        if( m_lock.compare_exchange_weak( old_lock, new_lock, std::memory_order_acquire ) )
        {
            while( m_lock & 0x7fffffff );// wait for readers to finish
            return;
        }
    }
}

bool rw_spinlock::try_lock_write( uint32_t attempts ) noexcept
{
    for( uint32_t attempt{ 0 }; attempt < attempts; ++attempt )
    {
        uint_fast32_t expected{ 0 };
        if( m_lock.compare_exchange_strong( expected, 0x80000000, std::memory_order_acquire ) )
        {
            return true;
        }
    }

    return false;
}

void rw_spinlock::unlock_read() noexcept
{
    assert( m_lock & 0x7fffffff );
    --m_lock;
}

void rw_spinlock::unlock_write() noexcept
{
    assert( m_lock == 0x80000000 );
    m_lock = 0;
}

//

rw_spinlock_guard::rw_spinlock_guard( rw_spinlock& lock, const lock_mode& mode, const lock_policy& policy ) noexcept :
    m_lock( lock ),
    m_mode( mode )
{
    if( policy == lock_policy::instant )
    {
        m_lock.lock( mode );
    }
}

rw_spinlock_guard::~rw_spinlock_guard()
{
    if( m_owns_lock )
    {
        m_lock.unlock( m_mode );
    }
}

void rw_spinlock_guard::lock() noexcept
{
    m_lock.lock( m_mode );
    m_owns_lock = true;
}

bool rw_spinlock_guard::try_lock( uint32_t attempts ) noexcept
{
    m_owns_lock = m_lock.try_lock( m_mode, attempts );
    return m_owns_lock;
}

void rw_spinlock_guard::unlock() noexcept
{
    m_lock.unlock( m_mode );
    m_owns_lock = false;
}

void rw_spinlock_guard::release() noexcept
{
    m_owns_lock = false;
}

}// concurrency

}//locking
