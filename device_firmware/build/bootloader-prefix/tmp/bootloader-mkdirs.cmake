# Distributed under the OSI-approved BSD 3-Clause License.  See accompanying
# file Copyright.txt or https://cmake.org/licensing for details.

cmake_minimum_required(VERSION 3.5)

file(MAKE_DIRECTORY
  "C:/esp/esp-idf/components/bootloader/subproject"
  "C:/Users/perry/Documents/GitHub/EasyDSP/device_firmware/build/bootloader"
  "C:/Users/perry/Documents/GitHub/EasyDSP/device_firmware/build/bootloader-prefix"
  "C:/Users/perry/Documents/GitHub/EasyDSP/device_firmware/build/bootloader-prefix/tmp"
  "C:/Users/perry/Documents/GitHub/EasyDSP/device_firmware/build/bootloader-prefix/src/bootloader-stamp"
  "C:/Users/perry/Documents/GitHub/EasyDSP/device_firmware/build/bootloader-prefix/src"
  "C:/Users/perry/Documents/GitHub/EasyDSP/device_firmware/build/bootloader-prefix/src/bootloader-stamp"
)

set(configSubDirs )
foreach(subDir IN LISTS configSubDirs)
    file(MAKE_DIRECTORY "C:/Users/perry/Documents/GitHub/EasyDSP/device_firmware/build/bootloader-prefix/src/bootloader-stamp/${subDir}")
endforeach()
if(cfgdir)
  file(MAKE_DIRECTORY "C:/Users/perry/Documents/GitHub/EasyDSP/device_firmware/build/bootloader-prefix/src/bootloader-stamp${cfgdir}") # cfgdir has leading slash
endif()
