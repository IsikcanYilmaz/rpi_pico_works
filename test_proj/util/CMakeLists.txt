aux_source_directory(. util)
add_library(util ${util})
target_include_directories(util PRIVATE
													${CMAKE_CURRENT_LIST_DIR}
													../
													# ../oled_lib/Config
													# ../oled_lib/GUI
													# ../oled_lib/OLED
													# ../oled_lib/Fonts
										)
target_link_libraries(misc PRIVATE
											# OLED
											# GUI
											# Fonts
											# Config
											)
