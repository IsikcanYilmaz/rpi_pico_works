#include "wifi.h"
#include "pico/stdlib.h"
#include "pico/cyw43_arch.h"
#include "hardware/timer.h"
#include "access_point/picow_access_point.h"
#include "tcp_recv_picture/tcp_recv_picture.h"
#include <string.h>

static struct repeating_timer wifiPollTimer;

static const char *authModeStrings[] = {
	[0] = "WEP_PSK",
	[1] = "WPA",
	[2] = "WPA2",
};

static const char *wifiModeStrings[] = {
	[WIFI_MODE_ACCESS_POINT] = "WIFI_MODE_ACCESS_POINT",
	[WIFI_MODE_STATION] = "WIFI_MODE_STATION",
	[WIFI_MODE_NONE] = "WIFI_MODE_NONE",
	[WIFI_MODE_MAX] = "WIFI_MODE_MAX",
};

static bool Wifi_NoneRoutineSanityInit(void *args){ // TODO unelegant
	printf("Wifi None Init Routine! This shouldnt happen!!\n");
	return true;
}
static void Wifi_NoneRoutineSanityDeinit(void){
	printf("Wifi None DeInit Routine! This shouldnt happen!!\n");
}

static WifiRoutine_s wifiRoutines[] = {
	[WIFI_ROUTINE_ACCESS_POINT_EXAMPLE] = (WifiRoutine_s) { .name = "access_point_example",
																													.init = PicowAp_Init,
																													.deinit = PicowAp_Deinit,
																													.poll = NULL,
																													.running = false,
																													.requiredMode = WIFI_MODE_ACCESS_POINT,
																													.requiredConnection = false,
																													.updatePeriodMs = 0,
																												},
	[WIFI_ROUTINE_TCP_RECV_PICTURE] = 	  (WifiRoutine_s) { .name = "tcp_recv_picture",
																													.init = TcpRecvPicture_Init,
																													.deinit = TcpRecvPicture_Deinit,
																													.poll = TcpRecvPicture_Update,
																													.running = false,
																													.requiredMode = WIFI_MODE_STATION,
																													.requiredConnection = true,
																													.updatePeriodMs = TCP_RECV_PICTURE_UPDATE_PERIOD_MS,
																												},
	[WIFI_ROUTINE_NONE] = 								(WifiRoutine_s) { .name = "none",
																													.init = Wifi_NoneRoutineSanityInit,
																													.deinit = Wifi_NoneRoutineSanityInit,
																													.poll = NULL,
																													.running = false,
																													.requiredMode = WIFI_MODE_NONE,
																													.requiredConnection = false,
																													.updatePeriodMs = 0,
																												},
};

WifiContext_t wifiContext;

static void Wifi_PrintAuthMode(int auth)
{
	if (auth == 0)
	{
		printf("OPEN ");
		return;
	}
	for (uint8_t i = 0; i < sizeof(authModeStrings)/sizeof(authModeStrings[0]); i++)
	{
		if (auth & (1 << i))
		{
			printf("%s ", authModeStrings[i]);
		}
	}
}

static void Wifi_PrintScanResult(WifiAccessPoint_s *result)
{
	printf("ssid: %-32s rssi: %4d chan: %3d mac: %02x:%02x:%02x:%02x:%02x:%02x sec: 0x%x ",
					 result->ssid, result->rssi, result->channel,
					 result->bssid[0], result->bssid[1], result->bssid[2], result->bssid[3], result->bssid[4],
					 result->bssid[5], result->auth_mode);
	Wifi_PrintAuthMode(result->auth_mode);
	printf("\n");
}

static bool Wifi_CompareMac(uint8_t *buf1, uint8_t *buf2)
{
	for (uint8_t i = 0; i < 6; i++)
	{
		if (buf1[i] != buf2[i])
		{
			return false;
		}
	}
	return true;
}

// TODO horrible name :(
static void Wifi_ConvertCyw43ScanResultToLocalStruct(cyw43_ev_scan_result_t *src, WifiAccessPoint_s *dst)
{
	dst->rssi = src->rssi;
	dst->auth_mode = src->auth_mode;
	dst->ssid_len = src->ssid_len;
	dst->channel = src->channel;
	strcpy(dst->ssid, src->ssid);
	memcpy(dst->bssid, src->bssid, 6);
}

static bool Wifi_RecordScanResult(cyw43_ev_scan_result_t *result)
{
	for (uint16_t i = 0; i < wifiContext.scanNumDevices; i++)
	{
		if (Wifi_CompareMac(&(result->bssid), &(wifiContext.scanBuf[i].bssid))) // we saw this before
		{
			// wifiContext.scanBuf[i] = *result;	
			Wifi_ConvertCyw43ScanResultToLocalStruct(result, &wifiContext.scanBuf[i]);
			wifiContext.ssidStrings[i] = wifiContext.scanBuf[i].ssid;
			return true;
		}
	}
	if (wifiContext.scanNumDevices == WIFI_SCAN_BUF_LEN-1)
	{
		printf("Scan buf full!\n");
		return false;
	}
	Wifi_ConvertCyw43ScanResultToLocalStruct(result, &wifiContext.scanBuf[wifiContext.scanNumDevices]);
	wifiContext.ssidStrings[wifiContext.scanNumDevices] = wifiContext.scanBuf[wifiContext.scanNumDevices].ssid;
	wifiContext.scanNumDevices++;
}

static int Wifi_ScanResult(void *env, const cyw43_ev_scan_result_t *result) {
	if (result) {
		// Wifi_PrintScanResult(result);
		Wifi_RecordScanResult(result);
	}
	return 0;
}

 bool Wifi_Poll(struct repeating_timer *t)
{
	// first poll to see the wifi status. then
	// check if we have any pending requests
	bool pollRet = true;
	if (wifiContext.connectRequested)
	{
		pollRet |= Wifi_Connect(wifiContext.reqConnectSsid, wifiContext.reqConnectPw);
		wifiContext.connectRequested = false;
	}

	// we'll poll the currently running routine if it wants
	if (wifiContext.currentRoutineIdx < WIFI_ROUTINE_NONE)
	{
		if (wifiContext.currentRoutine->poll == NULL)
		{
			pollRet |= true;
		}
		else
		{
			pollRet = wifiContext.currentRoutine->poll();
		}	
	}
	if (!pollRet)
	{
		printf("Wifi poll failed at some point!\n"); 
	}
	return pollRet;
}

void Wifi_Init(void)
{
	wifiContext.isConnected = false;
	wifiContext.connectRequested = false;
	wifiContext.mode = WIFI_MODE_NONE;
	wifiContext.currentRoutineIdx = WIFI_ROUTINE_NONE;
	wifiContext.currentRoutine = &wifiRoutines[WIFI_ROUTINE_NONE];
	memset(&wifiContext.connectedAp, 0x00, sizeof(WifiAccessPoint_s));
	memset(wifiContext.apSsid, 0x00, sizeof(WIFI_AP_SSID_MAX_LEN));
	memset(wifiContext.apPass, 0x00, sizeof(WIFI_AP_PASS_MAX_LEN));
	strcpy(wifiContext.apSsid, WIFI_AP_DEFAULT_SSID);
	strcpy(wifiContext.apPass, WIFI_AP_DEFAULT_PASS);
	wifiContext.cyw43_state = &cyw43_state;
	Wifi_ClearScanBuf();
	cyw43_arch_init();
	Wifi_SetMode(WIFI_DEFAULT_MODE);
}

void Wifi_Deinit(void)
{
	cyw43_arch_deinit();
}

void Wifi_ClearScanBuf(void)
{
	memset(&wifiContext.scanBuf, 0x00, sizeof(WifiAccessPoint_s) * WIFI_SCAN_BUF_LEN);
	memset(&wifiContext.ssidStrings, 0x00, sizeof(char *) * WIFI_SCAN_BUF_LEN);
	wifiContext.scanNumDevices = 0;
}

bool Wifi_Scan(void)
{
	if (wifiContext.mode != WIFI_MODE_STATION)
	{
		printf("Wrong mode for scanning %d:%s\n", wifiContext.mode, wifiModeStrings[wifiContext.mode]);
		return false;
	}
	cyw43_wifi_scan_options_t scan_options = {0};
	int err = cyw43_wifi_scan(&cyw43_state, &scan_options, NULL, Wifi_ScanResult);
	if (err == 0)
	{
		printf("Performing wifi scan\n");
		return true;
	}
	else
	{
		printf("Failed to start scan: %d\n", err);
		return false;
	}
}

void Wifi_PollTimerStart(void)
{
	add_repeating_timer_ms(WIFI_POLL_PERIOD_MS, Wifi_Poll, NULL, &wifiPollTimer);
}

void Wifi_PollTimerStop(void)
{
	cancel_repeating_timer(&wifiPollTimer);
}

WifiRoutine_s *Wifi_GetRoutinePtrByIdx(WifiRoutine_e r)
{
	if (r <= WIFI_ROUTINE_NONE) // todo this could be done better? there is a MAX and a NONE enum? 
	{
		return &wifiRoutines[r];
	}
	else
	{
		return NULL;
	}
}

bool Wifi_Disconnect(void)
{
	if (!wifiContext.isConnected)
	{
		printf("Wifi not connected, cant disconnect\n");
		return true;
	}
	int ret = cyw43_wifi_leave(wifiContext.cyw43_state, CYW43_ITF_STA);
	if (ret)
	{
		printf("Could not leave network %d\n", ret);
	}
	else
	{
		wifiContext.connected = false;
	}
	return ret == 0;
}

bool Wifi_Connect(char *ssid, char *pass)
{
	if (wifiContext.mode != WIFI_MODE_STATION)
	{
		printf("Wrong mode for connection %d:%s\n", wifiContext.mode, wifiModeStrings[wifiContext.mode]);
		return false;
	}
	printf("Attempting to connect to AP SSID:%s PASS:%s\n", ssid, pass);
	if (cyw43_arch_wifi_connect_timeout_ms(ssid, pass, CYW43_AUTH_WPA2_AES_PSK, WIFI_CONNECT_TIMEOUT_MS)) 
	{
		printf("Failed to connect!\n");
		wifiContext.isConnected = false;
	}
	else
	{
		printf("Connected!\n");
		wifiContext.connectedAp.ssid_len = strlen(ssid);
		strcpy(wifiContext.connectedAp.ssid, ssid);
		cyw43_wifi_get_bssid(wifiContext.cyw43_state, wifiContext.connectedAp.bssid);
		cyw43_wifi_get_rssi(wifiContext.cyw43_state, &wifiContext.connectedAp.rssi);
		wifiContext.connectedAp.auth_mode = (uint8_t) cyw43_wifi_ap_get_auth(wifiContext.cyw43_state);
		wifiContext.connectedAp.channel = 0;
		wifiContext.isConnected = true;
	}
	return wifiContext.connected;
}

void Wifi_RequestConnect(char *ssid, char *pass)
{
	strncpy(wifiContext.reqConnectSsid, ssid, WIFI_AP_SSID_MAX_LEN);
	strncpy(wifiContext.reqConnectPw, pass, WIFI_AP_PASS_MAX_LEN);
	wifiContext.connectRequested = true;
}

bool Wifi_UnsetCurrentRoutine(void)
{
	return Wifi_SetRoutine(WIFI_ROUTINE_NONE, NULL);
}

bool Wifi_SetRoutine(WifiRoutine_e r, void *arg)
{
	bool ret = true;

	// Check if arg is good
	if (r >= WIFI_ROUTINE_MAX)
	{
		printf("Bad routine idx %d:%s\n", r, wifiRoutines[r].name);
		return false;
	}

	if (r == wifiContext.currentRoutineIdx)
	{
		printf("Already running routine %d:%s\n", r, wifiRoutines[r].name);
		return false;
	}

	WifiRoutine_s *targetRoutine = Wifi_GetRoutinePtrByIdx(r);

	// first deinit the current routine and unset it
	// cyw43_arch_deinit();
	ret = wifiContext.currentRoutine->deinit();
	// cyw43_arch_init();
	wifiContext.currentRoutine = NULL;
	wifiContext.currentRoutineIdx = WIFI_ROUTINE_NONE;
	if (!ret)
	{
		printf("Couldnt deinit current routine %d:%s\n", wifiContext.currentRoutine, wifiContext.currentRoutine->name);
		return false;
	}

	if (r == WIFI_ROUTINE_NONE)
	{
		return true;
	}

	// the routine we want needs a different mode. deinit the current one and init the next
	if (wifiContext.mode != targetRoutine->requiredMode)
	{
		printf("Need to change wifi modes!\n");
		Wifi_UnsetCurrentMode();
	}

	// now that we unset our routine we can set the new mode
	ret = Wifi_SetMode(targetRoutine->requiredMode);
	if (!ret)
	{
		printf("Couldnt set mode %d\n", targetRoutine->requiredMode);
		return false;
	}

	// now init the target routine
	ret = targetRoutine->init(arg);
	if (!ret)
	{
		printf("Couldnt init target routine %s:%d\n", targetRoutine->name, r);
		return false;
	}
	wifiContext.currentRoutine = targetRoutine;
	wifiContext.currentRoutineIdx = r;
	return ret;
}

bool Wifi_UnsetCurrentMode(void)
{
	printf("Unsetting wifi mode %d:%s\n", wifiContext.mode, wifiModeStrings[wifiContext.mode]);
	int ret = 0;
	switch (wifiContext.mode)
	{
		case WIFI_MODE_STATION:
		{
			ret = cyw43_wifi_leave(wifiContext.cyw43_state, CYW43_ITF_STA);
			break;
		}
		case WIFI_MODE_ACCESS_POINT:
		{
			ret = cyw43_wifi_leave(wifiContext.cyw43_state, CYW43_ITF_AP);
			break;
		}
		default:
		{
			printf("Nothing to unset for current mode %d:%s\n", wifiContext.mode, wifiModeStrings[wifiContext.mode]);
			break;
		}
	}
	wifiContext.mode = WIFI_MODE_NONE;
	return true;
}

bool Wifi_SetMode(WifiMode_e m)
{
	printf("Setting wifi mode to %d:%s\n", m, wifiModeStrings[m]);
	if (m >= WIFI_MODE_MAX)
	{
		printf("Bad wifi mode %d\n", m);
		return false;
	}

	if (m == wifiContext.mode)
	{
		printf("Already in mode %d:%s\n", m, wifiModeStrings[m]);
		return true;
	}

	// if we're currently running a routine that has a different mode requirement
	// then deinit the routine also.
	if (wifiContext.currentRoutineIdx != WIFI_ROUTINE_NONE)
	{
		printf("Need to exit out of the current wifi routine first! %d:%s\n", wifiContext.currentRoutineIdx, wifiContext.currentRoutine->name);
		return false;
	}

	if (wifiContext.mode != WIFI_MODE_NONE)
	{
		Wifi_UnsetCurrentMode();
	}

	switch(m)
	{
		case WIFI_MODE_STATION:
		{
			cyw43_arch_enable_sta_mode();
			break;
		}
		case WIFI_MODE_ACCESS_POINT:
		{
			printf("Pico AP SSID: %s, PASS: %s\n", wifiContext.apSsid, wifiContext.apPass);
			cyw43_arch_enable_ap_mode(wifiContext.apSsid, wifiContext.apPass, WIFI_AP_DEFAULT_AUTH);
			break;
		}
		case WIFI_MODE_NONE: // For now lets let NONE mode do STATION
		{
			break;
		}
		default:
		{
			printf("Bad wifi mode set %d\n", m);
			return false;
		}
	}
	wifiContext.mode = m;
	return true;
}

void Wifi_PrintRecords(void)
{
	for (uint16_t i = 0; i < wifiContext.scanNumDevices; i++)
	{
		printf("%d ", i);
		Wifi_PrintScanResult(&(wifiContext.scanBuf[i]));
	}
}

uint16_t Wifi_GetNumScanRecords(void)
{
	return wifiContext.scanNumDevices;
}

WifiAccessPoint_s* Wifi_GetScanRecordByIdx(uint16_t idx)
{
	if (idx >= wifiContext.scanNumDevices) 
	{
		printf("Bad scan record idx! %d\n", idx);
		return NULL;
	}
	return &(wifiContext.scanBuf[idx]);
}

char** Wifi_GetStringsList(void)
{
	return wifiContext.ssidStrings;
}

WifiRoutine_e Wifi_GetCurrentRoutine(void)
{
	return (WifiRoutine_e) wifiContext.currentRoutineIdx;
}

bool Wifi_IsRoutineRunning(void)
{
	return Wifi_GetCurrentRoutine() != WIFI_ROUTINE_NONE;
}

void Wifi_PrintInfo(void)
{
	printf("Wifi Info:\n");
	printf("Mode: %d:%s\n", wifiContext.mode, wifiModeStrings[wifiContext.mode]);
	printf("Routine: %d:%s\n", wifiContext.currentRoutineIdx, wifiContext.currentRoutine->name);
	if (wifiContext.isConnected)
	{
		printf("Connected to wifi access point:\n");
		Wifi_PrintScanResult(&wifiContext.connectedAp);
	}
}
