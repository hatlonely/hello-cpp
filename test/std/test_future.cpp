#include <gtest/gtest.h>
#include <future>
#include <thread>

namespace testing::future {

TEST(FutureTest, SimpleFuture) {
    std::promise<int> p;
    std::future<int> f = p.get_future();
    p.set_value(10);
    EXPECT_EQ(f.get(), 10);
}

TEST(FutureTest, SharedFuture) {
    std::promise<int> p;
    std::shared_future<int> f = p.get_future().share();

    auto f1 = f;
    std::thread t1([f] { EXPECT_EQ(f.get(), 10); });
    std::thread t2([f] { EXPECT_EQ(f.get(), 10); });

    p.set_value(10);
    EXPECT_EQ(f1.get(), 10);

    t1.join();
    t2.join();
}

TEST(FutureTest, Async) {
    auto future = std::async(std::launch::async, [] {
        std::cout << "async run in thread " << std::this_thread::get_id() << std::endl;
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
        return 10;
    });
    std::cout << "main run in thread " << std::this_thread::get_id() << std::endl;
    EXPECT_EQ(future.get(), 10);
}

}  // namespace testing::future
