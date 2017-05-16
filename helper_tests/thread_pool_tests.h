#ifndef _HELPERS_THREAD_POOL_TESTS_H_
#define _HELPERS_THREAD_POOL_TESTS_H_

#include <iostream>
#include <functional>

#include "../test.h"
#include "../concurrency/thread_pool.h"

using namespace helpers;
using namespace helpers::test;
using namespace helpers::concurrency;

namespace thread_pool_tests
{

TEST_CASE( thread_pool_test )
{
    auto task = [](int i)->int{ std::this_thread::sleep_for( std::chrono::seconds{ 2 } ); return i; };
    std::vector< task_result< int > > results;
    uint64_t workers_number{ 0 };
    uint64_t total_tasks{ 0 };
    uint64_t queued_tasks{ 0 };

    {
        thread_pool t{ 2 };

        CHECK_NOTHROW( workers_number = t.workers_number() )
        DYNAMIC_ASSERT( workers_number == 2 )

        for( int  i{ 0 }; i < 10; ++i )
        {
            results.emplace_back( t.add_task( task_priority::high, task, i ) );
        }

        CHECK_NOTHROW( total_tasks = t.total_tasks() )
        CHECK_NOTHROW( queued_tasks = t.queue_size() )

        DYNAMIC_ASSERT( total_tasks == 10 )

        CHECK_NOTHROW( t.add_workers( 1 ) )
        CHECK_NOTHROW( workers_number = t.workers_number() )
        DYNAMIC_ASSERT( workers_number == 3 )
        CHECK_NOTHROW( t.schedule_remove_workers( 1 ) )
        CHECK_NOTHROW( t.clean_pending_tasks( ) )

        CHECK_NOTHROW( t.wait_until_finished() )

        for( int i{ 0 }; i < 10; ++i )
        {
            results.emplace_back( t.add_task( task_priority::high, task, i ) );
        }

        CHECK_NOTHROW( workers_number = t.workers_number() )
        DYNAMIC_ASSERT( workers_number == 2 )
    }

    for( int i{ 0 }; i < 10; ++i )
    {
        results.emplace_back( thread_pool::get_instance().add_task( task_priority::high, task, i ) );
    }

    for( task_result< int >& r : results  )
    {
        if( r.second.valid() )
        {
            int  i = r.second.get();
        }
    }
}
TEST_CASE_END( thread_pool_test )

void run_all_tests()
{
    thread_pool_test();
}

}

#endif
