#include "pico/stdlib.h"
#include "pico/cyw43_arch.h"
#include <stdio.h>
#include "hardware/timer.h"
#include "test_functionality.h"

#define ALARM_NUM 0
#define ALARM_IRQ TIMER_IRQ_0

static uint64_t get_time(void) {
    // Reading low latches the high value
    uint32_t lo = timer_hw->timelr;
    uint32_t hi = timer_hw->timehr;
    return ((uint64_t) hi << 32u) | lo;
}

void timerEchoTimeout()
{
	static bool ledOn = false;

	// Clear the alarm irq
	hw_clear_bits(&timer_hw->intr, 1u << ALARM_NUM);

	ledOn = !ledOn;
	cyw43_arch_gpio_put(CYW43_WL_GPIO_LED_PIN, (ledOn ? 1 : 0));
}

void timerEchoSetup(uint32_t delay_us)
{ 
	// Enable the interrupt for our alarm (the timer outputs 4 alarm irqs)
	hw_set_bits(&timer_hw->inte, 1u << ALARM_NUM);
	// Set irq handler for alarm irq
	irq_set_exclusive_handler(ALARM_IRQ, timerEchoTimeout);
	// Enable the alarm irq
	irq_set_enabled(ALARM_IRQ, true);
	// Enable interrupt in block and at processor

	// Alarm is only 32 bits so if trying to delay more
	// than that need to be careful and keep track of the upper
	// bits
	uint32_t target = timer_hw->timerawl + delay_us;

	// Write the lower 32 bits of the target time to the alarm which
	// will arm it
	// timer_hw->alarm[ALARM_NUM] = (uint32_t) target;
	hardware_alarm_set_target(ALARM_NUM, get_time() + delay_us);
}

void timerSetup1(uint32_t delay_us)
{
	//Enable/Disable a callback for a hardware timer on this core
	//void hardware_alarm_set_callback(uint alarm_num, hardware_alarm_callback_t callback);

}

int main() {
	// setup_default_uart();
	stdio_init_all();
	if (cyw43_arch_init()) {
		printf("Wi-Fi init failed");
		return -1;
	}
	// timerEchoSetup(10000);
	// cyw43_arch_gpio_put(CYW43_WL_GPIO_LED_PIN, 1);
	setAlarm();
	while(1)
	{
		sleep_ms(1000);
		cyw43_arch_gpio_put(CYW43_WL_GPIO_LED_PIN, 1);
		sleep_ms(1000);
		cyw43_arch_gpio_put(CYW43_WL_GPIO_LED_PIN, 0);
		// printf("%d\n", timer_hw->timerawl + 1000000);
	}
	return 0;
}
