FROM ubuntu:22.04

ENV DEBIAN_FRONTEND=noninteractive

RUN apt update && apt install -y \
    build-essential gcc g++ make pkg-config \
    python3 python3-dev python3-pip python3-tk \
    python3-yaml python3-jinja2 python3-bs4 python3-lxml \
    valgrind gdb git lcov gcovr \
    check libncurses-dev zlib1g-dev \
    && rm -rf /var/lib/apt/lists/*