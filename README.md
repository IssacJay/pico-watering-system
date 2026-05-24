# pico-watering-system


rm -rf build
mkdir build
cd build
cmake .. -DPICO_SDK_PATH=~/pico-sdk -DPICO_TOOLCHAIN_PATH=/Users/issacthomas/GitHub/pico_resources/gcc-arm-none-eabi-10.3-2021.10
cmake --build .


ls /dev/tty.*
screen /dev/cu.usbmodemXXXX 115200

screen /dev/cu.usbmodem14101 115200
screen /dev/tty.usbmodem14101 115200