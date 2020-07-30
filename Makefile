


build:
	/c/Users/axsau/scoop/apps/gcc/current/bin/g++.exe \
		-std=c++17 \
		-c src/main.cpp \
		-I"./external" \
		-I"C:\\VulkanSDK\\1.2.141.2\\Include" \
		-L"C:\\VulkanSDK\\1.2.141.2\\Lib\\vulkan-1.lib" \
		-o main.exe

clean:
	rm ./game;

run:
	./game;

