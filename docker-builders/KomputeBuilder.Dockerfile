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

# Run swiftshader via env VK_ICD_FILENAMES=/swiftshader/vk_swiftshader_icd.json
RUN git clone https://github.com/google/swiftshader swiftshader-build
RUN CC="/usr/bin/gcc-8" CXX="/usr/bin/g++-8" cmake swiftshader/. -Bswiftshader/build/
RUN cmake --build swiftshader/build/. --parallel
RUN mv swiftshader/build/Linux/ swiftshader/
RUN rm -rf swiftshader-build/

RUN mkdir builder
WORKDIR /builder
COPY ./ ./

RUN cmake -Bbuild
RUN cmake --build build/. --parallel

