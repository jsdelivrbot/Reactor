

FROM ubuntu:16.04

RUN apt-get update
RUN apt-get install -y build-essential git python
RUN apt-get install -y g++-arm-linux-gnueabihf gcc-arm-linux-gnueabihf libncurses-dev automake libtool libusb-1.0-0-dev

RUN git clone https://github.com/libusb/libusb.git
RUN cd libusb && ./bootstrap.sh
RUN cd libusb && ./configure --host=arm-linux-gnueabihf -disable-udev --prefix=/usr/arm-linux-gnueabihf
RUN cd libusb && make && make install

RUN git clone https://github.com/gittup/ncurses.git
RUN cd ncurses && ./configure --host=arm-linux-gnueabihf --prefix=/usr/arm-linux-gnueabihf



WORKDIR "/BuildArea"
#ENTRYPOINT ["Build.sh"]
ENTRYPOINT ["/bin/bash"]
CMD ["Build.sh"]




