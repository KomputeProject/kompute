
build_kompute_builder:
	docker build .. -f KomputeBuilder.Dockerfile -t axsauze/kompute-builder:0.2

push_kompute_builder: build_kompute_builder
	docker push axsauze/kompute-builder:0.2

build_swiftshader:
	docker build .. -f Swiftshader.Dockerfile -t axsauze/swiftshader:0.1

push_swiftshader: build_swiftshader
	docker push axsauze/swiftshader:0.1

build_vulkan_sdk:
	docker build .. -f VulkanSDK.Dockerfile -t axsauze/vulkan-sdk:0.1

push_vulkan_sdk: build_vulkan_sdk
	docker push axsauze/vulkan-sdk:0.1

