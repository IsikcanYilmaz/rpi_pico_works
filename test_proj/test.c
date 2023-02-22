#include "pico/stdlib.h"
#include "pico/cyw43_arch.h"
#include "pico/stdlib.h"
#include <stdio.h>

int main() {
	// setup_default_uart();
	stdio_init_all();
	if (cyw43_arch_init()) {
		printf("Wi-Fi init failed");
		return -1;
	}
	uint16_t delay = 10;
	uint16_t lowDelay = 10;
	uint16_t highDelay = 1000;
	uint16_t delayIncrement = 10;
	bool up = true;
	while(1)
	{
		printf("Hello, world!\n");
		cyw43_arch_gpio_put(CYW43_WL_GPIO_LED_PIN, 1);
		sleep_ms(delay);
		cyw43_arch_gpio_put(CYW43_WL_GPIO_LED_PIN, 0);
		sleep_ms(delay);
		delay += (up ? delayIncrement : -delayIncrement);
		if (delay >= highDelay || delay <= lowDelay)
		{
			up = !up;
		}
	}
	return 0;
}
