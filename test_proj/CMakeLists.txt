cmake_minimum_required(VERSION 3.13)
set(PICO_BOARD pico_w)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# initialize the SDK based on PICO_SDK_PATH
# note: this must happen before project()
include(pico_sdk_import.cmake)

project(test_proj)

# initialize the Raspberry Pi Pico SDK
pico_sdk_init()

# subdirectories
add_subdirectory(oled_lib/Config)    
add_subdirectory(oled_lib/Fonts)    
# add_subdirectory(oled_lib/GUI)    
add_subdirectory(oled_lib/OLED)
add_subdirectory(misc/)
add_subdirectory(gui/)
add_subdirectory(util/)
add_subdirectory(inet/)

# rest of your project
add_executable(test_proj
	test.c 
	test_functionality.c
	usr_input.c 
	usr_commands.c 
	wifi.c
	oled_manager.c 
	button.c 
	paint.c
	)

target_include_directories(test_proj PRIVATE
													 ${CMAKE_CURRENT_LIST_DIR}
													 ./oled_lib/Config
													 # ./oled_lib/GUI
													 ./oled_lib/OLED
													 ./oled_lib/Fonts
													 ./misc/
													 ./gui/
													 ./util/
													 ./inet/
													)

# Add pico_stdlib library which aggregates commonly used features
target_link_libraries(test_proj 
											pico_stdlib
											# pico_cyw43_arch_none
											# pico_cyw43_arch_lwip_poll
											pico_cyw43_arch_lwip_threadsafe_background
											hardware_irq
											hardware_pwm
											hardware_spi
											# misc
											misc
											util
											gui
											picow_access_point_background
											# Below for the oled driver
											OLED
											# GUI
											Fonts
											Config
											)


# add_compile_definitions(PICO_TIME_DEFAULT_ALARM_POOL_DISABLED=1)
# add_compile_definitions(PICO_STDIO_USB_SUPPORT_CHARS_AVAILABLE_CALLBACK=1)
add_compile_definitions(LWIP_DEBUG=0)
add_compile_definitions(NDEBUG) # Set to disable lwip debug
add_compile_definitions(WIFI_BACKGROUND=1)
add_compile_definitions(WIFI_POLL=0)

# enable usb output, disable uart output
pico_enable_stdio_usb(test_proj 1)
pico_enable_stdio_uart(test_proj 0)

# add url via pico_set_program_url
# example_auto_set_url(test_proj)

# create map/bin/hex/uf2 file in addition to ELF.
pico_add_extra_outputs(test_proj)
