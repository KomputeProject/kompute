FROM nvidia/vulkan:1.3-470

RUN apt update -y
RUN apt install g++ -y

RUN mkdir /workspace
WORKDIR /workspace

COPY . /workspace

RUN make build_linux
