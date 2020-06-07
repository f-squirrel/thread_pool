#pragma once

#include <future>
#include <memory>

#include "JoinThreads.hpp"
#include "TaskWrapper.hpp"
#include "ThreadSafeQueue.hpp"

namespace thread_pool {

class ThreadPool {
public:
    explicit ThreadPool(
        size_t threadCount = std::thread::hardware_concurrency())
        : _done(false), _joiner(_threads) {
        if (0u == threadCount) {
            threadCount = 4u;
        }
        _threads.reserve(threadCount);
        try {
            for (size_t i = 0; i < threadCount; ++i) {
                _threads.push_back(
                    std::thread(&ThreadPool::workerThread, this));
            }
        } catch (...) {
            _done = true;
            throw;
        }
    }

    ~ThreadPool() { _done = true; }

    size_t capacity() const { return _threads.size(); }
    size_t queueSize() const { return _workQueue.size(); }

    template <typename FunctionType>
    std::future<typename std::result_of<FunctionType()>::type>
    submit(FunctionType f) {
        using ResultType = typename std::result_of<FunctionType()>::type;

        std::packaged_task<ResultType()> task(std::move(f));
        std::future<ResultType> res(task.get_future());
        _workQueue.push(std::move(task));
        return res;
    }

    ThreadPool(const ThreadPool&) = delete;
    ThreadPool(ThreadPool&) = delete;
    ThreadPool& operator=(const ThreadPool&) = delete;

private:
    void workerThread() {
        while (!_done) {
            TaskWrapper task;
            if (_workQueue.try_pop(task)) {
                task();
            } else {
                std::this_thread::yield();
            }
        }
    }

private:
    std::atomic_bool _done;
    ThreadSafeQueue<TaskWrapper> _workQueue;
    std::vector<std::thread> _threads;
    JoinThreads _joiner;
};
} // namespace thread_pool
