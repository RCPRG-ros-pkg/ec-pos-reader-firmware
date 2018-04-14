# CMakeLists.txt
# Specifies main binary and other microcontroller-specific targets
# Author: akowalew

# configuration: sources, flags, standard
set(CMAKE_CXX_FLAGS_DEBUG "-g -Os")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} \
-std=c++17 \
-Wall \
-Werror \
-Wextra \
-ffunction-sections \
-fdata-sections \
-fno-rtti \
-fno-exceptions \
-fno-unwind-tables \
-fno-asynchronous-unwind-tables \
-fno-threadsafe-statics \
--specs=nano.specs \
")

set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} \
-std=c11 \
-Wall \
-Werror \
-Wextra \
-ffunction-sections \
-fdata-sections \
-fno-unwind-tables \
-fno-asynchronous-unwind-tables \
--specs=nano.specs \
")

set(LD_SCRIPT "${CMAKE_CURRENT_SOURCE_DIR}/tm4c123gh6pm.ld")
set(CMAKE_EXE_LINKER_FLAGS "\
-Wl,--gc-sections \
-Wl,-Map=main.map \
-Wl,-T ${LD_SCRIPT} \
")

add_subdirectory(app)
add_subdirectory(component)
add_subdirectory(device)
add_subdirectory(util)

add_executable(${PROJECT_NAME}
    init.cpp
    main.cpp
    newlib-stubs.cpp
    other-stubs.cpp
    startup.cpp
)

target_link_libraries(${PROJECT_NAME}
    tivaware_driverlib
    tivaware_utils

    app
    util_driverlib
)

# Add dependency to the program of linker script during linking stage
set_target_properties(${PROJECT_NAME} PROPERTIES
    LINK_DEPENDS ${LD_SCRIPT}
)

# add 'bin' target - prepary binary image of program
add_custom_target(bin
    COMMAND /opt/embedded/bleeding-edge-toolchain/installNative/bin/arm-none-eabi-objcopy
        --output-target=binary      # binary format direct to MCU
        ${PROJECT_NAME}      # source ELF file
        ${PROJECT_NAME}.bin  # target BIN file
    DEPENDS ${PROJECT_NAME})

# add 'flash' target - write prepared binary image to the MCU
add_custom_target(flash
    COMMAND lm4flash ${PROJECT_NAME}.bin
    DEPENDS bin)

# add 'objdump' target - makes objdump from compiled program without code snippets
add_custom_target(objdump
    COMMAND /opt/embedded/bleeding-edge-toolchain/installNative/bin/arm-none-eabi-objdump -d -C ${PROJECT_NAME} >objdump
    DEPENDS ${PROJECT_NAME})

# add 'objdump-full' target - makes objdump, but with full code listing
add_custom_target(objdump-full
    COMMAND /opt/embedded/bleeding-edge-toolchain/installNative/bin/arm-none-eabi-objdump -D -r -t -x -S -C ${PROJECT_NAME} >objdump
    DEPENDS ${PROJECT_NAME})

# add 'debug' target - write program and start debug session
add_custom_target(debug
    COMMAND /usr/bin/arm-none-eabi-gdb ${PROJECT_NAME}
        -x ${CMAKE_SOURCE_DIR}/.gdbinit
    DEPENDS flash)
