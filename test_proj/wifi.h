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

#define WIFI_AP_SSID_MAX_LEN 50
#define WIFI_AP_PASS_MAX_LEN 16

#define WIFI_AP_DEFAULT_SSID "picow_test"
#define WIFI_AP_DEFAULT_PASS "password"
#define WIFI_AP_DEFAULT_AUTH CYW43_AUTH_WPA2_AES_PSK

typedef enum WifiMode_e_
{
	WIFI_MODE_ACCESS_POINT,
	WIFI_MODE_STATION,
	WIFI_MODE_STATION_CONNECTED,
	WIFI_MODE_NONE,
	WIFI_MODE_MAX,
} WifiMode_e;

typedef enum WifiRoutine_e_
{
	WIFI_ROUTINE_ACCESS_POINT_EXAMPLE,
	// WIFI_ROUTINE_SCAN,
	WIFI_ROUTINE_TCP_RECV_TEST,
	WIFI_ROUTINE_NONE,
	WIFI_ROUTINE_MAX,
} WifiRoutine_e;

typedef struct WifiRoutine_s_
{
	char *name;
	bool (*init)(void *args);
	bool (*deinit)(void);
	bool (*poll)(void);
	bool running;
	uint16_t updatePeriodMs;
	WifiMode_e requiredMode;
} WifiRoutine_s;

typedef struct WifiContext_s
{
	bool isConnected;
	char apSsid[WIFI_AP_SSID_MAX_LEN]; // ssid and pass of our access point
	char apPass[WIFI_AP_PASS_MAX_LEN];
	cyw43_ev_scan_result_t scanBuf[WIFI_SCAN_BUF_LEN];
	char *ssidStrings[WIFI_SCAN_BUF_LEN]; // for gui
	uint16_t scanNumDevices;
	WifiMode_e mode;
	WifiRoutine_s *currentRoutine;
	WifiRoutine_e currentRoutineIdx;
} WifiContext_t;

void Wifi_Init(void);
void Wifi_Deinit(void);
WifiRoutine_s *Wifi_GetRoutinePtrByIdx(WifiRoutine_e r); // todo this chould be a static
bool Wifi_SetMode(WifiMode_e m);
bool Wifi_UnsetCurrentMode(void);
bool Wifi_SetRoutine(WifiRoutine_e r, void *arg);
bool Wifi_UnsetCurrentRoutine(void);
bool Wifi_Scan(void);
void Wifi_PollTimerStart(void);
void Wifi_PollTimerStop(void);
bool Wifi_Connect(char *ssid, char *pass);
void Wifi_PrintRecords(void);
void Wifi_ClearScanBuf(void);
uint16_t Wifi_GetNumScanRecords(void);
cyw43_ev_scan_result_t* Wifi_GetScanRecordByIdx(uint16_t idx);
char** Wifi_GetStringsList(void);
#endif
