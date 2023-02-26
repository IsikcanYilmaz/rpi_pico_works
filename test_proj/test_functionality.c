#include "test_functionality.h"
// #include "pico/stdlib.h"
#include "pico/cyw43_arch.h"
#include "hardware/timer.h"

void flasher() 
{
	uint16_t delay = 10;
	uint16_t lowDelay = 10;
	uint16_t highDelay = 1000;
	uint16_t delayIncrement = 10;
	bool up = true;
	while(up)
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
}

void echo()
{
	char buf[128];
	uint16_t idx = 0;
	bool ledOn = true;
	while(1)
	{
		buf[idx] = getchar();
		printf("%c", buf[idx]);
		if (buf[idx] == '\n')
		{
			printf("n %s\n", buf);
			memset(&buf, 0x00, sizeof(buf));
			idx = 0;
		}
		else if (buf[idx] == '\r')
		{
			printf("r %s\n", buf);
			memset(&buf, 0x00, sizeof(buf));
			idx = 0;
		}
		else if (buf[idx] == '\b')
		{
			printf("b %s\n", buf);
			memset(&buf, 0x00, sizeof(buf));
			idx = 0;
		}
		else
		{
			idx++;
		}
		ledOn = !ledOn;
		cyw43_arch_gpio_put(CYW43_WL_GPIO_LED_PIN, (ledOn ? 1 : 0));
	}
}

// TIMER STUFF

int64_t alarmCallback(alarm_id_t id, void *user_data)
{
	printf("alarm callback %d\n", id);
}

void setAlarm()
{
	struct repeating_timer_t timer;
	add_alarm_in_ms(2000, alarmCallback, NULL, &timer);
}
