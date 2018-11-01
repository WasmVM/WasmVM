FROM ubuntu:cosmic

RUN apt update \
    && apt install -y gcc g++ cmake automake libtool binutils git \
    && rm -rf /var/lib/apt/lists/*

WORKDIR /root