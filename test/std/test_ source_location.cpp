//! https://en.cppreference.com/w/cpp/utility/source_location

#include <gtest/gtest.h>
#include <source_location>

namespace testing::source_location {

// c++20 之前类似的需求必须要写成宏，使用 __FILE__ 和 __LINE__ 来获取文件名和行号
void log(const std::string_view message, const std::source_location& location = std::source_location::current()) {
    std::cout << "[" << location.file_name() << ":" << location.function_name() << ":" << location.line() << "] "
              << message << std::endl;
}

TEST(SourceLocation, SourceLocation) {
    log("Hello, World!");
}

}  // namespace testing::source_location
