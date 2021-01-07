cmake_minimum_required(VERSION 3.1)


#Force arm-none-eabi-gcc compiler for cmake

#include(CMakeForceCompiler)
include(./arm-gcc.cmake)


#set(CMAKE_SYSTEM_NAME Generic)

#CMAKE_FORCE_C_COMPILER(arm-none-eabi-gcc GNU)
#CMAKE_FORCE_CXX_COMPILER(arm-none-eabi-g++ GNU)

set(CMAKE_FIND_ROOT_PATH_MODE_PROGRAM NEVER)
set(CMAKE_FIND_ROOT_PATH_MODE_LIBRARY ONLY)
set(CMAKE_FIND_ROOT_PATH_MODE_INCLUDE ONLY)
set(CMAKE_FIND_ROOT_PATH_MODE_PACKAGE ONLY)

#project(ws2812)
set(TARGET ws2812)
project(${TARGET} C CXX ASM)

if(NOT DEVICE)
	set(DEVICE stm32f407)
	set(FLASH_START 0x08000000)
endif()

set(USE_STLINK ON)



#include libopencm3

set(LIBOPENCM3_DIR ${CMAKE_SOURCE_DIR}/libopencm3)
add_custom_target(libopencm3 make WORKING_DIRECTORY ${LIBOPENCM3_DIR})
include_directories(${LIBOPENCM3_DIR}/include ${CMAKE_SOURCE_DIR}/src /usr/lib/arm-none-eabi/include)
link_directories(${LIBOPENCM3_DIR}/lib /usr/lib/arm-none-eabi/newlib/armv7e-m/fpu)
add_definitions(-DSTM32F4)

set(STM32F4_FLAGS "-mfloat-abi=hard -mfpu=fpv4-sp-d16 -mthumb -mcpu=cortex-m4 -Wextra -Wimplicit-function-declaration -Wredundant-decls -Wmissing-prototypes -Wstrict-prototypes -Wundef -Wshadow -fno-common")
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wall ${STM32F4_FLAGS} -std=c11 -g -O0")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall --std=c++14 ${STM32F4_FLAGS}")
set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -T ${PROJECT_SOURCE_DIR}/linker/libopencm3_stm32f4.ld -nostartfiles --specs=rdimon.specs -g")

function(add_bin_from_elf bin elf)
	add_custom_target(${bin} ALL ${CMAKE_OBJCOPY} -Obinary ${elf} ${bin} DEPENDS ${elf})
endfunction(add_bin_from_elf)

#file(GLOB Source RELATIVE "src" "*.c")



add_executable(ws2812.elf "")

target_sources(ws2812.elf
  PUBLIC
  "src/main.c"
  "src/atx.c"
  "src/atx.h"
  "src/wait.c"
  "src/wait.h"
  "src/uart5.h"
  "src/uart5.c"
  #  "src/serial.c"
    "src/ws2812b.c"
    "src/ws2812b.h"
    "src/lightcontrol.c"
    "src/lightcontrol.h"
    "src/consol.c"
    "src/consol.h"
    "src/string_helperfunctions.c"
    "src/string_helperfunctions.h"
    "src/decoder.c"
    "src/decoder.h"
    "src/button.c"
    "src/button.h"
    "src/usart1.c"
    "src/usart1.h"
)

target_link_libraries(ws2812.elf opencm3_stm32f4 m)
add_bin_from_elf(ws2812.bin ws2812.elf)

set(BINARY ${TARGET}.bin)

add_custom_target(flash 
	COMMAND st-flash write ${BINARY} ${FLASH_START}
	DEPENDS ${TARGET}.elf
	)


add_custom_target(debug-server 
	COMMAND st-util --listen_port 4242
	DEPENDS ${TARGET}.elf
	)

add_custom_target(debug 
	COMMAND ${DEBUGGER} -command ${CMAKE_CURRENT_LIST_DIR}/remote.gdbconf ${CMAKE_CURRENT_BINARY_DIR}/${TARGET}.elf 
	DEPENDS ${TARGET}.elf
	)
add_dependencies(ws2812.elf libopencm3)
