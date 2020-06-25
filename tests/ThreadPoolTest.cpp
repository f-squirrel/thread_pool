#include <chrono>
#include <future>
#include <mutex>
#include <set>
#include <stdexcept>
#include <thread>
#include <vector>

#include <gtest/gtest.h>

#include <thread_pool/ThreadPool.hpp>

using namespace thread_pool;
using namespace std::chrono_literals;

TEST(ThreadPool, RunMoreTasksThanThreads) {
    const size_t THREAD_COUNT{2u};
    const size_t TASK_COUNT{20u};

    std::mutex mutex;
    size_t result = 0u;
    std::set<std::thread::id> thread_ids;

    ThreadPool pool{THREAD_COUNT};
    std::vector<std::future<void>> futures;
    for (size_t i = 0; i < TASK_COUNT; ++i) {
        futures.push_back(pool.submit([&] {
            std::this_thread::sleep_for(1ms);
            std::lock_guard<std::mutex> l{mutex};
            ++result;
            thread_ids.insert(std::this_thread::get_id());
        }));
    }
    for (auto& f : futures) {
        f.wait();
    }
    EXPECT_EQ(pool.queueSize(), 0);
    EXPECT_EQ(result, TASK_COUNT);
    EXPECT_EQ(thread_ids.size(), THREAD_COUNT);
}

TEST(ThreadPool, VariousTypesOfTasks) {
    ThreadPool pool{2u};
    auto fi = pool.submit([] { return 42; });
    auto fs = pool.submit([] { return std::string{"42"}; });
    EXPECT_EQ(fi.get(), 42);
    EXPECT_EQ(fs.get(), std::string{"42"});
}

TEST(ThreadPool, Lambdas) {
    const size_t TASK_COUNT{4u};
    std::vector<std::future<size_t>> v;
    ThreadPool pool{4u};
    for (size_t i = 0; i < TASK_COUNT; ++i) {
        v.push_back(pool.submit([task_num = i] {
            std::this_thread::sleep_for(1ms);
            return task_num;
        }));
    }
    for (size_t i = 0; i < TASK_COUNT; ++i) {
        EXPECT_EQ(i, v[i].get());
    }
    EXPECT_EQ(pool.queueSize(), 0u);
}

TEST(ThreadPool, Exception) {
    ThreadPool pool{1u};
    auto f = pool.submit([] { throw std::runtime_error{"Error"}; });

    EXPECT_THROW(f.get(), std::runtime_error);
}

TEST(ThreadPool, capacity) {
    ThreadPool pool{4u};
    EXPECT_EQ(pool.capacity(), 4u);
}

TEST(ThreadPool, EmptyQueue) {
    ThreadPool pool{4u};
    std::this_thread::sleep_for(1s);
}

static int sum(int a, int b) {
    return a + b;
}

TEST(ThreadPool, FunctionWithArgs) {
    ThreadPool pool{4u};
    auto f = pool.submit(sum, 2, 2);
    EXPECT_EQ(f.get(), 4);
}

static std::thread::id test_function(size_t delay) {
    std::this_thread::sleep_for(std::chrono::milliseconds(delay + 1));
    return std::this_thread::get_id();
}

TEST(ThreadPool, ThreadsAreReused) {
    const size_t THREAD_COUNT{4u};
    ThreadPool pool{THREAD_COUNT};
    std::vector<std::future<std::thread::id>> futures;
    std::set<std::thread::id> thread_ids;

    for (size_t i = 0u; i < THREAD_COUNT; ++i) {
        futures.push_back(pool.submit(test_function, i));
    }

    for (size_t i = 0u; i < THREAD_COUNT; ++i) {
        auto r = futures[i].get();
        auto iter = thread_ids.insert(r);
        // New thread is used
        EXPECT_TRUE(iter.second);
    }

    futures.clear();

    for (size_t i = 0u; i < THREAD_COUNT; ++i) {
        futures.push_back(pool.submit(test_function, i));
    }

    for (size_t i = 0; i < THREAD_COUNT; ++i) {
        auto r = futures[i].get();
        auto iter = thread_ids.find(r);
        EXPECT_TRUE(iter != thread_ids.end());
        thread_ids.erase(iter);
    }
}
