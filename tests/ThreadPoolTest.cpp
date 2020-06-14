#include <gtest/gtest.h>

#include <thread_pool/ThreadPool.hpp>

#include <chrono>
#include <iostream>
#include <mutex>
#include <set>
#include <thread>

using namespace thread_pool;
using namespace std::chrono_literals;

struct Task {
    Task(std::set<std::thread::id>& s, std::mutex& m) : _ids(s), _mutex(m) {}
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

TEST(ThreadPool, RunMoreTasksThanThreads) {

    std::mutex mutex;
    std::set<std::thread::id> set;
    const size_t THREAD_COUNT{2u};
    const size_t TASK_COUNT{20u};
    std::vector<std::future<typename std::result_of<Task()>::type>> v;
    {
        ThreadPool pool{THREAD_COUNT};
        for (size_t i = 0; i < TASK_COUNT; ++i) {
            v.push_back(pool.submit(Task(set, mutex)));
        }

        std::this_thread::sleep_for(1s);
        EXPECT_TRUE(v.size() == TASK_COUNT);
        for (size_t i = 0; i < TASK_COUNT; ++i) {
            v[i].get();
        }
        EXPECT_EQ(pool.queueSize(), 0u);
    }
}

struct IntegerTask : public Task {
    size_t _task_num;
    IntegerTask(std::set<std::thread::id>& s, std::mutex& m, size_t task_num)
        : Task(s, m), _task_num{task_num} {}
    size_t operator()() {
        {
            std::unique_lock<std::mutex> lock{_mutex};
            _ids.insert(std::this_thread::get_id());
        }
        std::this_thread::sleep_for(1s);
        return _task_num;
    }
};

TEST(ThreadPool, ReturnIntegers) {
    std::mutex mutex;
    std::set<std::thread::id> set;
    const size_t TASK_COUNT{4u};
    std::vector<std::future<typename std::result_of<IntegerTask()>::type>> v;
    {
        ThreadPool pool{2u};
        for (size_t i = 0; i < TASK_COUNT; ++i) {
            v.push_back(pool.submit(IntegerTask(set, mutex, i)));
        }

        std::this_thread::sleep_for(1s);
        EXPECT_TRUE(v.size() == TASK_COUNT);
        for (size_t i = 0; i < TASK_COUNT; ++i) {
            EXPECT_EQ(i, v[i].get());
        }
        EXPECT_EQ(pool.queueSize(), 0u);
    }
}

struct StringTask {
    StringTask(std::set<std::thread::id>& s, std::mutex& m, size_t task_num)
        : _ids{s}, _mutex{m}, _task_num{task_num} {}
    std::string operator()() {
        const auto id = std::this_thread::get_id();
        {
            std::unique_lock<std::mutex> lock{_mutex};
            _ids.insert(id);
        }
        std::this_thread::sleep_for(1s);
        return std::to_string(_task_num);
    }
    std::set<std::thread::id>& _ids;
    std::mutex& _mutex;
    size_t _task_num;
};

TEST(ThreadPool, VariousTypesOfTasks) {
    std::mutex mutex;
    std::set<std::thread::id> set;
    const size_t TASK_COUNT{4u};
    std::vector<std::future<typename std::result_of<IntegerTask()>::type>> vi;
    std::vector<std::future<typename std::result_of<StringTask()>::type>> vs;
    {
        ThreadPool pool{4u};
        for (size_t i = 0; i < TASK_COUNT; ++i) {
            vi.push_back(pool.submit(IntegerTask(set, mutex, i)));
        }
        for (size_t i = 0; i < TASK_COUNT; ++i) {
            vs.push_back(pool.submit(StringTask(set, mutex, i)));
        }
        std::this_thread::sleep_for(1s);
        for (size_t i = 0; i < TASK_COUNT; ++i) {
            EXPECT_EQ(i, vi[i].get());
            EXPECT_EQ(std::to_string(i), vs[i].get());
        }
        EXPECT_EQ(pool.queueSize(), 0u);
    }
}

TEST(ThreadPool, Lambdas) {
    const size_t TASK_COUNT{4u};
    std::vector<std::future<size_t>> v;
    {
        ThreadPool pool{4u};
        for (size_t i = 0; i < TASK_COUNT; ++i) {
            v.push_back(pool.submit([task_num = i] {
                std::this_thread::sleep_for(1s);
                return task_num;
            }));
        }
        for (size_t i = 0; i < TASK_COUNT; ++i) {
            EXPECT_EQ(i, v[i].get());
        }
        EXPECT_EQ(pool.queueSize(), 0u);
    }
}

TEST(ThreadPool, capacity) {
    ThreadPool pool{4u};
    EXPECT_EQ(pool.capacity(), 4u);
}
