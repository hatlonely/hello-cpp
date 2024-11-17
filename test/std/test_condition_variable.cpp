#include <gtest/gtest.h>
#include <chrono>
#include <condition_variable>

namespace testing::condition_variable {

TEST(ConditionVariableTest, Wait) {
    std::mutex m;
    std::condition_variable cv;
    bool turn = false;

    std::thread producer([&] {
        for (int i = 0; i < 3; ++i) {
            std::unique_lock<std::mutex> lock(m);
            cv.wait(lock, [&] { return !turn; });
            std::cout << "produce" << std::endl;
            turn = true;
            cv.notify_one();
        }
    });

    std::thread consumer([&] {
        for (int i = 0; i < 3; ++i) {
            std::unique_lock<std::mutex> lock(m);
            cv.wait(lock, [&] { return turn; });
            std::cout << "consume" << std::endl;
            turn = false;
            cv.notify_one();
        }
    });

    producer.join();
    consumer.join();
}

TEST(ConditionVariableTest, WaitFor) {
    std::mutex m;
    std::condition_variable cv;
    bool stop = false;

    std::thread t([&] {
        auto start = std::chrono::system_clock::now();
        while (!stop) {
            std::unique_lock<std::mutex> lock(m);
            if (cv.wait_for(lock, std::chrono::milliseconds(100), [&] { return stop; })) {
                auto d =
                    std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now() - start);
                std::cout << "[+" << d.count() << "ms] exit" << std::endl;
                break;
            }
            auto d = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now() - start);
            std::cout << "[+" << d.count() << "ms] produce" << std::endl;
        }
    });

    std::this_thread::sleep_for(std::chrono::milliseconds(378));
    stop = true;
    cv.notify_one();
    t.join();
}

TEST(ConditionVariableTest, WaitUntil) {
    std::mutex m;
    std::condition_variable cv;
    bool stop = false;

    std::thread t([&] {
        auto start = std::chrono::system_clock::now();
        auto next = start;
        while (!stop) {
            next += std::chrono::milliseconds(100);
            std::unique_lock<std::mutex> lock(m);
            if (cv.wait_until(lock, next, [&] { return stop; })) {
                auto d =
                    std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now() - start);
                std::cout << "[+" << d.count() << "ms] exit" << std::endl;
                break;
            }
            auto d = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now() - start);
            std::cout << "[+" << d.count() << "ms] produce" << std::endl;
        }
    });

    std::this_thread::sleep_for(std::chrono::milliseconds(378));
    stop = true;
    cv.notify_one();
    t.join();
}

}  // namespace testing::condition_variable
