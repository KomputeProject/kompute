
####### SRC Build Params #######

CC=/c/Users/axsau/scoop/apps/gcc/current/bin/g++.exe


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

build_shaders:
	$(SCMP) -V shaders/glsl/computeheadless.comp -o shaders/glsl/computeheadless.comp.spv

clean:
	rm ./bin/main.exe;

run:
	cd bin && ./main.exe;

