add_executable(blinky.elf)

target_compile_options(blinky.elf
PRIVATE 
    -Wall 
    -Wextra
)

target_compile_definitions(blinky.elf
PUBLIC 
    -D${CMAKE_SYSTEM_PROCESSOR}_XXAA_APPLICATION
)

target_link_options(blinky.elf
PRIVATE 
    -T ${CMAKE_CURRENT_LIST_DIR}/linkerscript.ld 
    -Wl,--gc-sections -Wl,-Map,blinky.map
)

target_include_directories(blinky.elf
PRIVATE
    include/
    ../3rd-party/CMSIS_5/CMSIS/Core/Include
    ../3rd-party/nrfx/
    ../3rd-party/nrfx/hal/
    ../3rd-party/nrfx/mdk/
    ../3rd-party/nrfx/templates/
)

target_sources(blinky.elf
PRIVATE
    startup_gcc.c
    source/platform_cortexm33.cpp
    source/main.cpp
    source/nrf/digitalio.cpp
    source/nrf/interrupt.cpp
    source/nrf/timer.cpp
)

target_link_libraries(blinky.elf 
    Flow
    driver
)

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