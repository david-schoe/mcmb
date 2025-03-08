# Distributed under the OSI-approved BSD 3-Clause License.  See accompanying
# file Copyright.txt or https://cmake.org/licensing for details.

cmake_minimum_required(VERSION 3.5)

file(MAKE_DIRECTORY
  "/home/davey_schoe/esp/esp-idf/components/bootloader/subproject"
  "/home/davey_schoe/esp/mcmb_tx_dv3/build/bootloader"
  "/home/davey_schoe/esp/mcmb_tx_dv3/build/bootloader-prefix"
  "/home/davey_schoe/esp/mcmb_tx_dv3/build/bootloader-prefix/tmp"
  "/home/davey_schoe/esp/mcmb_tx_dv3/build/bootloader-prefix/src/bootloader-stamp"
  "/home/davey_schoe/esp/mcmb_tx_dv3/build/bootloader-prefix/src"
  "/home/davey_schoe/esp/mcmb_tx_dv3/build/bootloader-prefix/src/bootloader-stamp"
)

set(configSubDirs )
foreach(subDir IN LISTS configSubDirs)
    file(MAKE_DIRECTORY "/home/davey_schoe/esp/mcmb_tx_dv3/build/bootloader-prefix/src/bootloader-stamp/${subDir}")
endforeach()
if(cfgdir)
  file(MAKE_DIRECTORY "/home/davey_schoe/esp/mcmb_tx_dv3/build/bootloader-prefix/src/bootloader-stamp${cfgdir}") # cfgdir has leading slash
endif()
