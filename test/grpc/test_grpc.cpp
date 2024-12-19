#include <google/protobuf/stubs/common.h>
#include <grpcpp/grpcpp.h>
#include <grpcpp/impl/codegen/client_interceptor.h>
#include <grpcpp/support/status.h>
#include <gtest/gtest.h>
#include <iostream>
#include <memory>
#include <thread>
#include "gen/cpp/helloworld.grpc.pb.cc"
#include "gen/cpp/helloworld.grpc.pb.h"
#include "gen/cpp/helloworld.pb.cc"
#include "gen/cpp/helloworld.pb.h"

// 参考 https://grpc.io/docs/languages/cpp/quickstart/
// 样例代码 https://github.com/grpc/grpc/blob/v1.22.0/examples/cpp/helloworld/greeter_client.cc

namespace test_grpc {

TEST(GRPC, Example) {
    std::unique_ptr<grpc::Server> svr;
    auto server = std::thread([&svr] {
        class GreeterServiceImpl final : public helloworld::Greeter::Service {
            grpc::Status SayHello(
                grpc::ServerContext* context, const helloworld::HelloRequest* request, helloworld::HelloReply* reply
            ) override {
                std::string prefix("Hello ");
                reply->set_message(prefix + request->name());
                return grpc::Status::OK;
            }
        };

        std::string server_address("0.0.0.0:50051");
        GreeterServiceImpl service;
        grpc::ServerBuilder builder;
        builder.AddListeningPort(server_address, grpc::InsecureServerCredentials());
        builder.RegisterService(&service);
        svr = builder.BuildAndStart();
        svr->Wait();
    });

    std::this_thread::sleep_for(std::chrono::seconds(1));

    auto client = std::thread([] {
        helloworld::HelloRequest request;
        request.set_name("hatlonely");
        helloworld::HelloReply reply;
        grpc::ClientContext context;

        auto channel = grpc::CreateChannel("localhost:50051", grpc::InsecureChannelCredentials());
        auto stub = helloworld::Greeter::NewStub(channel);
        grpc::Status status = stub->SayHello(&context, request, &reply);
        if (status.ok()) {
            std::cout << "Greeter received: " << reply.message() << std::endl;
        } else {
            std::cout << "RPC failed" << std::endl;
        }
    });

    client.join();
    svr->Shutdown();
    server.join();
}

TEST(GRPC, Interceptor) {
    std::unique_ptr<grpc::Server> svr;
    auto server = std::thread([&svr] {
        class LoggingServerInterceptor : public grpc::experimental::Interceptor {
          public:
            void Intercept(grpc::experimental::InterceptorBatchMethods* methods) override {
                if (methods->QueryInterceptionHookPoint(
                        grpc::experimental::InterceptionHookPoints::PRE_SEND_INITIAL_METADATA
                    )) {
                    std::cout << "Server: Received RPC call" << std::endl;
                }
                if (methods->QueryInterceptionHookPoint(
                        grpc::experimental::InterceptionHookPoints::POST_RECV_INITIAL_METADATA
                    )) {
                    std::cout << "Server: Received RPC call" << std::endl;
                }
                std::cout << "Call Before Interceptor" << std::endl;
                methods->Proceed();
                std::cout << "Call After Interceptor" << std::endl;
            }
        };

        class LoggingServerInterceptorFactory : public grpc::experimental::ServerInterceptorFactoryInterface {
          public:
            grpc::experimental::Interceptor* CreateServerInterceptor(grpc::experimental::ServerRpcInfo* info) override {
                std::cout << info->method() << std::endl;
                auto metadata = info->server_context()->client_metadata();
                for (auto& [key, value] : metadata) {
                    std::cout << key << ": " << value << std::endl;
                }

                return new LoggingServerInterceptor();
            }
        };

        class GreeterServiceImpl final : public helloworld::Greeter::Service {
            grpc::Status SayHello(
                grpc::ServerContext* context, const helloworld::HelloRequest* request, helloworld::HelloReply* reply
            ) override {
                // std::cout << context->client_metadata().find("interceptor")->second.data() << std::endl;

                std::string prefix("Hello ");
                reply->set_message(prefix + request->name());
                return grpc::Status::OK;
            }
        };

        std::string server_address("0.0.0.0:50051");
        GreeterServiceImpl service;
        grpc::ServerBuilder builder;
        builder.AddListeningPort(server_address, grpc::InsecureServerCredentials());
        builder.RegisterService(&service);
        std::vector<std::unique_ptr<grpc::experimental::ServerInterceptorFactoryInterface>> interceptor_creators;
        interceptor_creators.emplace_back(std::make_unique<LoggingServerInterceptorFactory>());
        builder.experimental().SetInterceptorCreators(std::move(interceptor_creators));
        // builder.experimental().SetInterceptorCreators({std::make_unique<LoggingServerInterceptorFactory>()});
        svr = builder.BuildAndStart();
        svr->Wait();
    });

    std::this_thread::sleep_for(std::chrono::seconds(1));

    auto client = std::thread([] {
        class LoggingClientInterceptor : public grpc::experimental::Interceptor {
          public:
            void Intercept(grpc::experimental::InterceptorBatchMethods* methods) override {
                if (methods->QueryInterceptionHookPoint(
                        grpc::experimental::InterceptionHookPoints::PRE_SEND_INITIAL_METADATA
                    )) {
                    std::cout << "Client: Sending RPC call" << std::endl;
                }
                methods->Proceed();
            }
        };

        class LoggingClientInterceptorFactory : public grpc::experimental::ClientInterceptorFactoryInterface {
          public:
            grpc::experimental::Interceptor* CreateClientInterceptor(grpc::experimental::ClientRpcInfo* info) override {
                return new LoggingClientInterceptor();
            }
        };

        helloworld::HelloRequest request;
        request.set_name("hatlonely");
        helloworld::HelloReply reply;
        grpc::ClientContext context;
        context.AddMetadata("name", "hatlonely");

        std::vector<std::unique_ptr<grpc::experimental::ClientInterceptorFactoryInterface>> interceptor_creators;
        interceptor_creators.emplace_back(std::make_unique<LoggingClientInterceptorFactory>());
        auto channel = grpc::experimental::CreateCustomChannelWithInterceptors(
            "localhost:50051", grpc ::InsecureChannelCredentials(), {}, std::move(interceptor_creators)
        );
        auto stub = helloworld::Greeter::NewStub(channel);
        grpc::Status status = stub->SayHello(&context, request, &reply);
        if (status.ok()) {
            std::cout << "Greeter received: " << reply.message() << std::endl;
        } else {
            std::cout << "RPC failed" << std::endl;
        }
    });

    client.join();
    svr->Shutdown();
    server.join();
}

}  // namespace test_grpc
