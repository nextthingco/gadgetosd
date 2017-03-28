from ubuntu:16.04
run apt -y update && \
	apt -y upgrade && \
	apt -y install build-essential \
	git \
	uuid-dev \
	python \
	curl \
	apt-transport-https \
	software-properties-common \
	ca-certificates \
	cmake \
	dh-make \
	librsvg2-bin \
	debhelper  && \
	curl -fsSL https://download.docker.com/linux/ubuntu/gpg | apt-key add - && \
	add-apt-repository \
	"deb [arch=amd64] https://download.docker.com/linux/ubuntu \
	$(lsb_release -cs) \
	stable" && \
	apt update && \
	apt -y install docker-ce
