#include <gtest/gtest.h>
#include <future>

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

}  // namespace testing::future
