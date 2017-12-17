#!/bin/bash

#****************************************************
# Dumb Install ALL Script For Debian Based Systems  *
# squbs@straks.io                                   *
#****************************************************
sudo apt-get update
sudo apt-get install -y build-essential
sudo apt-get install -y libtool autotools-dev autoconf automake
sudo apt-get install -y libssl-dev
sudo apt-get install -y libboost-all-dev
sudo apt-get install -y pkg-config 
sudo apt-get install -y software-properties-common python-software-properties

sudo add-apt-repository -y ppa:bitcoin/bitcoin
sudo apt-get -y update

sudo apt-get install -y libdb4.8-dev
sudo apt-get install -y libdb4.8++-dev

sudo apt-get install -y libminiupnpc-dev

sudo apt-get install -y libqt4-dev libprotobuf-dev protobuf-compiler
sudo apt-get install -y libqt5gui5 libqt5core5a libqt5dbus5 qttools5-dev qttools5-dev-tools libprotobuf-dev

sudo apt-get install -y libcanberra-gtk-module
sudo apt-get install -y gtk2-engines-murrine

sudo apt-get install -y libqrencode-dev

sudo apt-get install -y libevent-dev

sudo apt-get install -y libzmq3-dev
