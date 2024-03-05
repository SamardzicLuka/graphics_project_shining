FROM ubuntu:latest

WORKDIR /app

RUN apt-get update && \
    apt-get install -y cmake make g++ libgl1-mesa-dev libx11-dev libxi-dev && \
    rm -rf /var/lib/apt/lists/*

COPY . /app

RUN mkdir build
WORKDIR /app/build

RUN cmake -G "Unix Makefile" ..
RUN make

WORKDIR /app

CMD ["./build/project_base"]


