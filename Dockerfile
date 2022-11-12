FROM ubuntu:latest

RUN apt-get update
RUN DEBIAN_FRONTEND=noninteractive apt-get install -y gcc make git binutils libc6-dev gdb sudo

WORKDIR /home/app
