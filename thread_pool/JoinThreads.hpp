#pragma once

#include <thread>
#include <vector>


namespace thread_pool {

class JoinThreads {
public:
    explicit JoinThreads(std::vector<std::thread>& threads):
        _threads(threads)
    {}

    ~JoinThreads() {
        for(size_t i = 0; i < _threads.size(); ++i) {
            if(_threads[i].joinable()) {
                _threads[i].join();
            }
        }
    }
private:
    std::vector<std::thread>& _threads;
};
}

