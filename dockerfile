# Use the official Ubuntu base image
FROM ubuntu:latest

# Source API key from build args
#   `--build-arg NYSDOT_API_KEY=`
ARG NYSDOT_API_KEY
ENV NYSDOT_API_KEY=$NYSDOT_API_KEY

# Update and install essential packages
RUN apt-get update && apt-get install -y \
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
RUN rm -rf /var/lib/apt/lists/*

# Clone the project repository
RUN git clone https://github.com/mkavanagh-23/Traffic_Dashboard.git

# Set working directory to project root
WORKDIR /Traffic_Dashboard

# Run the project build script in release mode
RUN chmod +x build.sh \
    && ./build.sh release

# Set the default shell to bash
CMD ["/bin/bash"]
