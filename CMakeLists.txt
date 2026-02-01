# == DO NOT EDIT THE FOLLOWING LINES for the Raspberry Pi Pico VS Code Extension to work ==
if(WIN32)
    set(USERHOME $ENV{USERPROFILE})
else()
    set(USERHOME $ENV{HOME})
endif()
set(sdkVersion 2.2.0)
set(toolchainVersion 14_2_Rel1)
set(picotoolVersion 2.2.0-a4)
set(picoVscode ${USERHOME}/.pico-sdk/cmake/pico-vscode.cmake)
if (EXISTS ${picoVscode})
    include(${picoVscode})
endif()
# ====================================================================================
set(PICO_BOARD pico CACHE STRING "Board type")

cmake_minimum_required(VERSION 3.13)

include($ENV{PICO_SDK_PATH}/external/pico_sdk_import.cmake)

project(pico_lcd_demo C CXX ASM)
set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 17)

pico_sdk_init()

add_executable(lcd_demo
    src/main.c
    src/drivers/lcd_pcf8574.c
    src/drivers/led.c
    src/drivers/dht20.c
    src/ui/ui.c
    src/tools/command/command_interface.c
)

target_link_libraries(lcd_demo pico_stdlib hardware_i2c)

pico_enable_stdio_usb(lcd_demo 1)
pico_enable_stdio_uart(lcd_demo 0)

pico_add_extra_outputs(lcd_demo)
