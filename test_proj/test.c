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
#include "button.h"

// OLED v
#include "DEV_Config.h"

static void Main_Init(void)
{
	UserInput_Init();
	Wifi_Init();
	OledMan_Init();
	Button_Init();

	Misc_StartProgram(MISC_GAME_OF_LIFE, NULL);
}

int main() {
	// setup_default_uart();
	stdio_init_all();
	if (cyw43_arch_init()) {
		printf("Wi-Fi init failed");
		return -1;
	}
	Main_Init();
	toggleLed();
	while(1)
	{
		UserInput_Service();
	}
	return 0;
}
