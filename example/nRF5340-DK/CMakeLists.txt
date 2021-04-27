cmake_minimum_required(VERSION 3.16)

set(CMAKE_TRY_COMPILE_TARGET_TYPE "STATIC_LIBRARY")

project(FlowBlinky)

include("${CMAKE_BINARY_DIR}/conanbuildinfo.cmake")
conan_basic_setup(TARGETS)

set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED True)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED True)

macro(remove_cxx_flag flag)
string(REPLACE "${flag}" "" CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE}")
endmacro()

remove_cxx_flag("-O3")
set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -O2")

add_compile_options(-fno-rtti -fno-exceptions -fno-unwind-tables -ffunction-sections -fdata-sections -Wall -Wextra)

add_definitions(-D${DEFINE_PART} -Dgcc)

add_link_options(--specs=nosys.specs -T ${PROJECT_SOURCE_DIR}/linkerscript.ld -Wl,--gc-sections -Wl,-Map,blinky.map)

add_executable(blinky.elf "")

target_include_directories(blinky.elf
    PRIVATE
        include/
        ../../
)

target_sources(blinky.elf
    PRIVATE
        startup_gcc.c
        ${PROJECT_BINARY_DIR}/source/flow/platform_cortexm4.cpp
        source/main.cpp
        source/nrf/digitalio.cpp
        source/nrf/interrupt.cpp
        source/nrf/timer.cpp
)

target_link_libraries(blinky.elf 
    CONAN_PKG::Flow
    CONAN_PKG::nrfx
)

add_custom_command(TARGET blinky.elf POST_BUILD
    COMMAND arm-none-eabi-size --format=gnu blinky.elf
    WORKING_DIRECTORY ${CMAKE_BINARY_DIR}/bin
    COMMENT "Firmware size:"
)

add_custom_command(TARGET blinky.elf POST_BUILD
    COMMAND arm-none-eabi-objcopy -O ihex blinky.elf blinky.hex
    WORKING_DIRECTORY ${CMAKE_BINARY_DIR}/bin
    COMMENT "Converting to HEX..."
)