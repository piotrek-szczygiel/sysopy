FROM ubuntu:bionic

ENV DEBIAN_FRONTEND=noninteractive
RUN apt-get update \
    && apt-get -y install --no-install-recommends apt-utils 2>&1

RUN apt-get -y install git procps lsb-release build-essential clang-format \
    fish wget unzip tzdata locales

RUN apt-get autoremove -y \
    && apt-get clean -y \
    && rm -rf /var/lib/apt/lists/*
ENV DEBIAN_FRONTEND=dialog

WORKDIR /root

RUN mkdir -p .local/bin && mkdir -p .cargo/bin
RUN git clone https://github.com/piotrek-szczygiel/.dotfiles \
    && /root/.dotfiles/install

RUN wget https://github.com/ogham/exa/releases/download/v0.8.0/exa-linux-x86_64-0.8.0.zip \
    -O exa.zip && unzip exa.zip && mv exa-linux-x86_64 /usr/bin/exa

ENV TERM xterm-256color
ENV USER root
ENV SHELL /usr/bin/fish
