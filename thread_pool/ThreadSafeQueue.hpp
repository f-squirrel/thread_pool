#pragma once

#include <queue>
#include <mutex>


namespace thread_pool {

template<typename Data>
class ThreadSafeQueue {
public:
    void push(Data const& data) {
        std::unique_lock<std::mutex> lock{_mutex};
        _queue.push(data);
        lock.unlock();
        _condition.notify_one();
    }

    void push( Data&& data ) {
        std::unique_lock<std::mutex> lock{_mutex};
        _queue.push(std::move(data));
        lock.unlock();
        _condition.notify_one();
    }

    bool empty() const {
        std::unique_lock<std::mutex> lock{_mutex};
        return _queue.empty();
    }

	bool size() const {
        std::unique_lock<std::mutex> lock{_mutex};
        return _queue.size();
	}

    bool try_pop(Data& poppedValue) {
        std::unique_lock<std::mutex> lock{_mutex};
        if(_queue.empty()) {
            return false;
        }

        poppedValue = std::move(_queue.front());
        _queue.pop();
        return true;
    }

    void wait_and_pop(Data& poppedValue) {
        std::unique_lock<std::mutex> lock{_mutex};
        while(_queue.empty()) {
            _condition.wait(lock);
        }

        poppedValue =_queue.front();
        _queue.pop();
    }
private:
    std::queue<Data>        _queue;
    mutable std::mutex      _mutex;
    std::condition_variable _condition;
};
}

