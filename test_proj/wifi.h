#ifndef __WIFI_H_
#define __WIFI_H_
#include <stdlib.h>
#include <stdbool.h>
#include <stdint.h>
#include "pico/stdlib.h"
#include "pico/cyw43_arch.h"

#define WIFI_SCAN_POLL_PERIOD_MS 10
#define WIFI_CONNECT_TIMEOUT_MS 10000 
#define WIFI_SCAN_BUF_LEN 50

typedef struct WifiContext_s
{
	bool isConnected;
	char *ssid;
	cyw43_ev_scan_result_t scanBuf[WIFI_SCAN_BUF_LEN];
	uint16_t scanNumDevices;
} WifiContext_t;

void Wifi_Init(void);
void Wifi_Scan(void);
void Wifi_PollTimerStart(void);
void Wifi_PollTimerStop(void);
bool Wifi_Connect(char *ssid, char *pass);
void Wifi_PrintRecords(void);
void Wifi_ClearScanBuf(void);
uint16_t Wifi_GetNumScanRecords(void);
cyw43_ev_scan_result_t* Wifi_GetScanRecordByIdx(uint16_t idx);
#endif
