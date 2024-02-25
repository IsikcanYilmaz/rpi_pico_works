#include "pico/stdlib.h"
#include "pico/cyw43_arch.h"
#include <stdio.h>
#include "hardware/timer.h"
#include "test_functionality.h"
#include "usr_input.h"
#include "wifi.h"
#include "oled_manager.h"
#include "gui_manager.h"
#include "button.h"
#include "watchdog.h"

#include "access_point/picow_access_point.h"

// OLED v
#include "DEV_Config.h"

static void Main_Init(void)
{
	bool watchdogReset = Watchdog_CheckReset(); // TODO do something with this 

	UserInput_Init();
	printf("UserInput Init\n");

	OledMan_Init();
	printf("OledMan Init\n");

	Button_Init();
	printf("Button Init\n");

	GuiMan_Init();
	printf("GuiMan Init\n");

	Wifi_Init();
	printf("Wifi Init\n");

	Watchdog_Init();
	printf("Watchdog Init\n");

	printf("Init complete\n");
}

int main() {
	stdio_init_all();
	Main_Init();
	// PicowAp_Init();
	toggleLed();
	while(1)
	{
		UserInput_Service();
		Wifi_Poll(NULL);
	}
	return 0;
}
