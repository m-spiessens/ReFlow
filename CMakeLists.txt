cmake_minimum_required(VERSION 3.16)

project(Flow LANGUAGES C CXX)

include(CTest)

set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED True)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE "Debug")
endif()

add_library(Flow)
target_include_directories(Flow
PUBLIC 
    include/
)

target_sources(Flow
PRIVATE
    source/flow/components.cpp
    source/flow/flow.cpp
    source/flow/reactor.cpp
)

add_library(driver INTERFACE)

target_include_directories(driver
INTERFACE
    ./
)

if(${CMAKE_SYSTEM_NAME} STREQUAL Linux)
    add_subdirectory(test/)
endif()

if(${CMAKE_SYSTEM_PROCESSOR} STREQUAL TM4C129ENCPDT)
    add_subdirectory(example/EK-TM4C129EXL)
endif()

if(${CMAKE_SYSTEM_PROCESSOR} STREQUAL NRF5340)
    add_subdirectory(example/nRF5340-DK)
endif()