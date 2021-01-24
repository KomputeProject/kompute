FROM ubuntu:18.04

RUN apt-get update -y
RUN apt-get install -y wget
RUN apt-get install -y gnupg
RUN apt-get install -y ca-certificates
RUN apt-get install -y software-properties-common

# Repository to install latest cmake
RUN wget -O - https://apt.kitware.com/keys/kitware-archive-latest.asc 2>/dev/null | apt-key add -
RUN apt-add-repository 'deb https://apt.kitware.com/ubuntu/ bionic main'
# Refresh repositories
RUN apt-get update -y

RUN apt-get install -y cmake g++
RUN apt-get install -y libvulkan-dev
RUN apt-get install -y git
RUN apt-get install -y g++-8 gcc-8
RUN apt-get install -y libx11-dev zlib1g-dev
RUN apt-get install -y libxext-dev

# Run swiftshader via env VK_ICD_FILENAMES=/swiftshader/vk_swiftshader_icd.json
RUN git clone https://github.com/google/swiftshader swiftshader-build
RUN CC="/usr/bin/gcc-8" CXX="/usr/bin/g++-8" cmake swiftshader-build/. -Bswiftshader-build/build/
RUN cmake --build swiftshader-build/build/. --parallel 12
RUN mv swiftshader-build/build/Linux/ swiftshader/
RUN rm -rf swiftshader-build/

# Setup Python
RUN apt-get install -y python3-pip

RUN mkdir builder
WORKDIR /builder


