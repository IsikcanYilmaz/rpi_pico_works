#ifndef __WIFI_SCAN_H_
#define __WIFI_SCAN_H_
#include "wifi.h"
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include "button.h"
#include "pico/stdlib.h"
#include "pico/cyw43_arch.h"

#define WIFI_SCAN_NO_SSID_SELECTED 0xffff
#define WIFI_SCAN_PERIOD_MS 10000

typedef struct WifiScanContext_s_
{
	struct repeating_timer wifiScanUpdateTimer;
	uint16_t cursor;
	uint16_t ssidSelectionIdx;
	uint64_t tsSinceLastScan;
} WifiScanContext_s;

#define WIFI_SCAN_UPDATE_PERIOD_MS 20

bool WifiScan_Init(void *arg);
void WifiScan_Deinit(void);
void WifiScan_Update(void);
void WifiScan_Draw(void);
void WifiScan_Start(void);
void WifiScan_Stop(void);
void WifiScan_ButtonInput(Button_e b, ButtonGesture_e g);
#endif
