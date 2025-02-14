FROM ubuntu:24.04

# Install git, git-lfs, colorama and build tools
RUN echo "deb http://security.ubuntu.com/ubuntu focal-security main universe" > /etc/apt/sources.list.d/ubuntu-focal-sources.list && \
    apt-get update && apt-get -y install git git-lfs python3-colorama cmake g++ build-essential libncurses5 libusb-1.0-0 gdb && \
    git lfs install

# Install cubeclt
RUN mkdir /temp && cd /temp && git clone https://github.com/HyperloopUPV-H8/cubeclt.git && \
    cd cubeclt && git lfs pull && chmod +x cubeclt_1.16.0_installer.sh && \
    echo | LICENSE_ALREADY_ACCEPTED=1 ./cubeclt_1.16.0_installer.sh

ENV PATH="$PATH:/opt/st/stm32cubeclt_1.16.0/GNU-tools-for-STM32/bin:/opt/st/stm32cubeclt_1.16.0/CMake/bin:/opt/st/stm32cubeclt_1.16.0/Ninja/bin"

ENV PYTHONPATH="/workspaces/template-project/Tests/VirtualMCU/src/:$PYTHONPATH"
