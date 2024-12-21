REGISTRY_ENDPOINT ?= docker.io
REGISTRY_NAMESPACE ?= hatlonely
REPOSITORY=hello-cpp
REPOSITORY_DEV=$(REPOSITORY)-dev
VERSION=$(shell git describe --tags --always 2>/dev/null || echo "unknown")

# 检测系统类型
UNAME_S := $(shell uname -s)

# 根据系统类型设置环境变量
ifeq ($(UNAME_S),Linux)
    VCPKG_FORCE_SYSTEM_BINARIES=1
endif
ifeq ($(UNAME_S),Darwin)
    # macOS 特定的环境变量
endif
ifeq ($(UNAME_S),Windows_NT)
    # Windows 特定的环境变量
endif

image-dev:
	docker build \
		--build-arg HTTP_PROXY=172.25.144.1:7080 \
		--build-arg HTTPS_PROXY=172.25.144.1:7080 \
		-t $(REGISTRY_ENDPOINT)/$(REGISTRY_NAMESPACE)/$(REPOSITORY_DEV):$(VERSION) \
		-f Dockerfile.dev .

dev-env:
	docker run -d --rm \
		--network host \
		-e VCPKG_FORCE_SYSTEM_BINARIES=$(VCPKG_FORCE_SYSTEM_BINARIES) \
		-v $(PWD):/app \
		-v $(HOME)/.cache.ubuntu22/vcpkg:/root/.cache/vcpkg \
		-v $(HOME)/.ssh:/root/.ssh \
		-v /var/run/docker.sock:/var/run/docker.sock \
		-w /app \
		--name $(REPOSITORY_DEV) \
		$(REGISTRY_ENDPOINT)/$(REGISTRY_NAMESPACE)/$(REPOSITORY_DEV):$(VERSION) \
		tail -f /dev/null
