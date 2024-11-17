NAME=hello-cpp
VERSION=0.0.1
IMAGE_DEV=$(NAME)-dev

image-dev:
	docker build \
		--build-arg HTTP_PROXY=$(HTTP_PROXY) \
		--build-arg HTTPS_PROXY=$(HTTPS_PROXY) \
		-t $(IMAGE_DEV):$(VERSION) -f Dockerfile.dev .

dev-env:
	docker run -d --rm \
		--network host \
		-v $(PWD):/app \
		-v /var/run/docker.sock:/var/run/docker.sock \
		-w /app \
		--name $(IMAGE_DEV) \
		$(IMAGE_DEV):$(VERSION) \
		tail -f /dev/null
