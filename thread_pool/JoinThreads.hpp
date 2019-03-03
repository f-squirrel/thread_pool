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
        for(auto& t : _threads) {
            if(t.joinable()) {
                t.join();
            }
        }
    }
private:
    std::vector<std::thread>& _threads;
};
}

