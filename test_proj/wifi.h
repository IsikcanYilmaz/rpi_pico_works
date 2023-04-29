#ifndef __WIFI_H_
#define __WIFI_H_
#include <stdlib.h>
#include <stdbool.h>
#include <stdint.h>
#include "pico/stdlib.h"
#include "pico/cyw43_arch.h"

#define WIFI_POLL_PERIOD_MS 250
#define WIFI_CONNECT_TIMEOUT_MS 10000 
#define WIFI_SCAN_BUF_LEN 50

#define WIFI_AP_SSID_MAX_LEN 50
#define WIFI_AP_PASS_MAX_LEN 16

#define WIFI_AP_DEFAULT_SSID "picow_test"
#define WIFI_AP_DEFAULT_PASS "password"
#define WIFI_AP_DEFAULT_AUTH CYW43_AUTH_WPA2_AES_PSK

#define WIFI_DEFAULT_MODE WIFI_MODE_STATION

typedef enum WifiMode_e_
{
	WIFI_MODE_ACCESS_POINT,
	WIFI_MODE_STATION,
	WIFI_MODE_NONE,
	WIFI_MODE_MAX,
} WifiMode_e;

typedef enum WifiRoutine_e_
{
	WIFI_ROUTINE_ACCESS_POINT_EXAMPLE,
	// WIFI_ROUTINE_SCAN,
	WIFI_ROUTINE_TCP_RECV_PICTURE,
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
	bool requiredConnection;
} WifiRoutine_s;

typedef struct WifiAccessPoint_s_
{
	uint8_t bssid[6];
	uint8_t ssid_len;
	uint8_t ssid[WIFI_AP_SSID_MAX_LEN];
	uint16_t channel;
	uint8_t auth_mode;
	int32_t rssi;
} WifiAccessPoint_s;

typedef struct WifiContext_s
{
	bool isConnected;
	char apSsid[WIFI_AP_SSID_MAX_LEN]; // ssid and pass of our access point
	char apPass[WIFI_AP_PASS_MAX_LEN];
	WifiAccessPoint_s scanBuf[WIFI_SCAN_BUF_LEN];
	char *ssidStrings[WIFI_SCAN_BUF_LEN]; // for gui
	uint16_t scanNumDevices;
	WifiMode_e mode;
	WifiRoutine_s *currentRoutine;
	WifiRoutine_e currentRoutineIdx;
	WifiAccessPoint_s connectedAp;
	bool connectRequested;
	char reqConnectSsid[WIFI_AP_SSID_MAX_LEN];
	char reqConnectPw[WIFI_AP_PASS_MAX_LEN];
	bool connected;
	cyw43_t *cyw43_state;
} WifiContext_t;

void Wifi_Init(void);
void Wifi_Deinit(void);
bool Wifi_Poll(struct repeating_timer *t);
void Wifi_RequestConnect(char *ssid, char *pass);
WifiRoutine_s *Wifi_GetRoutinePtrByIdx(WifiRoutine_e r); // todo this chould be a static
bool Wifi_SetMode(WifiMode_e m);
bool Wifi_UnsetCurrentMode(void);
bool Wifi_SetRoutine(WifiRoutine_e r, void *arg);
bool Wifi_UnsetCurrentRoutine(void);
bool Wifi_Scan(void);
void Wifi_PollTimerStart(void);
void Wifi_PollTimerStop(void);
bool Wifi_Connect(char *ssid, char *pass);
bool Wifi_Disconnect(void);
void Wifi_PrintRecords(void);
void Wifi_ClearScanBuf(void);
uint16_t Wifi_GetNumScanRecords(void);
WifiAccessPoint_s* Wifi_GetScanRecordByIdx(uint16_t idx);
char** Wifi_GetStringsList(void);
WifiRoutine_e Wifi_GetCurrentRoutine(void);
bool Wifi_IsRoutineRunning(void);
void Wifi_PrintInfo(void);
#endif
