codegen:
	mkdir -p gen/cpp
	/app/build/vcpkg_installed/arm64-linux/tools/protobuf/protoc \
		-I=. --cpp_out=gen/cpp --grpc_out=gen/cpp \
		--plugin=protoc-gen-grpc=/app/build/vcpkg_installed/arm64-linux/tools/grpc/grpc_cpp_plugin \
		helloworld.proto
