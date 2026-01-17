FROM debian:trixie-slim

WORKDIR /root

SHELL [ "/bin/bash", "-c" ]

RUN set -ex \
    \
    && DEBIAN_FRONTEND=noninteractive apt-get update \
    && DEBIAN_FRONTEND=noninteractive apt-get upgrade --no-install-recommends -y \
    && DEBIAN_FRONTEND=noninteractive apt-get install --no-install-recommends -y \
    \
    ca-certificates cmake g++-mingw-w64-x86-64 build-essential binutils-mingw-w64 \
    python3 scons git curl zip unzip tar \
    \
    && apt-get autoremove --purge -y \
    && apt-get clean

ARG LLVM_MINGW_VERSION=20250528
ENV LLVM_MINGW_NAME=llvm-mingw-${LLVM_MINGW_VERSION}-ucrt-ubuntu-22.04-x86_64
ENV LLVM_MINGW_PATH=/root/llvm-mingw

ENV PATH="$PATH:${LLVM_MINGW_PATH}/bin"

RUN curl -LO https://github.com/mstorsjo/llvm-mingw/releases/download/${LLVM_MINGW_VERSION}/${LLVM_MINGW_NAME}.tar.xz && \
  tar xf ${LLVM_MINGW_NAME}.tar.xz && \
  rm -f ${LLVM_MINGW_NAME}.tar.xz && \
  mv -f ${LLVM_MINGW_NAME} ${LLVM_MINGW_PATH}

WORKDIR /mnt