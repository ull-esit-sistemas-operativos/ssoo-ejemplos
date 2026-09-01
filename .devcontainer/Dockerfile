FROM mcr.microsoft.com/devcontainers/cpp:3-ubuntu24.04

ARG REINSTALL_CMAKE_VERSION_FROM_SOURCE="none"

# Optionally install the cmake for vcpkg
COPY ./reinstall-cmake.sh /tmp/

RUN if [ "${REINSTALL_CMAKE_VERSION_FROM_SOURCE}" != "none" ]; then \
        chmod +x /tmp/reinstall-cmake.sh && /tmp/reinstall-cmake.sh ${REINSTALL_CMAKE_VERSION_FROM_SOURCE}; \
    fi \
    && rm -f /tmp/reinstall-cmake.sh

# [Optional] Uncomment this section to install additional vcpkg ports.
# RUN su vscode -c "${VCPKG_ROOT}/vcpkg install <your-port-name-here>"

# [Optional] Uncomment this section to install additional packages.
RUN apt update && export DEBIAN_FRONTEND=noninteractive \
    && apt -y install software-properties-common \
    && add-apt-repository ppa:ubuntu-toolchain-r/test \
    && apt -y install --no-install-recommends \
        g++-15 strace ltrace htop memstat iotop sysstat procps lsof net-tools netcat-openbsd

# Setup GCC/G++ 15 as default
RUN update-alternatives --install /usr/bin/gcc gcc /usr/bin/gcc-15 100 \
    && update-alternatives --install /usr/bin/g++ g++ /usr/bin/g++-15 100