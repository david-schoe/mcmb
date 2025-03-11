# This must be included

cmake_minimum_required(VERSION 3.16)
include($ENV{IDF_PATH}/tools/cmake/project.cmake)
set(EXTRA_COMPONENT_DIRS $ENV{IDF_PATH}/../lwmb $ENV{IDF_PATH}/../txrx/components)
project(txrx)
