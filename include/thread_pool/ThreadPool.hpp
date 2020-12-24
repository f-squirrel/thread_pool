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
