# Starting with a Ubuntu docker file
FROM ubuntu:rolling

# Updating
RUN apt-get update -q -y

# Installing dependencies
RUN apt-get install -q -y make
RUN apt-get install -q -y g++
RUN apt-get install -q -y cmake
RUN apt-get install -q -y libgmp-dev
RUN apt-get install -q -y libssl-dev
RUN apt-get install -q -y libboost-all-dev
RUN apt-get install -q -y tmux

# Git & ABY
RUN apt-get install -q -y git
RUN cd /home && git clone https://github.com/encryptogroup/ABY
RUN cd /home/ABY/ && mkdir build && cd build/ && cmake .. -DCMAKE_INSTALL_PREFIX=/usr/local && make && make install
RUN cd /home/ && rm -rf ABY

# Setting Working directory to /home
WORKDIR /home
RUN apt-get install -q -y iputils-ping
RUN apt-get install -q -y python3-pip
RUN pip3 install pandas
RUN pip3 install phe
RUN pip3 install numpy
RUN pip3 install xlrd
