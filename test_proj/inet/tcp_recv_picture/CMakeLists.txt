set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
aux_source_directory(. tcp_recv_picture)
add_library(tcp_recv_picture ${tcp_recv_picture}
	tcp_recv_picture.c)
target_include_directories(tcp_recv_picture PRIVATE
        ${CMAKE_CURRENT_LIST_DIR}
        ${CMAKE_CURRENT_LIST_DIR}/.. # for our common lwipopts
        )
target_link_libraries(tcp_recv_picture PRIVATE
				pico_cyw43_arch_lwip_threadsafe_background
				# pico_cyw43_arch_lwip_poll
        pico_stdlib
        )

