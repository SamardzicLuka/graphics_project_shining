FROM ubuntu:latest

WORKDIR /app

RUN apt-get update && \
    apt-get install -y cmake make g++ libgl1-mesa-dev \
	libx11-dev libxi-dev libglfw3 libglfw3-dev libassimp-dev libglm-dev \
	libxinerama-dev libxxf86vm-dev libxcursor-dev libfreetype6-dev && \
    rm -rf /var/lib/apt/lists/*

COPY . /app

RUN ([ -d build ] && rm -rf build || true) && mkdir build
WORKDIR /app/build

RUN cmake -G "Unix Makefiles" ..
RUN make

WORKDIR /app

CMD ["./project_base"]


