#include <ctpl_stl.h>
#include <gtest/gtest.h>

namespace testing::vit_vit_ctpl {

TEST(VitVitCtpl, ThreadPool) {
    ctpl::thread_pool pool(4);

    pool.push([](int id) {
        std::cout << "hello from " << id << std::endl;
        std::this_thread::sleep_for(std::chrono::milliseconds(300));
        std::cout << "world from " << id << std::endl;
    });
    pool.push(
        [](int id, int arg1, int arg2) {
            std::cout << "hello from " << id << " with args " << arg1 << " " << arg2 << std::endl;
            std::this_thread::sleep_for(std::chrono::milliseconds(200));
            std::cout << "world from " << id << " with args " << arg1 << " " << arg2 << std::endl;
        },
        123, 456
    );
    auto future = pool.push([](int id) {
        std::cout << "hello from " << id << std::endl;
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
        std::cout << "world from " << id << std::endl;
        return 123456;
    });
    EXPECT_EQ(future.get(), 123456);

    pool.stop(true);  // 析构的时候也会自动调用
}

TEST(VitVitCtpl, ThreadPoolException) {
    ctpl::thread_pool pool(4);

    auto future = pool.push([](int id) {
        std::cout << "hello from " << id << std::endl;
        std::this_thread::sleep_for(std::chrono::milliseconds(300));
        std::cout << "world from " << id << std::endl;
        throw std::runtime_error("exception from thread");
    });

    try {
        future.get();
    } catch (const std::exception& e) {
        std::cout << "exception caught: " << e.what() << std::endl;
    }
}

}  // namespace testing::vit_vit_ctpl
