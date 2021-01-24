FROM ubuntu:18.04

# Base packages from default ppa
RUN apt-get update -y
RUN apt-get install -y wget
RUN apt-get install -y gnupg
RUN apt-get install -y ca-certificates
RUN apt-get install -y software-properties-common

# Repository to latest cmake
RUN wget -O - https://apt.kitware.com/keys/kitware-archive-latest.asc 2>/dev/null | apt-key add -
RUN apt-add-repository 'deb https://apt.kitware.com/ubuntu/ bionic main'

# Repository for latest git (needed for gh actions)
RUN add-apt-repository -y ppa:git-core/ppa

# Refresh repositories
RUN apt update -y

RUN apt install -y git
RUN apt-get install -y cmake g++
RUN apt-get install -y libvulkan-dev
# Swiftshader dependencies
RUN apt-get install -y libx11-dev zlib1g-dev
RUN apt-get install -y libxext-dev

COPY --from=axsauze/swiftshader:0.1 /swiftshader/ /swiftshader/

# Setup Python
RUN apt-get install -y python3-pip

RUN mkdir builder
WORKDIR /builder


