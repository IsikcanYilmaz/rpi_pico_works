#include "pico/stdlib.h"
#include "pico/cyw43_arch.h"
#include <stdio.h>
#include "hardware/timer.h"
#include "test_functionality.h"
#include "usr_input.h"
#include "wifi.h"
#include "oled_manager.h"
#include "game_of_life.h"
#include "screen_saver.h"

// OLED v
#include "DEV_Config.h"

int main() {
	// setup_default_uart();
	stdio_init_all();
	if (cyw43_arch_init()) {
		printf("Wi-Fi init failed");
		return -1;
	}
	UserInput_Init();
	Wifi_Init();
	OledMan_Init();

	Gol_Init();
	// ScreenSaver_Init("TEST");

	// ScreenSaver_Start();

	toggleLed();
	while(1)
	{
		UserInput_Service();
	}
	return 0;
}
