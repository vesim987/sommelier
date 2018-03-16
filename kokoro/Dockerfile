FROM debian:stretch

RUN apt-get update && \
    apt-get -y upgrade && \
    apt-get -y install build-essential \
                       debhelper \
                       devscripts \
                       git \
                       libsystemd-dev \
                       libwayland-dev \
                       libwayland-bin \
                       libxcb-composite0-dev \
                       pkg-config

VOLUME /src

CMD ["make", "-C", "/src", "deb"]
