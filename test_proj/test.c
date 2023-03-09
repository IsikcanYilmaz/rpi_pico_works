#include "pico/stdlib.h"
#include "pico/cyw43_arch.h"
#include <stdio.h>
#include "hardware/timer.h"
#include "test_functionality.h"
#include "usr_input.h"

int main() {
	// setup_default_uart();
	stdio_init_all();
	if (cyw43_arch_init()) {
		printf("Wi-Fi init failed");
		return -1;
	}
	UserInput_Init();
	Wifi_Init();
	while(1)
	{
		UserInput_Service();
	}
	return 0;
}
