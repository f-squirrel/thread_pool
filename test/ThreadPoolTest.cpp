#define BOOST_TEST_DYN_LINK
#define BOOST_TEST_MODULE ThreadPoolTest
#include <boost/test/unit_test.hpp>

#include "../thread_pool/ThreadPool.hpp"
#include <chrono>
#include <iostream>
#include <mutex>
#include <set>
#include <thread>


using namespace thread_pool;
using namespace std::chrono_literals;


struct Task {
    Task(std::set<std::thread::id>& s, std::mutex& m): _ids(s), _mutex(m) {}
    void operator()() {
        {
            std::unique_lock<std::mutex> lock{_mutex};
            _ids.insert(std::this_thread::get_id());
        }
        std::this_thread::sleep_for(1s);
    }
    std::set<std::thread::id>& _ids;
    std::mutex& _mutex;
};

BOOST_AUTO_TEST_CASE( RunMoreTasksThanThreads ) {

    std::mutex mutex;
    std::set<std::thread::id> set;
    const size_t THREAD_COUNT{2u};
    const size_t TASK_COUNT{20u};
    std::vector<std::future<typename std::result_of<Task()>::type>> v;
    {
        ThreadPool pool{THREAD_COUNT};
        for(size_t i = 0; i < TASK_COUNT; ++i) {
            v.push_back(pool.submit(Task(set, mutex)));
        }

        std::this_thread::sleep_for(1s);
        BOOST_REQUIRE( v.size() == TASK_COUNT );
        for(size_t i = 0; i < TASK_COUNT; ++i) {
            v[i].get();
        }
    }
}

BOOST_AUTO_TEST_CASE( ReturnIntegers ) {

    struct IntegerTask : public Task {
        IntegerTask(std::set<std::thread::id>& s, std::mutex& m) : Task(s, m) {}
        std::thread::id operator()() {
            {
                std::unique_lock<std::mutex> lock{_mutex};
                _ids.insert(std::this_thread::get_id());
            }
            std::this_thread::sleep_for(1s);
            return std::this_thread::get_id();
        }
    };

    std::mutex mutex;
    std::set<std::thread::id> set;
    const size_t TASK_COUNT{4u};
    std::vector<std::future<typename std::result_of<IntegerTask()>::type>> v;
    {
        ThreadPool pool{2u};
        for(size_t i = 0; i < TASK_COUNT; ++i) {
            v.push_back(pool.submit(IntegerTask(set, mutex)));
        }

        std::this_thread::sleep_for(1s);
        BOOST_REQUIRE( v.size() == TASK_COUNT );
        for(size_t i = 0; i < TASK_COUNT; ++i) {
            std::cout << v[i].get() << "\n";
        }
    }
}
