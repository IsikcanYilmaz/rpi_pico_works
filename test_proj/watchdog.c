#include "hardware/watchdog.h"
#include "watchdog.h"
#include <stdio.h>
#include <stdlib.h>

WatchdogContext_s watchdogContext;

static bool Watchdog_TimerCallback(struct repeating_timer_t *t)
{
	watchdog_update();
	// printf("Kicked watchdog\n");
}

void Watchdog_Init(void)
{
#if WATCHDOG_ENABLED
	// Check if we want to enable the watchdog
	// TODO for now, this is static. Get this from flash later
	watchdogContext.watchdogEnabled = true;

	if (watchdogContext.watchdogEnabled)
	{
		watchdog_enable(WATCHDOG_INTERVAL_MS, 1);
		add_repeating_timer_ms(WATCHDOG_KICK_INTERVAL_MS, (repeating_timer_callback_t) Watchdog_TimerCallback, NULL, &(watchdogContext.watchdogTimer));
		printf("Watchdog enabled\n");
	}
#endif
}

bool Watchdog_CheckReset(void)
{
	if (watchdog_caused_reboot())
	{
		printf("Reset caused by watchdog!\n");
		// TODO retrieve debug info about whatever caused the reset 
		return true;
	}
	return false;
}

