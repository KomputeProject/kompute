
####### SRC Build Params #######

CC=/c/Users/axsau/scoop/apps/gcc/current/bin/g++.exe
CF=~/Programming/lib/clang+llvm-10.0.0-x86_64-linux-gnu-ubuntu-18.04/bin/clang-format


####### Shader Build Params #######

SCMP=/c/VulkanSDK/1.2.141.2/Bin32/glslangValidator.exe

####### Package manager #######

VCPKG=/c/Users/axsau/Programming/lib/vcpkg/vcpkg

####### Main Target Rules #######

run_cmake:
	wcmake \
		-Bbuild \
		-DCMAKE_TOOLCHAIN_FILE=C:\\Users\\axsau\\Programming\\lib\\vcpkg\\scripts\\buildsystems\\vcpkg.cmake \
		-DCMAKE_EXPORT_COMPILE_COMMANDS=1 \
		-G "Visual Studio 16 2019"

build: clean build_shaders
	$(CC) \
		src/* \
		-w \
		-std=c++17 \
		-DDEBUG=1 \
		-DRELEASE=1 \
		-I"./external/" \
		-L"C:\\VulkanSDK\\1.2.141.2\\Lib\\" \
		-lvulkan-1 \
		-o ./bin/main.exe
		#\
		#-L"C:\\Users\\axsau\\Programming\\lib\\vcpkg\\installed\\x64-windows\\lib\\" \
		#-lspdlog \
		#\
		#-g -fexceptions -fPIC \
		#-static-libgcc -static-libstdc++ \

run_test:
	$(CC) \
		test/* \
		-Wall \
		-std=c++17 \
		-DDEBUG=1 \
		-DRELEASE=1 \
		-I"./external/" \
		-I"./src/" \
		-L"C:\\VulkanSDK\\1.2.141.2\\Lib\\" \
		-lvulkan-1 \
		-o ./bin/test.exe && \
		./bin/test.exe --success


build_linux:
	g++ \
		-g -shared-libgcc \
		-Wall \
		src/* \
		-std=c++17 \
		-I"./external/" \
		-I"./src" \
		-lvulkan \
		-o ./bin/main

build_shaders:
	python scripts/convert_shaders.py \
		--shader-path shaders/glsl \
		--shader-binary $(SCMP) \
		--header-path src/shaders \
		-v

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
	$(CF) -i -style="{BasedOnStyle: mozilla, IndentWidth: 4}" src/*.cpp src/*.hpp src/*.h

clean:
	find src -name "*gch" -exec rm {} \; || "No ghc files"
	rm ./bin/main.exe || echo "No main.exe"

run:
	./bin/main.exe;

