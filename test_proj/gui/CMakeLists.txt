aux_source_directory(. gui)
add_library(gui ${gui})
target_include_directories(gui PRIVATE
													${CMAKE_CURRENT_LIST_DIR}
													../
													../oled_lib/Config
													# ../oled_lib/GUI
													../oled_lib/OLED
													../oled_lib/Fonts
													../util/
													../misc/
										)
target_link_libraries(gui PRIVATE
											pico_stdlib
											pico_cyw43_arch_lwip_poll
											hardware_irq
											hardware_pwm
											hardware_spi
											OLED
											# GUI
											Fonts
											Config
											# 
											misc
											)
