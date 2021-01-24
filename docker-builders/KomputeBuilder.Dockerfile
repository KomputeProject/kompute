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

RUN mkdir builder
WORKDIR /builder
RUN git clone 

RUN wget https://www.dropbox.com/s/d0weho6l8dovm71/libvk_swiftshader.so?dl=1 -O /usr/lib/libvk_swiftshader.so
RUN mkdir -p /usr/share/vulkan/icd.d
RUN wget https://www.dropbox.com/s/5oly49ev3vvcfdu/vk_swiftshader_icd.json?dl=1 -O /usr/share/vulkan/icd.d/vk_swiftshader_icd.json

RUN mkdir builder
WORKDIR /builder
COPY ./ ./

RUN cmake -Bbuild
RUN cmake --build build/. --parallel

