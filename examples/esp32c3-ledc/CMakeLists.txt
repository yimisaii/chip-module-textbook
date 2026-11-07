cmake_minimum_required(VERSION 3.16)

set(COMPONENTS main)
include($ENV{IDF_PATH}/tools/cmake/project.cmake)
project(esp32c3_ledc)

if(NOT IDF_TARGET STREQUAL "esp32c3")
    message(FATAL_ERROR "This experiment requires IDF_TARGET=esp32c3.")
endif()
