cmake_minimum_required(VERSION 3.22)
project(HelloCPP)

# c++ 20 支持
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED True)

enable_testing()
find_package(GTest CONFIG REQUIRED)

find_package(fmt CONFIG REQUIRED)
find_package(gRPC CONFIG REQUIRED)
find_package(protobuf CONFIG REQUIRED)

SET(LIBS
    fmt::fmt
    protobuf::libprotoc protobuf::libprotobuf protobuf::libprotobuf-lite
    gRPC::gpr gRPC::grpc gRPC::grpc++ gRPC::grpc++_alts
    GTest::gtest GTest::gtest_main GTest::gmock GTest::gmock_main
)

file(GLOB_RECURSE TESTS
    "test/**.cpp"
)

add_executable(HelloCPP.test ${TESTS})
target_link_libraries(HelloCPP.test PRIVATE ${LIBS})
