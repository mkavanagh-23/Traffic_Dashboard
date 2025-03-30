# syntax=docker/dockerfile:1
# check=skip=SecretsUsedInArgOrEnv

# Use the official Ubuntu base image
FROM ubuntu:latest

# Source branch to build from
ARG BRANCH

# Source API key from build args
#   `--build-arg NYSDOT_API_KEY=`
ARG NYSDOT_API_KEY
ENV NYSDOT_API_KEY=$NYSDOT_API_KEY

# Set the time zone
ENV TZ=America/New_York
ENV DEBIAN_FRONTEND=noninteractive
RUN apt-get update && apt-get install -y tzdata
RUN ln -fs /usr/share/zoneinfo/America/New_York /etc/localtime && \
    dpkg-reconfigure --frontend noninteractive tzdata

# Update and install essential packages
RUN apt-get install -y \
    sudo \
    curl \
    bash \
    git

# Install project build dependencies
RUN apt-get install -y \
    build-essential \
    cmake \
    pkg-config \
    libssl-dev

# (Optional) Install distro-built libraries to link against
RUN apt-get install -y \
    libcurl4-openssl-dev \
    libjsoncpp-dev \
    libpoco-dev \
    libgumbo-dev

# Clean up cached packages to reduce image size
RUN apt-get clean \
    && rm -rf /var/lib/apt/lists/*

# Clone the project repository
RUN git clone https://github.com/mkavanagh-23/Traffic_Dashboard.git

# Set working directory to project root
WORKDIR /Traffic_Dashboard

# Switch to current working branch
RUN git checkout $BRANCH

# Run the project build script in release mode
RUN chmod +x build.sh \
    && ./build.sh release

# Set the default shell to bash
CMD ["/bin/bash"]

RUN ./Traffic_Dashboard
