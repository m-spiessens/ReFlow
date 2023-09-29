set(CMAKE_SYSTEM_NAME Generic)
set(CMAKE_SYSTEM_PROCESSOR TM4C129ENCPDT)

set(CMAKE_C_COMPILER arm-none-eabi-gcc)
set(CMAKE_C_FLAGS_INIT "-mcpu=cortex-m4 -ffunction-sections -fdata-sections ")

set(CMAKE_CXX_COMPILER arm-none-eabi-g++)
set(CMAKE_CXX_FLAGS_INIT "${CMAKE_C_FLAGS_INIT} -fno-rtti -fno-exceptions -fno-unwind-tables")

set(CMAKE_EXE_LINKER_FLAGS_INIT "--specs=nosys.specs --specs=nano.specs -ffunction-sections -fdata-sections ")

set(CMAKE_C_FLAGS_DEBUG "-Og -g3")
set(CMAKE_C_FLAGS_MINSIZEREL "-Os -DNDEBUG")
set(CMAKE_C_FLAGS_RELEASE "-O2 -DNDEBUG")
set(CMAKE_C_FLAGS_RELWITHDEBUGINFO "-O2 -g -DNDEBUG")

set(CMAKE_CXX_FLAGS_DEBUG "-Og -g3")
set(CMAKE_CXX_FLAGS_MINSIZEREL "-Os -DNDEBUG")
set(CMAKE_CXX_FLAGS_RELEASE "-O2 -DNDEBUG")
set(CMAKE_CXX_FLAGS_RELWITHDEBUGINFO "-O2 -g -DNDEBUG")
