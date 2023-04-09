#ifndef __WIFI_SCAN_H_
#define __WIFI_SCAN_H_
#include "wifi.h"
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include "button.h"
#include "pico/stdlib.h"
#include "pico/cyw43_arch.h"
#include "gui_manager.h"
#include "gui_list.h"
#include "gui_textbox.h"

#define WIFI_SCAN_NO_SSID_SELECTED 0xffff
#define WIFI_SCAN_PERIOD_MS 10000
#define WIFI_SCAN_INFO_STR_LEN 255

#define WIFI_SCAN_INFO_SSID_LEN (30 + 1) // + \0
#define WIFI_SCAN_INFO_SSID_IDX (0)

#define WIFI_SCAN_INFO_RSSI_LEN (11 + 1) // + \0
#define WIFI_SCAN_INFO_RSSI_IDX (0 + WIFI_SCAN_INFO_SSID_LEN)

#define WIFI_SCAN_INFO_CHAN_LEN (10 + 1) // + \0
#define WIFI_SCAN_INFO_CHAN_IDX (WIFI_SCAN_INFO_RSSI_IDX + WIFI_SCAN_INFO_RSSI_LEN)

#define WIFI_SCAN_INFO_MAC_LEN (30 + 1) // + \0
#define WIFI_SCAN_INFO_MAC_IDX (WIFI_SCAN_INFO_CHAN_IDX + WIFI_SCAN_INFO_CHAN_LEN)

#define WIFI_SCAN_INFO_AUTH_LEN (10 + 1) // + \0
#define WIFI_SCAN_INFO_AUTH_IDX (WIFI_SCAN_INFO_MAC_IDX + WIFI_SCAN_INFO_MAC_LEN)

typedef struct WifiScanContext_s_
{
	struct repeating_timer wifiScanUpdateTimer;
	uint16_t cursor;
	uint16_t ssidSelectionIdx;
	uint64_t tsSinceLastScan;
	GuiList_t wifiListbox;
	GuiTextbox_t wifiInfoTextbox;
	char wifiInfoStringBuf[WIFI_SCAN_INFO_STR_LEN];
	uint16_t knownNumScanRecords;
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
