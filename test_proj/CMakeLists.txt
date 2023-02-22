cmake_minimum_required(VERSION 3.13)

# initialize the SDK based on PICO_SDK_PATH
# note: this must happen before project()
include(pico_sdk_import.cmake)

project(test_proj)

# initialize the Raspberry Pi Pico SDK
pico_sdk_init()

# rest of your project
add_executable(test_proj
	test.c
	)

# Add pico_stdlib library which aggregates commonly used features
target_link_libraries(test_proj 
											pico_stdlib
											pico_cyw43_arch_none
											)

# enable usb output, disable uart output
pico_enable_stdio_usb(test_proj 1)
pico_enable_stdio_uart(test_proj 0)

# add url via pico_set_program_url
# example_auto_set_url(test_proj)

# create map/bin/hex/uf2 file in addition to ELF.
pico_add_extra_outputs(test_proj)
