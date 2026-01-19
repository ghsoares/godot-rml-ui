FROM fedora:42

WORKDIR /root

# Base setup

ENV DOTNET_NOLOGO=1
ENV DOTNET_CLI_TELEMETRY_OPTOUT=1

RUN dnf -y install --setopt=install_weak_deps=False \
  bash binutils bzip2 curl file findutils gettext git make nano patch pkgconfig python3-pip zip \
  unzip tar which xz node dotnet-sdk-8.0 && \
  pip install scons==4.9.1

# Windows setup

ARG LLVM_MINGW_VERSION=20250528
ENV LLVM_MINGW_NAME=llvm-mingw-${LLVM_MINGW_VERSION}-ucrt-ubuntu-22.04-x86_64
ENV LLVM_MINGW_PATH=/root/llvm-mingw

ENV PATH="$PATH:${LLVM_MINGW_PATH}/bin"

RUN dnf -y install --setopt=install_weak_deps=False \
  mingw32-gcc mingw32-gcc-c++ mingw32-winpthreads-static mingw64-gcc mingw64-gcc-c++ mingw64-winpthreads-static && \
  curl -LO https://github.com/mstorsjo/llvm-mingw/releases/download/${LLVM_MINGW_VERSION}/${LLVM_MINGW_NAME}.tar.xz && \
  tar xf ${LLVM_MINGW_NAME}.tar.xz && \
  rm -f ${LLVM_MINGW_NAME}.tar.xz && \
  mv -f ${LLVM_MINGW_NAME} ${LLVM_MINGW_PATH}

# Linux setup

ENV GODOT_SDK_LINUX_X86_64=/root/x86_64-godot-linux-gnu_sdk-buildroot
ENV GODOT_SDK_LINUX_X86_32=/root/i686-godot-linux-gnu_sdk-buildroot
ENV GODOT_SDK_LINUX_ARM64=/root/aarch64-godot-linux-gnu_sdk-buildroot
ENV GODOT_SDK_LINUX_ARM32=/root/arm-godot-linux-gnueabihf_sdk-buildroot
ENV PATH="$PATH:${GODOT_SDK_LINUX_X86_64}/bin"
ENV PATH="$PATH:${GODOT_SDK_LINUX_X86_32}/bin"
ENV PATH="$PATH:${GODOT_SDK_LINUX_ARM64}/bin"
ENV PATH="$PATH:${GODOT_SDK_LINUX_ARM32}/bin"

RUN dnf install -y wayland-devel && \
  curl -LO https://github.com/godotengine/buildroot/releases/download/godot-2023.08.x-4/x86_64-godot-linux-gnu_sdk-buildroot.tar.bz2 && \
  tar xf x86_64-godot-linux-gnu_sdk-buildroot.tar.bz2 && \
  rm -f x86_64-godot-linux-gnu_sdk-buildroot.tar.bz2 && \
  cd x86_64-godot-linux-gnu_sdk-buildroot && \
  ./relocate-sdk.sh && \
  cd /root && \
  curl -LO https://github.com/godotengine/buildroot/releases/download/godot-2023.08.x-4/i686-godot-linux-gnu_sdk-buildroot.tar.bz2 && \
  tar xf i686-godot-linux-gnu_sdk-buildroot.tar.bz2 && \
  rm -f i686-godot-linux-gnu_sdk-buildroot.tar.bz2 && \
  cd i686-godot-linux-gnu_sdk-buildroot && \
  ./relocate-sdk.sh && \
  cd /root && \
  curl -LO https://github.com/godotengine/buildroot/releases/download/godot-2023.08.x-4/aarch64-godot-linux-gnu_sdk-buildroot.tar.bz2 && \
  tar xf aarch64-godot-linux-gnu_sdk-buildroot.tar.bz2 && \
  rm -f aarch64-godot-linux-gnu_sdk-buildroot.tar.bz2 && \
  cd aarch64-godot-linux-gnu_sdk-buildroot && \
  ./relocate-sdk.sh && \
  cd /root && \
  curl -LO https://github.com/godotengine/buildroot/releases/download/godot-2023.08.x-4/arm-godot-linux-gnueabihf_sdk-buildroot.tar.bz2 && \
  tar xf arm-godot-linux-gnueabihf_sdk-buildroot.tar.bz2 && \
  rm -f arm-godot-linux-gnueabihf_sdk-buildroot.tar.bz2 && \
  cd arm-godot-linux-gnueabihf_sdk-buildroot && \
  ./relocate-sdk.sh

## Android setup

ENV ANDROID_SDK_ROOT=/root/sdk
ENV ANDROID_NDK_VERSION=28.1.13356709
ENV ANDROID_NDK_ROOT=${ANDROID_SDK_ROOT}/ndk/${ANDROID_NDK_VERSION}

RUN dnf -y install --setopt=install_weak_deps=False \
    java-21-openjdk-devel ncurses-compat-libs && \
    mkdir -p sdk && cd sdk && \
    export CMDLINETOOLS=commandlinetools-linux-13114758_latest.zip && \
    curl -LO https://dl.google.com/android/repository/${CMDLINETOOLS} && \
    unzip ${CMDLINETOOLS} && \
    rm ${CMDLINETOOLS} && \
    yes | cmdline-tools/bin/sdkmanager --sdk_root="${ANDROID_SDK_ROOT}" --licenses && \
    cmdline-tools/bin/sdkmanager --sdk_root="${ANDROID_SDK_ROOT}" "ndk;${ANDROID_NDK_VERSION}" 'cmdline-tools;latest' 'build-tools;35.0.1' 'platforms;android-35' 'cmake;3.31.6'

WORKDIR /mnt