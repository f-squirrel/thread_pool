#pragma once

#include <condition_variable>
#include <future>
#include <mutex>
#include <queue>

#include "JoinThreads.hpp"
#include "TaskWrapper.hpp"

namespace thread_pool {

class ThreadPool {
public:
    explicit ThreadPool(
        size_t threadCount = std::thread::hardware_concurrency())
        : _done{false}, _joiner{_threads} {
        if (0u == threadCount) {
            threadCount = 4u;
        }
        _threads.reserve(threadCount);
        try {
            for (size_t i = 0; i < threadCount; ++i) {
                _threads.push_back(
                    std::thread{&ThreadPool::workerThread, this});
            }
        } catch (...) {
            _done = true;
            _queue.cv.notify_all();
            throw;
        }
    }

    ~ThreadPool() {
        _done = true;
        _queue.cv.notify_all();
    }

    size_t capacity() const { return _threads.size(); }
    size_t queueSize() const {
        std::unique_lock<std::mutex> l{_queue.m};
        return _queue.q.size();
    }

    template <typename FunctionType>
    auto submit(FunctionType f) {
        using ResultType = typename std::result_of<FunctionType()>::type;
        std::packaged_task<ResultType()> task{std::move(f)};
        auto future = task.get_future();
        {
            std::lock_guard<std::mutex> l{_queue.m};
            _queue.q.push(std::move(task));
        }
        _queue.cv.notify_all();
        return future;
    }

    ThreadPool(const ThreadPool&) = delete;
    ThreadPool(ThreadPool&) = delete;
    ThreadPool& operator=(const ThreadPool&) = delete;

private:
    void workerThread() {
        while (!_done) {
            std::unique_lock<std::mutex> l{_queue.m};
            _queue.cv.wait(l, [&] { return !_queue.q.empty() || _done; });
            if (_done)
                break;
            auto task = std::move(_queue.q.front());
            _queue.q.pop();
            l.unlock();
            task();
        }
    }

    struct TaskQueue {
        std::queue<TaskWrapper> q;
        std::mutex m;
        std::condition_variable cv;
    };

private:
    std::atomic_bool _done;
    mutable TaskQueue _queue;
    std::vector<std::thread> _threads;
    JoinThreads _joiner;
};
} // namespace thread_pool
