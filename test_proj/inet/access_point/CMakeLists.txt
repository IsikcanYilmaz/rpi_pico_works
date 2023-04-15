# add_executable(picow_access_point_background
#         picow_access_point.c
#         dhcpserver/dhcpserver.c
#         dnsserver/dnsserver.c
#         )
#
# target_include_directories(picow_access_point_background PRIVATE
#         ${CMAKE_CURRENT_LIST_DIR}
#         ${CMAKE_CURRENT_LIST_DIR}/.. # for our common lwipopts
#         ${CMAKE_CURRENT_LIST_DIR}/dhcpserver
#         ${CMAKE_CURRENT_LIST_DIR}/dnsserver
#         )
#
# target_link_libraries(picow_access_point_background
#         pico_cyw43_arch_lwip_threadsafe_background
#         pico_stdlib
#         )
#
# pico_add_extra_outputs(picow_access_point_background)
# add_library(picow_access_point_poll)

# aux_source_directory(. picow_access_point_poll)
# add_library(picow_access_point_poll ${picow_access_point_poll}
#         picow_access_point.c
#         dhcpserver/dhcpserver.c
#         dnsserver/dnsserver.c
#         )
# target_include_directories(picow_access_point_poll PRIVATE
#         ${CMAKE_CURRENT_LIST_DIR}
#         ${CMAKE_CURRENT_LIST_DIR}/.. # for our common lwipopts
#         ${CMAKE_CURRENT_LIST_DIR}/dhcpserver
#         ${CMAKE_CURRENT_LIST_DIR}/dnsserver
#         )
# target_link_libraries(picow_access_point_poll PRIVATE
#         pico_cyw43_arch_lwip_poll
#         pico_stdlib
#         )

# pico_add_extra_outputs(picow_access_point_poll)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
aux_source_directory(. picow_access_point_background)
add_library(picow_access_point_background ${picow_access_point_background}
	picow_access_point.c
	dhcpserver/dhcpserver.c
	dnsserver/dnsserver.c)
target_include_directories(picow_access_point_background PRIVATE
        ${CMAKE_CURRENT_LIST_DIR}
        ${CMAKE_CURRENT_LIST_DIR}/.. # for our common lwipopts
        ${CMAKE_CURRENT_LIST_DIR}/dhcpserver
        ${CMAKE_CURRENT_LIST_DIR}/dnsserver
        )
target_link_libraries(picow_access_point_background PRIVATE
				pico_cyw43_arch_lwip_threadsafe_background
				# pico_cyw43_arch_lwip_poll
        pico_stdlib
        )

