//! \see https://en.cppreference.com/w/cpp/thread/jthread

#include <gtest/gtest.h>
#include <future>
#include <thread>

namespace testing::jthread {

TEST(JThreadTest, Join) {
    std::promise<void> p;
    auto f = p.get_future().share();

    // std::jthread 会在析构时自动 join()
    std::jthread t1([&f] {
        std::cout << "t1 start" << std::endl;
        f.get();
        std::cout << "t1 wake up" << std::endl;
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
        std::cout << "t1 end" << std::endl;
    });

    std::jthread t2([&f] {
        std::cout << "t2 start" << std::endl;
        f.get();
        std::cout << "t2 wake up" << std::endl;
        std::this_thread::sleep_for(std::chrono::milliseconds(150));
        std::cout << "t2 end" << std::endl;
    });

    p.set_value();
    std::cout << "main end" << std::endl;
}

TEST(JThreadTest, Stop) {
    std::jthread t([](std::stop_token s) {
        while (!s.stop_requested()) {
            std::cout << "running" << std::endl;
            std::this_thread::sleep_for(std::chrono::milliseconds(100));
        }
    });

    std::this_thread::sleep_for(std::chrono::milliseconds(378));
    t.request_stop();
    std::cout << "main end" << std::endl;
}

}  // namespace testing::jthread
