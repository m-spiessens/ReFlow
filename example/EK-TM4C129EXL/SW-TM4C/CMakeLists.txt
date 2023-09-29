add_library(SW-TM4C)

target_compile_options(SW-TM4C
PRIVATE 
    -w
)

target_compile_definitions(SW-TM4C
PUBLIC 
    -DPART_${CMAKE_SYSTEM_PROCESSOR}
    -Dgcc
)

target_include_directories(SW-TM4C
PUBLIC
    ./
)

target_sources(SW-TM4C
PRIVATE
    driverlib/cpu.c
    driverlib/fpu.c
    driverlib/gpio.c
    driverlib/interrupt.c
    driverlib/ssi.c
    driverlib/sysctl.c
    driverlib/systick.c
    driverlib/timer.c
    driverlib/udma.c
)
