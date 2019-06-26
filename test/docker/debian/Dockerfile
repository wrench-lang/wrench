FROM debian:latest

RUN mkdir /app
WORKDIR /app

ADD . /app

RUN apt-get update
# RUN apt-get -y install gcc
RUN apt-get -y install build-essential
RUN apt-get -y install sudo
