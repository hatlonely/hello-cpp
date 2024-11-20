//! \see https://en.cppreference.com/w/cpp/language/coroutines

#include <gtest/gtest.h>
#include <coroutine>
#include <thread>

namespace testing::coroutine {

template <typename T>
struct Generator {
    struct promise_type {
        T current_value;

        std::suspend_always yield_value(T value) {
            std::cout << "yield_value: " << value << ". current thread " << std::this_thread::get_id() << std::endl;
            this->current_value = value;
            return {};
        }

        std::suspend_always initial_suspend() {
            std::cout << "initial_suspend" << std::endl;
            this->current_value = T{};
            return {};
        }

        std::suspend_always final_suspend() noexcept {
            std::cout << "final_suspend" << std::endl;
            return {};
        }

        Generator get_return_object() {
            std::cout << "get_return_object" << std::endl;
            return Generator{this};
        }

        void unhandled_exception() {
            std::cout << "unhandled_exception" << std::endl;
            std::terminate();
        }
    };

    promise_type* p;

    struct Iterator {
        std::coroutine_handle<promise_type> coro;

        void operator++() { coro.resume(); }

        T operator*() { return coro.promise().current_value; }

        bool operator==(std::default_sentinel_t) const { return !coro || coro.done(); }
    };

    Iterator begin() {
        auto it = Iterator{std::coroutine_handle<promise_type>::from_promise(*p)};
        it.coro.resume();
        return it;
    }

    std::default_sentinel_t end() { return {}; }
};

TEST(CoroutineTest, Generator) {
    std::cout << "main thread " << std::this_thread::get_id() << std::endl;
    auto generateNumbers = []() -> Generator<int> {
        co_yield 1;
        co_yield 2;
        co_yield 3;
    };

    auto gen = generateNumbers();
    for (auto i : gen) {
        std::cout << i << std::endl;
    }
}

struct IncAwaiter {
    int req;

    IncAwaiter(int req) : req(req) {}

    bool await_ready() const noexcept { return false; }

    void await_suspend(std::coroutine_handle<> h) const noexcept {
        std::thread([h]() {
            std::cout << "await_suspend: " << std::this_thread::get_id() << std::endl;
            std::this_thread::sleep_for(std::chrono::milliseconds(100));
            h.resume();
        }).detach();
    }

    int await_resume() const noexcept {
        auto res = req + 1;
        return res;
    }
};

struct Task {
    struct promise_type {
        int current_value;

        Task get_return_object() { return Task{std::coroutine_handle<promise_type>::from_promise(*this)}; }

        std::suspend_always initial_suspend() { return {}; }

        std::suspend_always final_suspend() noexcept { return {}; }

        void return_value(int value) { current_value = value; }

        void unhandled_exception() { std::terminate(); }
    };

    std::coroutine_handle<promise_type> coro;

    Task(std::coroutine_handle<promise_type> coro) : coro(coro) {}

    ~Task() {
        if (coro) {
            coro.destroy();
        }
    }
};

Task Inc() {
    auto res1 = co_await IncAwaiter{1};
    std::cout << "co_await IncAwaiter{1} done" << std::endl;
    auto res2 = co_await IncAwaiter{2};
    std::cout << "co_await IncAwaiter{2} done" << std::endl;

    std::cout << "res1: " << res1 << std::endl;
    std::cout << "res2: " << res2 << std::endl;
}

TEST(CoroutineTest, Awaiter) {
    auto task = Inc();
    task.coro.resume();
    std::this_thread::sleep_for(std::chrono::milliseconds(1000));
}

}  // namespace testing::coroutine
