cmake_minimum_required(VERSION 3.16)

set(CMAKE_TRY_COMPILE_TARGET_TYPE "STATIC_LIBRARY")

project(FlowBlinky)

include("${CMAKE_BINARY_DIR}/conanbuildinfo.cmake")
conan_basic_setup(TARGETS)

set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED True)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED True)

add_compile_options(-fno-rtti -fno-exceptions -fno-unwind-tables -ffunction-sections -fdata-sections -Wall -Wextra)

add_definitions(-D${DEFINE_PART} -Dgcc)

add_link_options(--specs=nosys.specs -T ${PROJECT_SOURCE_DIR}/linkerscript.ld -Wl,--gc-sections -Wl,-Map,blinky.map)

add_executable(blinky.elf "")

target_include_directories(blinky.elf
    PRIVATE
        include/
        ../../
        SW-TM4C/
)

target_sources(blinky.elf
    PRIVATE
        startup_gcc.c
        ${PROJECT_BINARY_DIR}/source/flow/platform_cortexm4.cpp
        source/main.cpp
        source/pinmux/pinout.c
        source/tm4c/clock.cpp
        source/tm4c/interrupt.cpp
        source/tm4c/digitalio.cpp
        source/tm4c/timer.cpp
        SW-TM4C/driverlib/cpu.c
        SW-TM4C/driverlib/fpu.c
        SW-TM4C/driverlib/gpio.c
        SW-TM4C/driverlib/interrupt.c
        SW-TM4C/driverlib/sysctl.c
        SW-TM4C/driverlib/systick.c
        SW-TM4C/driverlib/timer.c
)

target_link_libraries(blinky.elf 
    CONAN_PKG::Flow
)

configure_file(FlowBlinky-Debug_EK-TM4C129EXL.launch FlowBlinky-Debug_EK-TM4C129EXL.launch COPYONLY)
