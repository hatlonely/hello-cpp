#include <gtest/gtest.h>
#include <mutex>

namespace testing::call_once {

TEST(CallOnceTest, CallOnce) {
    std::once_flag flag;

    auto once_func = [&] {
        std::cout << "called once" << std::endl;
    };

    for (int i = 0; i < 3; ++i) {
        std::call_once(flag, once_func);
    }
}

}  // namespace testing::call_once
