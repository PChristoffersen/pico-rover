#!/usr/bin/env bash

set -e

rm -rf /opt/pico
mkdir -p /opt/pico

cd /opt/pico
git clone -b master https://github.com/raspberrypi/pico-sdk.git --depth=1
( cd pico-sdk && git submodule update --init )


git clone -b master https://github.com/raspberrypi/pico-examples.git
git clone -b master https://github.com/raspberrypi/pico-extras.git
git clone -b master https://github.com/raspberrypi/pico-playground.git

git clone -b main https://github.com/FreeRTOS/FreeRTOS-Kernel.git FreeRTOS-Kernel
cp -a FreeRTOS-Kernel FreeRTOS-Kernel-SMP
( cd FreeRTOS-Kernel-SMP && git checkout smp )

