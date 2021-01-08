#!/bin/bash

#https://www.qemu.org/download/
function install_1() {
    wget https://download.qemu.org/qemu-5.2.0.tar.xz
    tar xvJf qemu-5.2.0.tar.xz
    cd qemu-5.2.0
    ./configure
    make
}

function install_2() {
    git clone https://git.qemu.org/git/qemu.git
    cd qemu
    git submodule init
    git submodule update --recursive
    ./configure
    make
}
