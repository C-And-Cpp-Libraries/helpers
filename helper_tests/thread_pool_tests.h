#include <iostream>
#include <functional>

#include "../concurrency/thread_pool.h"
#include "../test.h"

using namespace helpers;
using namespace helpers::test;
using namespace helpers::concurrency;

namespace thread_pool_tests
{

void thread_pool_test()
{
    try{
    auto task = [](int i)->int{ std::this_thread::sleep_for( std::chrono::seconds{ 2 } ); return i; };
    std::vector< task_result< int > > results;
    uint64_t workers_number{ 0 };
    uint64_t total_tasks{ 0 };
    uint64_t queued_tasks{ 0 };

    {
        thread_pool t{ 2 };

        workers_number = TEST_EXEC_FUNC( TEST_NAME, SHOULD_THROW::NO, std::bind( &thread_pool::workers_number, &t ) );
        TEST_DYNAMIC_ASSERT( workers_number == 2, "Wrong number of workers 1 assert" );

        for( int  i{ 0 }; i < 10; ++i )
        {
            results.emplace_back( t.add_task( task_priority::high, task, i ) );
        }

        total_tasks = TEST_EXEC_FUNC( TEST_NAME, SHOULD_THROW::NO, std::bind( &thread_pool::total_tasks, &t ) );
        queued_tasks = TEST_EXEC_FUNC( TEST_NAME, SHOULD_THROW::NO, std::bind( &thread_pool::queue_size, &t ) );

        TEST_DYNAMIC_ASSERT( total_tasks == 10, "Wrong number of total tasks assert" );

        TEST_EXEC_FUNC( TEST_NAME, SHOULD_THROW::NO, std::bind( &thread_pool::add_workers, &t, 1 ) );
        workers_number = TEST_EXEC_FUNC( TEST_NAME, SHOULD_THROW::NO, std::bind( &thread_pool::workers_number, &t ) );
        TEST_DYNAMIC_ASSERT( workers_number == 3, "Wrong number of workers 2 assert" );
        TEST_EXEC_FUNC( TEST_NAME, SHOULD_THROW::NO, std::bind( &thread_pool::schedule_remove_workers, &t, 1 ) );

        TEST_EXEC_FUNC( TEST_NAME, SHOULD_THROW::NO, std::bind( &thread_pool::clean_pending_tasks, &t ) );


        t.wait_until_finished();

        for( int  i{ 0 }; i < 10; ++i )
        {
            results.emplace_back( t.add_task( task_priority::high, task, i ) );
        }

        workers_number = TEST_EXEC_FUNC( TEST_NAME, SHOULD_THROW::NO, std::bind( &thread_pool::workers_number, &t ) );
        TEST_DYNAMIC_ASSERT( workers_number == 2, "Wrong number of workers 3 assert" );
    }

//    for( int  i{ 0 }; i < 10; ++i )
//    {
//        results.emplace_back( thread_pool::get_instance().add_task( task_priority::high, task, i ) );
//    }

    for( task_result< int >& r : results  )
    {
        try
        {
        if( r.second.valid() )
        {
            int  i = r.second.get();
            std::cout <<  i  << std::endl;
        }

    }
    catch( const std::exception& e )
    {
        std::cout << e.what() << std::endl;
        int  i = 0;
    }
    }

    }
    catch( const std::exception& e )
    {
        std::string s = e.what();
        int  i = 0;
    }
}

void run_all_tests()
{
    TEST_CASE( "thread_pool_test", thread_pool_test );
}

}
