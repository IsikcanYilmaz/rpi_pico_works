cmake_minimum_required(VERSION 3.13)

# initialize the SDK based on PICO_SDK_PATH
# note: this must happen before project()
include(pico_sdk_import.cmake)

project(test_proj)

# initialize the Raspberry Pi Pico SDK
pico_sdk_init()

# subdirectories
add_subdirectory(oled_lib/Config)    
add_subdirectory(oled_lib/Fonts)    
add_subdirectory(oled_lib/GUI)    
add_subdirectory(oled_lib/OLED)

# rest of your project
add_executable(test_proj
	test.c test_functionality.c
	usr_input.c usr_commands.c wifi.c
	oled_manager.c
	)

target_include_directories(test_proj PRIVATE
													 ${CMAKE_CURRENT_LIST_DIR}
													 ./oled_lib/Config
													 ./oled_lib/GUI
													 ./oled_lib/OLED
													 ./oled_lib/Fonts
													)

# Add pico_stdlib library which aggregates commonly used features
target_link_libraries(test_proj 
											pico_stdlib
											# pico_cyw43_arch_none
											pico_cyw43_arch_lwip_poll
											hardware_irq
											hardware_pwm
											hardware_spi
											# Below for the oled driver
											OLED
											GUI
											Fonts
											Config
											)


set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(PICO_TIME_DEFAULT_ALARM_POOL_DISABLED 1)
set(PICO_STDIO_USB_SUPPORT_CHARS_AVAILABLE_CALLBACK 1)

# enable usb output, disable uart output
pico_enable_stdio_usb(test_proj 1)
pico_enable_stdio_uart(test_proj 0)

# add url via pico_set_program_url
# example_auto_set_url(test_proj)

# create map/bin/hex/uf2 file in addition to ELF.
pico_add_extra_outputs(test_proj)
