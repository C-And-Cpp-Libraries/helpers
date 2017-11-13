#ifndef _HELPERS_THREAD_POOL_TESTS_H_
#define _HELPERS_THREAD_POOL_TESTS_H_

#include <array>
#include <chrono>
#include <iostream>
#include <functional>

#include "test.h"
#include "thread_pool.h"
#include "atomic_locks.h"

using namespace helpers::concurrency;

namespace concurrency_tests
{

void check_task_results( std::vector<std::future<int>>& futures, const std::array<int, 10>& ref )
{
    std::array<int, 10> results = {};
    int val{ 0 };
    for( std::future<int>& f : futures )
    {
        CHECK_NOTHROW( val = f.get() );
        results[ val ] = val;
    }

    futures.clear();
    DYNAMIC_ASSERT( results == ref );
}

TEST_CASE( thread_pool_test )
{
    auto task = [](int i)->int{ std::this_thread::sleep_for( std::chrono::milliseconds{ 100 } ); return i; };

    std::vector< std::future< int > > futures;
    uint64_t workers_number{ 0 };
    uint64_t total_tasks{ 0 };
    uint64_t queued_tasks{ 0 };

    std::array<int, 10> values{ 0, 1, 2, 3, 4, 5, 6, 7, 8, 9 };

    thread_pool t{ 2 };

    CHECK_NOTHROW( workers_number = t.workers_number() )
    DYNAMIC_ASSERT( workers_number == 2 )

    for( auto i : values )
    {
        futures.emplace_back( t.add_task( task, i ) );
    }

    std::this_thread::sleep_for( std::chrono::milliseconds{ 50 } );

    CHECK_NOTHROW( total_tasks = t.total_tasks_number() )
    DYNAMIC_ASSERT( total_tasks == values.size() )
    CHECK_NOTHROW( queued_tasks = t.queue_size() )
    DYNAMIC_ASSERT( queued_tasks == values.size() - workers_number )

    std::this_thread::sleep_for( std::chrono::seconds{ 2 } );

    DYNAMIC_ASSERT( !t.total_tasks_number() )
    DYNAMIC_ASSERT( !t.queue_size() )

    CHECK_NOTHROW( t.wait_until_finished() )

    check_task_results( futures, values );

    uint64_t prev_worker_num{ workers_number };
    uint64_t workers_number_add{ 1 };
    CHECK_NOTHROW( t.add_workers( workers_number_add ) )
    CHECK_NOTHROW( workers_number = t.workers_number() )
    DYNAMIC_ASSERT( workers_number == prev_worker_num + workers_number_add )
    CHECK_THROW( t.schedule_remove_workers( 1000 ) )
    CHECK_NOTHROW( t.schedule_remove_workers( 1 ) )
    CHECK_NOTHROW( t.clean_pending_tasks() )

    for( auto i : values )
    {
        futures.emplace_back( t.add_task( task, i ) );
    }

    CHECK_NOTHROW( workers_number = t.workers_number() )
    DYNAMIC_ASSERT( workers_number == 2 );
}

TEST_CASE( atomic_locks_test )
{
    using namespace std::chrono;

    // spinlock
    {
        spinlock s;
        std::thread t{ []( spinlock& s )
        {
            s.lock();
            std::this_thread::sleep_for( std::chrono::milliseconds{ 100 } );
            s.unlock();
        }, std::ref( s ) };

        try
        {
            auto start = system_clock::now();
            std::this_thread::sleep_for( std::chrono::milliseconds{ 50 } );
            DYNAMIC_ASSERT(!s.try_lock())
            s.lock();
            auto end = system_clock::now();
            t.join();
            s.unlock();
            DYNAMIC_ASSERT( duration_cast< milliseconds >( end - start ).count() >= 99 );
        }
        catch(...)
        {
            t.join();
            throw;
        }
    }

    //rw_spinlock
    {
        //read
        {
            rw_spinlock rws;
            std::thread t{ []( rw_spinlock& rws )
            {
                rws.lock( lock_mode::read );
                std::this_thread::sleep_for( std::chrono::milliseconds{ 100 } );
                rws.unlock( lock_mode::read );
            }, std::ref( rws ) };

            try
            {
            auto start = system_clock::now();
            std::this_thread::sleep_for( std::chrono::milliseconds{ 50 } );
            rws.lock( lock_mode::read );
            DYNAMIC_ASSERT( rws.try_lock( lock_mode::read ) );
            rws.unlock( lock_mode::read );
            rws.unlock( lock_mode::read );
            DYNAMIC_ASSERT( !rws.try_lock( lock_mode::write ) )
            rws.lock( lock_mode::write );
            t.join();
            rws.unlock( lock_mode::write );
            auto end = system_clock::now();
            DYNAMIC_ASSERT( duration_cast< milliseconds >( end - start ).count() >= 99 );
            }
            catch(...)
            {
                t.join();
                throw;
            }
        }

        //write

        {
            std::array<lock_mode, 2> modes{ lock_mode::read, lock_mode::write };

            for( const auto& mode : modes )
            {
                rw_spinlock rws;
                std::thread t{ []( rw_spinlock& rws )
                {
                    rws.lock( lock_mode::write );
                    std::this_thread::sleep_for( std::chrono::milliseconds{ 100 } );
                    rws.unlock( lock_mode::write );
                }, std::ref( rws ) };

                try
                {
                    auto start = system_clock::now();
                    std::this_thread::sleep_for( std::chrono::milliseconds{ 50 } );
                    DYNAMIC_ASSERT( !rws.try_lock( lock_mode::read ) );
                    DYNAMIC_ASSERT( !rws.try_lock( lock_mode::write ) );
                    rws.lock( mode );
                    t.join();
                    rws.unlock( mode );
                    auto end = system_clock::now();
                    DYNAMIC_ASSERT( duration_cast< milliseconds >( end - start ).count() >= 99 );
                }
                catch(...)
                {
                    t.join();
                    throw;
                }
            }
        }
    }
}

}// concurrency_tests

#endif
