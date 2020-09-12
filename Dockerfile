FROM ubuntu:20.10

RUN apt-get update && apt-get install -y g++ cmake make

WORKDIR /usr/src/app

COPY . .

RUN mkdir lang_bin && cd lang_bin/ && cmake ../ && cmake --build .
