# Use Ubuntu as a parent image
FROM ubuntu:bionic

# Set the working directory to /app/data
WORKDIR /app
RUN mkdir /app/data
ENV DATA=/app/data

# Install dependencies for ordgeo
RUN apt-get update && apt-get install -y \
	bzip2 \
	cmake \
	g++ \
	git \
	libblas-dev \
	libboost-all-dev \
	libbz2-dev \
	libdlib-dev \
	liblapack-dev \
	python3 \
	python3-pip \
	swig \
	zlib1g-dev
RUN pip3 install \
	numpy

# Get ordgeo source from github
RUN git clone git@github.com:jesand/ordgeo /app/ordgeo

# Compile ordgeo
WORKDIR /app/ordgeo/release
RUN cmake -DCMAKE_BUILD_TYPE=Release ..
RUN make soe
ENV BIN=/app/ordgeo/release/src
