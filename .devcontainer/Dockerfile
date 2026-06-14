# Use a Node-based devcontainer image so npm/node-gyp work out of the box.
FROM mcr.microsoft.com/devcontainers/javascript-node:1-22-bookworm

# Avoid interactive prompts
ARG DEBIAN_FRONTEND=noninteractive

# Install build tools for native builds and recursive dependency builds.
RUN apt-get update && apt-get install -y --no-install-recommends \
    build-essential \
    git \
    make \
    cmake \
    ninja-build \
    python3 \
    doxygen \
    pkg-config \
    ca-certificates \
    curl \
    mingw-w64 \
    gcc \
    g++ \
    clangd \
    clang-format \
    ripgrep \
    && apt-get clean && rm -rf /var/lib/apt/lists/*

# Create workspace directory
RUN mkdir -p /workspaces/node-nstool
WORKDIR /workspaces/node-nstool

# Use bash as default shell
SHELL ["/bin/bash", "-lc"]
