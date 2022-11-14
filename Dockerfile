FROM ubuntu:latest

RUN apt-get update
RUN DEBIAN_FRONTEND=noninteractive apt-get install -y clang make git binutils libc6-dev gdb sudo

WORKDIR /home/app
