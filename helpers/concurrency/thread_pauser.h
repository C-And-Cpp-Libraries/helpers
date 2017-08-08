#ifndef HELPERS_THREAD_PAUSER
#define HELPERS_THREAD_PAUSER

#include <thread>
#include <condition_variable>

namespace helpers
{

namespace concurrency
{
/// \class thread_pauser
/// Pauses and resumes target thread execution
/// Should only be used to pause one thread, otherwise may degrade the efficiency
/// of the target threads due frequent contested mutex locks
class thread_pauser
{
public:
    enum class status{ waiting_to_pause, paused, waiting_to_resume, running };

public:
    thread_pauser() = default;
    thread_pauser( const std::thread::id& target_thread_id ) :
        m_target_thread_id( target_thread_id ){}

    /// \brief Resume paused execution of the target thread
    void trigger_resume();

    /// \brief Schedule target thread pause
    /// The thread will be paused upon the next pause_point() call
    void trigger_pause();

    /// \brief Puts the thread it's called from to sleep until trigger_resume() is called
    /// Should only be called in the scope of the target thread, will trigger assert otherwise(in debug)
    void pause_point();

    void set_target_thread_id( const std::thread::id& id );
    const std::thread::id& target_thread_id() const;

    const status& get_status() const;

private:
    std::thread::id m_target_thread_id;
    status m_status{ status::running };

    mutable std::mutex m_mutex;
    std::condition_variable m_cv;
};

}// concurrency

}// helpers


#endif
