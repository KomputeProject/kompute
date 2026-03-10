
KOMPUTE_BUILDER_VERSION=0.5
SWIFTSHADER_VERSION=0.3
VULKAN_SDK_VERSION=1.3.275.0

build_kompute_builder:
	docker build .. \
		-f KomputeBuilder.Dockerfile \
		--build-arg VULKAN_SDK_VERSION=$(VULKAN_SDK_VERSION) \
		--build-arg SWIFTSHADER_VERSION=$(SWIFTSHADER_VERSION) \
		-t axsauze/kompute-builder:${KOMPUTE_BUILDER_VERSION}

push_kompute_builder:
	docker push axsauze/kompute-builder:${KOMPUTE_BUILDER_VERSION}

build_swiftshader:
	docker build .. \
		-f Swiftshader.Dockerfile \
		--build-arg VULKAN_SDK_VERSION=$(VULKAN_SDK_VERSION) \
		-t axsauze/swiftshader:${SWIFTSHADER_VERSION}

push_swiftshader:
	docker push axsauze/swiftshader:${SWIFTSHADER_VERSION}

build_vulkan_sdk:
	docker build .. \
		-f VulkanSDK.Dockerfile \
		--build-arg VULKAN_SDK_VERSION=$(VULKAN_SDK_VERSION) \
		-t axsauze/vulkan-sdk:${VULKAN_SDK_VERSION}

push_vulkan_sdk:
	docker push axsauze/vulkan-sdk:${VULKAN_SDK_VERSION}
