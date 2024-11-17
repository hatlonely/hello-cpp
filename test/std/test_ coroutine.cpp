//! \see https://en.cppreference.com/w/cpp/language/coroutines

#include <gtest/gtest.h>
#include <coroutine>

namespace testing::coroutine {

template <typename T>
struct Generator {
    struct promise_type {
        T current_value;

        std::suspend_always yield_value(T value) {
            std::cout << "yield_value: " << value << std::endl;
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

}  // namespace testing::coroutine
