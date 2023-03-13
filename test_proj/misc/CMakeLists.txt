aux_source_directory(. misc)
add_library(misc ${misc})
target_include_directories(misc PRIVATE
													${CMAKE_CURRENT_LIST_DIR}
													../
													../oled_lib/Config
													../oled_lib/GUI
													../oled_lib/OLED
													../oled_lib/Fonts
										)
target_link_libraries(misc PRIVATE
											OLED
											GUI
											Fonts
											Config
											)
