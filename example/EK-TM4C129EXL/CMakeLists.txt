
add_executable(blinky.elf)

target_compile_options(blinky.elf 
PRIVATE 
    -Wall 
    -Wextra
)

target_link_options(blinky.elf
PRIVATE 
    -T ${CMAKE_CURRENT_LIST_DIR}/linkerscript.ld 
    -Wl,--gc-sections -Wl,-Map,blinky.map
)

target_include_directories(blinky.elf
PRIVATE
    include/
    SW-TM4C/
)

target_sources(blinky.elf
PRIVATE
    startup_gcc.c
    source/platform_cortexm4.cpp
    source/main.cpp
    source/pinmux/pinout.c
    source/tm4c/clock.cpp
    source/tm4c/digitalio.cpp
    source/tm4c/dma.cpp
    source/tm4c/interrupt.cpp
    source/tm4c/ssi.cpp
    source/tm4c/timer.cpp
)

target_link_libraries(blinky.elf 
    Flow
    driver
    SW-TM4C
)

add_subdirectory(SW-TM4C)

add_custom_command(TARGET blinky.elf POST_BUILD
    COMMAND arm-none-eabi-size --format=gnu blinky.elf
    WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
    COMMENT "Firmware size:"
)

add_custom_command(TARGET blinky.elf POST_BUILD
    COMMAND arm-none-eabi-objcopy -O ihex blinky.elf blinky.hex
    WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
    COMMENT "Converting to HEX..."
)