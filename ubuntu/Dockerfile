FROM ubuntu:16.04
ENV DEBIAN_FRONTEND=noninteractive
RUN apt -y update && \
	apt -y upgrade && \
	apt -y install build-essential \
	git \
	uuid-dev \
	python \
	curl \
	apt-transport-https \
	software-properties-common \
	cmake \
	ninja-build \
	librsvg2-bin \
	ca-certificates && \
	curl -fsSL https://download.docker.com/linux/ubuntu/gpg | apt-key add - && \
	add-apt-repository "deb [arch=amd64] https://download.docker.com/linux/ubuntu $(lsb_release -cs) stable" && \
	apt update && \
	apt -y install docker-ce
