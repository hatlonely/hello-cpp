cmake_minimum_required(VERSION 3.22)
project(HelloCPP)

# c++ 20 支持
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED True)

find_package(fmt CONFIG REQUIRED)
enable_testing()
find_package(GTest CONFIG REQUIRED)

SET(LIBS
    fmt::fmt
    GTest::gtest GTest::gtest_main GTest::gmock GTest::gmock_main
)

file(GLOB_RECURSE TESTS
    "test/**.cpp"
)

add_executable(HelloCPP.test ${TESTS})
target_link_libraries(HelloCPP.test PRIVATE ${LIBS})
