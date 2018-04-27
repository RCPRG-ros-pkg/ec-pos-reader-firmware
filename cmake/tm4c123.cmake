# tm4c123.cmake
# Toolchain file for TM4C123 series microcontrollers
# Author: akowalew

set(CMAKE_CROSSCOMPILE ON CACHE BOOL "Cross-compiling" FORCE)
set(CMAKE_SYSTEM_NAME Generic)
set(CMAKE_SYSTEM_PROCESSOR arm)

set(MCU TM4C123GH6PM)

# prevent from running linker during compiler test
set(CMAKE_TRY_COMPILE_TARGET_TYPE STATIC_LIBRARY CACHE STRING "" FORCE)

# specify the cross compiler
set(CMAKE_C_COMPILER /opt/embedded/gcc-arm-none-eabi-7-2017-q4-major/bin/arm-none-eabi-gcc)
set(CMAKE_CXX_COMPILER /opt/embedded/gcc-arm-none-eabi-7-2017-q4-major/bin/arm-none-eabi-g++)

# Flags directed into TM4C123x processors
set(CMAKE_C_FLAGS "\
-march=armv7e-m \
-mcpu=cortex-m4 \
-mtune=cortex-m4 \
-mfpu=fpv4-sp-d16 \
-mthumb \
-mfloat-abi=hard \
-munaligned-access \
-DPART_${MCU} \
-DTARGET_IS_TM4C123_RB1"
CACHE STRING "" FORCE)

set(CMAKE_CXX_FLAGS "\
-march=armv7e-m \
-mcpu=cortex-m4 \
-mtune=cortex-m4 \
-mfpu=fpv4-sp-d16 \
-mthumb \
-mfloat-abi=hard \
-munaligned-access \
-DPART_${MCU} \
-DTARGET_IS_TM4C123_RB1"
CACHE STRING "" FORCE)

# search for programs in the build host directories
SET(CMAKE_FIND_ROOT_PATH_MODE_PROGRAM NEVER)
# for libraries and headers in the target directories
SET(CMAKE_FIND_ROOT_PATH_MODE_LIBRARY ONLY)
SET(CMAKE_FIND_ROOT_PATH_MODE_INCLUDE ONLY)
