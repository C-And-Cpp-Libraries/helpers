#ifndef HELPERS_ATOMIC_LOCKS
#define HELPERS_ATOMIC_LOCKS

#include <chrono>
#include <atomic>

#define DEFAULT_LOCK_ATTEMPTS 5

namespace helpers
{

namespace concurrency
{

class spinlock final
{
public:
    void lock() noexcept;
    bool try_lock() noexcept;
    void unlock() noexcept;

private:
    std::atomic_flag m_flag{ ATOMIC_FLAG_INIT };
};

enum class lock_mode{ read, write };

class rw_spinlock final
{
public:
    void lock( const lock_mode& mode ) noexcept;
    bool try_lock( const lock_mode& mode, uint32_t attempts = 5 ) noexcept;
    void unlock( const lock_mode& mode ) noexcept;

private:
    void lock_read() noexcept;
    bool try_lock_read( uint32_t attempts = DEFAULT_LOCK_ATTEMPTS ) noexcept;

    void lock_write() noexcept;
    bool try_lock_write( uint32_t attempts = DEFAULT_LOCK_ATTEMPTS ) noexcept;

    void unlock_read() noexcept;
    void unlock_write() noexcept;

private:
    std::atomic_uint_fast32_t m_lock{ 0 };
};

enum class lock_policy{ instant, defer };

class rw_spinlock_guard
{
public:
    explicit rw_spinlock_guard( rw_spinlock& lock,
                                const lock_mode& mode,
                                const lock_policy& policy = lock_policy::instant ) noexcept;

    rw_spinlock_guard( const rw_spinlock_guard& ) = delete;
    rw_spinlock_guard( rw_spinlock_guard&& ) = delete;
    rw_spinlock_guard& operator=(const rw_spinlock_guard&) = delete;
    rw_spinlock_guard& operator=( rw_spinlock_guard&& ) = delete;
    ~rw_spinlock_guard();

    void lock() noexcept;
    bool try_lock( uint32_t attempts = DEFAULT_LOCK_ATTEMPTS ) noexcept;

    template< typename rep, typename period >
    bool try_lock_for( const std::chrono::duration< rep, period >& timeout ) noexcept
    {
        using namespace std::chrono;
        using clock = high_resolution_clock;
        using timeout_type = duration< rep, period >;

        auto start = clock::now();
        while( duration_cast< timeout_type >( clock::now() - start ) < timeout )
        {
            if( try_lock() ){
                return true;
            }
        }

        return false;
    }

    template< typename clock, typename duration >
    bool try_lock_until( const std::chrono::time_point< clock, duration >& timeout_point )
    {
        while( clock::now() < timeout_point )
        {
            if( try_lock() ){
                return true;
            }
        }

        return false;
    }

    void unlock() noexcept;

    bool owns_lock() const noexcept{ return m_owns_lock; }
    void release() noexcept;

private:
    rw_spinlock& m_lock;
    lock_mode m_mode;
    std::atomic_bool m_owns_lock{ false };
};

}// concurrency

}// helpers

#endif
