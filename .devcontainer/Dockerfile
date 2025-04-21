FROM mcr.microsoft.com/devcontainers/base:ubuntu

# Install core tools
RUN apt-get update && apt-get install -y \
    clang-14 \
    lldb-14 \
    lld-14 \
    ca-certificates \
    cmake \
    ninja-build \
    build-essential \
    curl \
    unzip \
    zip \
    wget \
    git \
    gnupg \
    software-properties-common \
    pkg-config && \
    update-alternatives --install /usr/bin/clang clang /usr/bin/clang-14 100 && \
    update-alternatives --install /usr/bin/clang++ clang++ /usr/bin/clang++-14 100

RUN apt-get update && apt-get install -y \
    liburing2 \
    liburing-dev

# Install Docker
RUN install -m 0755 -d /etc/apt/keyrings && \
    curl -fsSL https://download.docker.com/linux/ubuntu/gpg -o /etc/apt/keyrings/docker.asc && \
    chmod a+r /etc/apt/keyrings/docker.asc && \
    echo "deb [arch=$(dpkg --print-architecture) signed-by=/etc/apt/keyrings/docker.asc] https://download.docker.com/linux/ubuntu \
    $(. /etc/os-release && echo \"${UBUNTU_CODENAME:-$VERSION_CODENAME}\") stable" | tee /etc/apt/sources.list.d/docker.list > /dev/null && \
    apt-get update
RUN apt-get install -y docker-ce docker-ce-cli containerd.io docker-buildx-plugin docker-compose-plugin

# Install Bazel
RUN curl -fsSL https://bazel.build/bazel-release.pub.gpg | gpg --dearmor -o /etc/apt/trusted.gpg.d/bazel.gpg && \
    echo "deb [arch=amd64] https://storage.googleapis.com/bazel-apt stable jdk1.8" | tee /etc/apt/sources.list.d/bazel.list && \
    apt-get update && apt-get install -y bazel

# Install vcpkg
RUN git clone https://github.com/microsoft/vcpkg.git /vcpkg && \
    /vcpkg/bootstrap-vcpkg.sh

# Set environment variables
ENV VCPKG_ROOT=/vcpkg \
    PATH=$VCPKG_ROOT:$PATH

# Clean up
RUN apt-get clean && rm -rf /var/lib/apt/lists/*
