FROM ubuntu:cosmic

RUN apt update \
    && apt install -y gcc g++ cmake automake libtool binutils git vim astyle gdb libskypat-dev \
    && rm -rf /var/lib/apt/lists/*

WORKDIR /root

CMD ["bash"]