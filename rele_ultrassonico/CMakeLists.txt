# Generated Cmake Pico project file

cmake_minimum_required(VERSION 3.13)

set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# Initialise pico_sdk from installed location
# (note this can come from environment, CMake cache etc)

# == DO NOT EDIT THE FOLLOWING LINES for the Raspberry Pi Pico VS Code Extension to work ==
if(WIN32)
    set(USERHOME $ENV{USERPROFILE})
else()
    set(USERHOME $ENV{HOME})
endif()
set(sdkVersion 2.1.1)
set(toolchainVersion 14_2_Rel1)
set(picotoolVersion 2.1.1)
set(picoVscode ${USERHOME}/.pico-sdk/cmake/pico-vscode.cmake)
if (EXISTS ${picoVscode})
    include(${picoVscode})
endif()
# ====================================================================================
set(PICO_BOARD pico_w CACHE STRING "Board type")

# Pull in Raspberry Pi Pico SDK (must be before project)
include(pico_sdk_import.cmake)

project(rele_ultrassonico C CXX ASM)

# Initialise the Raspberry Pi Pico SDK
pico_sdk_init()

# Add executable. Default name is the project name, version 0.1

add_executable(rele_ultrassonico rele_ultrassonico.c )

pico_set_program_name(rele_ultrassonico "rele_ultrassonico")
pico_set_program_version(rele_ultrassonico "0.1")

# Modify the below lines to enable/disable output over UART/USB
pico_enable_stdio_uart(rele_ultrassonico 0)
pico_enable_stdio_usb(rele_ultrassonico 1)

# Add the standard library to the build
target_link_libraries(rele_ultrassonico
        pico_stdlib)

# Add the standard include files to the build
target_include_directories(rele_ultrassonico PRIVATE
        ${CMAKE_CURRENT_LIST_DIR}
)

# Add any user requested libraries
target_link_libraries(rele_ultrassonico 
        
        )

pico_add_extra_outputs(rele_ultrassonico)

