#pragma once

#include <memory>

namespace thread_pool {

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
    TaskWrapper(F&& f) : impl{std::make_unique<ImplType<F>>(std::move(f))} {}

    auto operator()() { impl->call(); }
};
} // namespace thread_pool
