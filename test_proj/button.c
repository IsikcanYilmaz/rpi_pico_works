#include "button.h"
#include "hardware/gpio.h"
#include <stdlib.h>
#include <stdio.h>

void Button_GpioCallback(uint gpio, uint32_t events) 
{
	printf("GPIO %d EVENTS 0x%x\n", gpio, events);
}

void Button_Init(void)
{ 
	gpio_init(BUTTON_KEY_0_GPIO);
	gpio_init(BUTTON_KEY_1_GPIO);

	gpio_set_dir(BUTTON_KEY_0_GPIO, GPIO_IN);
	gpio_set_dir(BUTTON_KEY_1_GPIO, GPIO_IN);

	gpio_set_irq_enabled_with_callback(BUTTON_KEY_0_GPIO, 
																		GPIO_IRQ_EDGE_RISE | GPIO_IRQ_EDGE_FALL, 
																		true, 
																		Button_GpioCallback);
	gpio_set_irq_enabled_with_callback(BUTTON_KEY_1_GPIO, 
																		GPIO_IRQ_EDGE_RISE | GPIO_IRQ_EDGE_FALL, 
																		true, 
																		Button_GpioCallback);
}
