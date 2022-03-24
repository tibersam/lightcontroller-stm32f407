# General
This project contains the firmware written for a private Lighting project. I have build a costume board shield for this. However this project can be used to see, how to programm an stm32 with libopencm3 and cmake. Also shows how one can implement a Uart consol, configure and use uart in general, configure and use SPI (which are connected to a custome circuite that generates pulses for ws2812/sk2812 leds), Read and write from and to gpio. Use it as you see fit.

# Setup
You need:
 - cmake
 - arm-none-eabi-gcc
 - arm-none-eabi-gdb
 - st-link (https://github.com/stlink-org/stlink)
 - libopencm3 (is a submodule, use git submodule init && git submodule update)

See startcmake to how to initialise the build system. Will create a build system.

# Build target
in the build directory use make to build project
make flash will build and flash firmware to microcontroller using st-link
make debug-server uses st-link to setup a remote gdb interface. in second terminal execute make debug to open gdb

