FROM ubuntu:cosmic

RUN apt update \
    && apt install -y gcc g++ cmake automake libtool binutils git vim zsh ninja-build curl locales gdb astyle libskypat-dev python \
    && rm -rf /var/lib/apt/lists/*

RUN locale-gen "en_US.UTF-8"

RUN sh -c "$(curl -fsSL https://raw.githubusercontent.com/robbyrussell/oh-my-zsh/master/tools/install.sh)"

RUN git clone --recursive --depth=1 https://github.com/WebAssembly/wabt.git

RUN cd wabt && mkdir build && cd build && cmake .. && cmake --build .

WORKDIR /root

CMD ["zsh"]