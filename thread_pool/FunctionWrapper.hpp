#pragma once

#include <memory>

namespace thread_pool {

class FunctionWrapper {
    struct ImplBase {
        virtual void call() = 0;
        virtual ~ImplBase() {}
    };
    std::unique_ptr<ImplBase> impl;

    template <typename F>
    struct ImplType : ImplBase {
        F f;
        ImplType(F&& f_) : f(std::move(f_)) {}
        void call() final { return f(); }
    };

public:
    template <typename F>
    FunctionWrapper(F&& f)
        : impl(std::make_unique<ImplType<F>>(std::move(f))) {}

    auto operator()() { impl->call(); }

    FunctionWrapper() = default;

    FunctionWrapper(FunctionWrapper&& other) : impl(std::move(other.impl)) {}

    FunctionWrapper& operator=(FunctionWrapper&& other) {
        impl = std::move(other.impl);
        return *this;
    }

    FunctionWrapper(const FunctionWrapper&) = delete;
    FunctionWrapper(FunctionWrapper&)       = delete;
    FunctionWrapper& operator=(const FunctionWrapper&) = delete;
};
} // namespace thread_pool
