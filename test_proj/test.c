#include "pico/stdlib.h"
#include "pico/cyw43_arch.h"
#include <stdio.h>
#include "hardware/timer.h"
#include "test_functionality.h"
#include "usr_input.h"

volatile bool ledon = false;

int main() {
	// setup_default_uart();
	stdio_init_all();
	if (cyw43_arch_init()) {
		printf("Wi-Fi init failed");
		return -1;
	}
	// struct repeating_timer timer1, timer2, usrInputPollTimer;
	// add_repeating_timer_ms(1000, repeatingAlarmCallback, NULL, &timer1);
	// add_repeating_timer_ms(500, testSecondaryRepeatingAlarmCallback, NULL, &timer2);
	// add_repeating_timer_ms(10, usrInputPoll, NULL, &timer2);
	UserInput_Init();
	while(1)
	{
		UserInput_Service();
		// sleep_ms(1000);
		// toggleLed();
	}
	return 0;
}
