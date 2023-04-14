aux_source_directory(. misc)
add_library(misc ${misc})
target_include_directories(misc PRIVATE
													${CMAKE_CURRENT_LIST_DIR}
													../
													../oled_lib/Config
													# ../oled_lib/GUI
													../oled_lib/OLED
													../oled_lib/Fonts
													../util/
													../gui/
										)
target_link_libraries(misc PRIVATE
											pico_stdlib
											# pico_cyw43_arch_lwip_poll
											pico_cyw43_arch_lwip_threadsafe_background
											hardware_irq
											hardware_pwm
											hardware_spi
											OLED
											# GUI
											Fonts
											Config
											)
