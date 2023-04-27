set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
aux_source_directory(. misc)
add_library(misc ${misc})
target_sources(misc PRIVATE
							game_of_life/game_of_life.c
							picture/picture.c
							picture/picture_buffer.c
							screen_saver/screen_saver.c
							tamagotchi/tamagotchi.c
							wifi_scan/wifi_scan.c
							)
target_include_directories(misc PRIVATE
													${CMAKE_CURRENT_LIST_DIR}
													../
													../oled_lib/Config
													# ../oled_lib/GUI
													../oled_lib/OLED
													../oled_lib/Fonts
													../util/
													../gui/
													../inet/
													game_of_life/
													picture/
													wifi_scan/
													screen_saver/
													tamagotchi/
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

