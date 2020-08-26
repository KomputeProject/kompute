
####### SRC Build Params #######

CC="/c/Program Files (x86)/Microsoft Visual Studio/2019/Community/MSBuild/Current/Bin/MSBuildTaskHost.exe"


####### Shader Build Params #######

ifeq ($(OS),Windows_NT)     # is Windows_NT on XP, 2000, 7, Vista, 10...
	SCMP=C:\VulkanSDK\1.2.141.2\Bin32\glslangValidator.exe
else
	SCMP=/c/VulkanSDK/1.2.141.2/Bin32/glslangValidator.exe
endif

####### Package manager #######

VCPKG=/c/Users/axsau/Programming/lib/vcpkg/vcpkg

####### Main Target Rules #######

run_cmake:
	cmake \
		-Bbuild \
		-DCMAKE_TOOLCHAIN_FILE=C:\\Users\\axsau\\Programming\\lib\\vcpkg\\scripts\\buildsystems\\vcpkg.cmake \
		-DCMAKE_EXPORT_COMPILE_COMMANDS=1 \
		-G "Visual Studio 16 2019"

clean_cmake:
	rm -rf build/

install_python_reqs:
	python -m pip install -r scripts/requirements.txt

build_shaders:
	python scripts/convert_shaders.py \
		--shader-path shaders/glsl \
		--shader-binary $(SCMP) \
		--header-path src/include/kompute/shaders/ \
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

build_single_header:
	quom \
		--include_directory \
		"src/include/" \
		"single_include/AggregateHeaders.cpp" \
		"single_include/kompute/Kompute.hpp"

format:
	clang-format -i -style="{BasedOnStyle: mozilla, IndentWidth: 4}" src/*.cpp src/include/kompute/*.hpp

clean:
	find src -name "*gch" -exec rm {} \; || "No ghc files"
	rm ./bin/main.exe || echo "No main.exe"

run:
	./bin/main.exe;

