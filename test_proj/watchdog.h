#ifndef __WATCHDOG_H
#define __WATCHDOG_H

#include <stdbool.h>
#include "pico/stdlib.h"
#include "hardware/timer.h"

#define WATCHDOG_INTERVAL_MS 5000
#define WATCHDOG_KICK_INTERVAL_MS 2000
#define WATCHDOG_ENABLED 1

typedef struct WatchdogContext_s_
{
	struct repeating_timer watchdogTimer;
	bool watchdogEnabled;
} WatchdogContext_s;

void Watchdog_Init(void);

#endif
