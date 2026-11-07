cmake_minimum_required(VERSION 3.13)

set(PICO_BOARD pico CACHE STRING "Board for this experiment")
if(NOT PICO_BOARD STREQUAL "pico")
    message(FATAL_ERROR "This experiment uses the original Raspberry Pi Pico board (PICO_BOARD=pico).")
endif()

if(NOT PICO_SDK_PATH)
    set(PICO_SDK_PATH "$ENV{PICO_SDK_PATH}")
endif()
if(NOT EXISTS "${PICO_SDK_PATH}/pico_sdk_init.cmake")
    message(FATAL_ERROR "Set PICO_SDK_PATH to an external checkout of Pico SDK 2.2.0.")
endif()
include("${PICO_SDK_PATH}/pico_sdk_init.cmake")
if(NOT PICO_SDK_VERSION_STRING VERSION_EQUAL "2.2.0")
    message(FATAL_ERROR "Pico SDK 2.2.0 required, found ${PICO_SDK_VERSION_STRING}.")
endif()

project(rp2040_dma C CXX ASM)
set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)
pico_sdk_init()

option(DMA_LAB_INJECT_MISMATCH "Flip one destination bit after the DMA completes" OFF)

add_executable(rp2040_dma src/main.c)
target_link_libraries(rp2040_dma PRIVATE pico_stdlib hardware_dma)
target_compile_options(rp2040_dma PRIVATE -Wall -Wextra)
target_compile_definitions(rp2040_dma PRIVATE
    DMA_LAB_INJECT_MISMATCH=$<BOOL:${DMA_LAB_INJECT_MISMATCH}>)
pico_enable_stdio_uart(rp2040_dma 1)
pico_enable_stdio_usb(rp2040_dma 0)
pico_add_extra_outputs(rp2040_dma)
