# Distributed under the OSI-approved BSD 3-Clause License.  See accompanying
# file Copyright.txt or https://cmake.org/licensing for details.

cmake_minimum_required(VERSION 3.5)

file(MAKE_DIRECTORY
  "D:/ESP-IDF/Espressif/frameworks/esp-idf-v5.1.2/components/bootloader/subproject"
  "C:/STM32/project/41 ESP32 Get Weather/build/bootloader"
  "C:/STM32/project/41 ESP32 Get Weather/build/bootloader-prefix"
  "C:/STM32/project/41 ESP32 Get Weather/build/bootloader-prefix/tmp"
  "C:/STM32/project/41 ESP32 Get Weather/build/bootloader-prefix/src/bootloader-stamp"
  "C:/STM32/project/41 ESP32 Get Weather/build/bootloader-prefix/src"
  "C:/STM32/project/41 ESP32 Get Weather/build/bootloader-prefix/src/bootloader-stamp"
)

set(configSubDirs )
foreach(subDir IN LISTS configSubDirs)
    file(MAKE_DIRECTORY "C:/STM32/project/41 ESP32 Get Weather/build/bootloader-prefix/src/bootloader-stamp/${subDir}")
endforeach()
if(cfgdir)
  file(MAKE_DIRECTORY "C:/STM32/project/41 ESP32 Get Weather/build/bootloader-prefix/src/bootloader-stamp${cfgdir}") # cfgdir has leading slash
endif()
