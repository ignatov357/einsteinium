FROM ubuntu:16.04
COPY ./testcoin.conf /root/.testcoin/testcoin.conf
COPY ./testcoin.conf /testcoin/testcoin.conf
COPY . /testcoin
WORKDIR /testcoin
#shared libraries and dependencies
RUN apt update
RUN apt-get install -y build-essential libtool autotools-dev automake pkg-config libssl-dev libevent-dev bsdmainutils
RUN apt-get install -y libboost-system-dev libboost-filesystem-dev libboost-chrono-dev libboost-program-options-dev libboost-test-dev libboost-thread-dev
RUN apt-get install -y software-properties-common
RUN add-apt-repository ppa:bitcoin/bitcoin
RUN apt-get update
RUN apt-get install -y libdb4.8-dev libdb4.8++-dev
RUN apt-get install -y libminiupnpc-dev
RUN apt-get install -y libzmq3-dev
RUN ./autogen.sh
RUN ./configure
RUN make
RUN make install
#open service port
CMD ["testcoind", "--printtoconsole"]