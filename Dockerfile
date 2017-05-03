

FROM ubuntu:16.04

RUN apt-get update
RUN apt-get install -y build-essential git 
RUN apt-get install -y g++-arm-linux-gnueabihf gcc-arm-linux-gnueabihf libncurses-dev automake libtool

RUN git clone https://github.com/libusb/libusb.git
RUN cd libusb && ./bootstrap.sh
RUN cd libusb && ./configure --host=arm-linux-gnueabihf -disable-udev --prefix=/usr/arm-linux-gnueabihf
RUN cd libusb && make && make install

RUN git clone https://github.com/gittup/ncurses.git
RUN cd ncurses && ./configure --host=arm-linux-gnueabihf --prefix=/usr/arm-linux-gnueabihf



ENTRYPOINT ["make"]
CMD ["--help"]




