#include "button.h"
#include "hardware/gpio.h"
#include <stdlib.h>
#include <stdio.h>

ButtonContext_s buttonContext;

char *buttonEnumStrings[] = {
	[BUTTON_0] = "BUTTON_0", 
	[BUTTON_1] = "BUTTON_1", 
	[BUTTON_BOTH] = "BUTTON_BOTH", 
	[BUTTON_NONE] = "BUTTON_NONE"
};

static bool Button_TimerCallback(struct repeating_timer_t *t)
{
	Button_e currPressed = Button_GetPressedButtons();
	printf("BUTTON CALLBACK %s\n", buttonEnumStrings[currPressed]);
	if (currPressed != BUTTON_NONE)
	{
		return true;
	}
	buttonContext.timerRunning = false;
	return false;
}

static void Button_StartTimer(void)
{
	if (buttonContext.timerRunning) return;
	buttonContext.timerRunning = true;
	add_repeating_timer_ms(BUTTON_POLL_PERIOD_MS, Button_TimerCallback, NULL, &(buttonContext.buttonTimer));
}

static void Button_StopTimer(void)
{
	if (!buttonContext.timerRunning) return;
	cancel_repeating_timer(&(buttonContext.buttonTimer));
	buttonContext.timerRunning = false;
}

// Events v
// "LEVEL_LOW",  // 0x1
// "LEVEL_HIGH", // 0x2
// "EDGE_FALL",  // 0x4
// "EDGE_RISE"   // 0x8
static void Button_GpioCallback(uint gpio, uint32_t events) 
{
	printf("GPIO %d EVENTS 0x%x\n", gpio, events);
	Button_StartTimer();
}

void Button_Init(void)
{ 
	gpio_init(BUTTON_KEY_0_GPIO);
	gpio_init(BUTTON_KEY_1_GPIO);

	gpio_set_dir(BUTTON_KEY_0_GPIO, GPIO_IN);
	gpio_set_dir(BUTTON_KEY_1_GPIO, GPIO_IN);

	gpio_pull_down(BUTTON_KEY_0_GPIO);
	gpio_pull_down(BUTTON_KEY_1_GPIO);

	gpio_set_irq_enabled_with_callback(BUTTON_KEY_0_GPIO, 
																		GPIO_IRQ_EDGE_RISE | GPIO_IRQ_EDGE_FALL, 
																		true, 
																		Button_GpioCallback);
	gpio_set_irq_enabled_with_callback(BUTTON_KEY_1_GPIO, 
																		GPIO_IRQ_EDGE_RISE | GPIO_IRQ_EDGE_FALL, 
																		true, 
																		Button_GpioCallback);
	
	buttonContext.button0State = false;
	buttonContext.button1State = false;
	buttonContext.timerRunning = false;
}

bool Button_IsPressed(Button_e b)
{
	switch(b)
	{
		case BUTTON_0:
		{
			return (gpio_get(BUTTON_KEY_0_GPIO) == BUTTON_STATE_PRESSED);
		}
		case BUTTON_1:
		{
			return (gpio_get(BUTTON_KEY_1_GPIO) == BUTTON_STATE_PRESSED);
		}
		case BUTTON_BOTH:
		{
			return ((gpio_get(BUTTON_KEY_0_GPIO) == BUTTON_STATE_PRESSED) && (gpio_get(BUTTON_KEY_1_GPIO) == BUTTON_STATE_PRESSED));
		}
		default:
		{
			printf("Wrong argument to Button_IsPressed %d\n", b);
			return false;
		}
	}
}

Button_e Button_GetPressedButtons(void)
{
	bool b0 = (gpio_get(BUTTON_KEY_0_GPIO) == BUTTON_STATE_PRESSED);
	bool b1 = (gpio_get(BUTTON_KEY_1_GPIO) == BUTTON_STATE_PRESSED);
	if (b0 && !b1) return BUTTON_0;
	if (b1 && !b0) return BUTTON_1;
	if (b0 && b1) return BUTTON_BOTH;
	return BUTTON_NONE;
}
