
####### SRC Build Params #######

CC=/c/Users/axsau/scoop/apps/gcc/current/bin/g++.exe
CF=~/Programming/lib/clang+llvm-10.0.0-x86_64-linux-gnu-ubuntu-18.04/bin/clang-format


####### Shader Build Params #######

SCMP=/c/VulkanSDK/1.2.141.2/Bin32/glslangValidator.exe


####### Main Target Rules #######

build: build_shaders
	$(CC) \
		-Wall \
		src/* \
		-std=c++11 \
		-I"./src/" \
		-I"C:\\VulkanSDK\\1.2.141.2\\Include\\" \
		-L"C:\\VulkanSDK\\1.2.141.2\\Lib\\" \
		-lvulkan-1 \
		-o ./bin/main.exe

build_linux:
	g++ \
		-Wall \
		src/* \
		-std=c++11 \
		-I"./src" \
		-lvulkan \
		-o ./bin/main

build_shaders:
	$(SCMP) -V shaders/glsl/computeheadless.comp -o shaders/glsl/computeheadless.comp.spv

docker_seldon_run:
	docker run \
		--rm -it \
		-p 5000:5000 \
		axsauze/vulkan-seldon:0.1 \
		seldon-core-microservice Model REST

docker_seldon_run_gpu:
	docker run \
		--rm -it \
		--gpus all \
		-p 5000:5000 \
		axsauze/vulkan-seldon:0.1 \
		seldon-core-microservice Model REST

docker_seldon_push:
	docker push axsauze/vulkan-seldon:0.1

docker_seldon_build: 
	docker build . -f Dockerfile.seldon -t axsauze/vulkan-seldon:0.1

docker_vulkan_build:
	docker build . -t axsauze/vulkan-sum:0.1 

push_vulkan_docker:
	docker push axsauze/vulkan-sum:0.1

format:
	$(CF) -i -style="{BasedOnStyle: mozilla, IndentWidth: 4}" src/*.cpp src/*.h src/*.hpp

clean:
	rm ./bin/main.exe;

run:
	./bin/main.exe;

