// MIT License
//
// Copyright (c) 2020 Dmitry Danilov
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
// SOFTWARE.

#pragma once

#include <condition_variable>
#include <functional>
#include <future>
#include <mutex>
#include <queue>
#include <thread>
#include <vector>

namespace thread_pool {

class ThreadPool {
private:
    class TaskWrapper {
        struct ImplBase {
            virtual void call() = 0;
            virtual ~ImplBase() = default;
        };
        std::unique_ptr<ImplBase> impl;

        template <typename F>
        struct ImplType : ImplBase {
            F f;
            ImplType(F&& f_) : f{std::move(f_)} {}
            void call() final { f(); }
        };

    public:
        template <typename F>
        TaskWrapper(F&& f)
            : impl{std::make_unique<ImplType<F>>(std::move(f))} {}

        auto operator()() { impl->call(); }
    };

    class JoinThreads {
    public:
        explicit JoinThreads(std::vector<std::thread>& threads)
            : _threads{threads} {}

        ~JoinThreads() {
            for (auto& t : _threads) {
                t.join();
            }
        }

    private:
        std::vector<std::thread>& _threads;
    };

public:
    explicit ThreadPool(
        size_t threadCount = std::thread::hardware_concurrency())
        : _done{false}, _joiner{_threads} {
        if (0u == threadCount) {
            threadCount = 1u;
        }
        _threads.reserve(threadCount);
        try {
            for (size_t i = 0; i < threadCount; ++i) {
                _threads.emplace_back(&ThreadPool::workerThread, this);
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

    template <typename FunctionT, typename... Args>
    auto submit(FunctionT f, Args... args) {
        using ResultT = typename std::result_of<FunctionT(Args...)>::type;
        std::packaged_task<ResultT()> task{std::bind(f, args...)};
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
