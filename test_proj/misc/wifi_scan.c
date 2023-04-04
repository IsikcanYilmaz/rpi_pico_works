#include "wifi_scan.h"
#include "oled_manager.h"

WifiScanContext_s wifiScanContext;

static void WifiScan_DrawScanList(void)
{
	uint8_t x, y, rectW, rectH;
	rectW = 117;
	rectH = 10;
	for (uint16_t i = 0; i < Wifi_GetNumScanRecords(); i++)
	{
		x = 10;
		y = i * 10;
		cyw43_ev_scan_result_t *scanRecord = Wifi_GetScanRecordByIdx(i);
		if (scanRecord == NULL)
		{
			printf("Problem with wifi scan record %d\n", i);
			continue;
		}
		char *name = scanRecord->ssid;
		OledMan_SetColor(WHITE);
		if (i == wifiScanContext.cursor)
		{
			// draw full rect white
			// draw string black
			OledMan_SetColor(WHITE);
			OledMan_DrawRectangle(x, y, rectW, rectH, 1);
			OledMan_SetColor(BLACK);
			OledMan_DrawString(x, y + 2, name);
			OledMan_SetColor(WHITE);
			OledMan_DrawRectangle(5, y + 4, 2, 2, 1);
		}
		else
		{
			// draw full rect black 
			// draw string white 
			OledMan_DrawRectangle(x, y, rectW, rectH, 0);
			OledMan_SetColor(WHITE);
			OledMan_DrawString(x, y + 2, name);
		}
	}
}

static void WifiScan_DrawScanRecordInfo(cyw43_ev_scan_result_t *result)
{
	// Setup
	char ssidStr[38];
	char rssiStr[11];
	char chanStr[10];
	char macStr[30];
	char authStr[10];
	snprintf(ssidStr, sizeof(ssidStr), "ssid: %s", result->ssid);
	snprintf(rssiStr, sizeof(rssiStr), "rssi: %4d", result->rssi);
	snprintf(chanStr, sizeof(chanStr), "chan: %3d", result->channel);
	snprintf(macStr, sizeof(macStr), "mac: %02x:%02x:%02x:%02x:%02x:%02x", result->bssid[0], result->bssid[1], result->bssid[2], result->bssid[3], result->bssid[4], result->bssid[5]);
	snprintf(authStr, sizeof(authStr), "auth: 0x%x", result->auth_mode);

	// draw frame
	OledMan_DrawRectangle(0, 0, 127, 63, 0);
	OledMan_DrawString(1, 1, ssidStr);
	OledMan_DrawString(1, 25, rssiStr);
	OledMan_DrawString(1, 35, chanStr);
	OledMan_DrawString(1, 45, macStr);
	OledMan_DrawString(1, 55, authStr);
}

bool WifiScan_Init(void *arg)
{
	wifiScanContext.cursor = 0;
	wifiScanContext.ssidSelectionIdx = WIFI_SCAN_NO_SSID_SELECTED;
	Wifi_Scan();
}

void WifiScan_Deinit(void)
{
}

void WifiScan_Update(void)
{
}

void WifiScan_Draw(void)
{
	OledMan_ClearBuf();
	if (wifiScanContext.ssidSelectionIdx == WIFI_SCAN_NO_SSID_SELECTED)
	{
		WifiScan_DrawScanList();
	}
	else
	{
		cyw43_ev_scan_result_t* r = Wifi_GetScanRecordByIdx(wifiScanContext.ssidSelectionIdx);
		if (r) WifiScan_DrawScanRecordInfo(Wifi_GetScanRecordByIdx(wifiScanContext.ssidSelectionIdx));
	}
}

void WifiScan_Start(void)
{
}

void WifiScan_Stop(void)
{
}

void WifiScan_ButtonInput(Button_e b, ButtonGesture_e g)
{
	if (g == GESTURE_SINGLE_TAP)
	{
		if (b == BUTTON_0)
		{
			wifiScanContext.cursor -= 1;
			if (wifiScanContext.cursor >= Wifi_GetNumScanRecords())
			{
				wifiScanContext.cursor = Wifi_GetNumScanRecords() - 1;
			}
		}
		else if (b == BUTTON_1)
		{
			wifiScanContext.cursor += 1;
			if (wifiScanContext.cursor >= Wifi_GetNumScanRecords())
			{
				wifiScanContext.cursor = 0;
			}
		}
	}
	
	if (g == GESTURE_DOUBLE_TAP)
	{
		if (b == BUTTON_0 && wifiScanContext.ssidSelectionIdx != WIFI_SCAN_NO_SSID_SELECTED)
		{
			wifiScanContext.ssidSelectionIdx = WIFI_SCAN_NO_SSID_SELECTED;
		}
		else if (b == BUTTON_1 && wifiScanContext.ssidSelectionIdx == WIFI_SCAN_NO_SSID_SELECTED)
		{
			wifiScanContext.ssidSelectionIdx = wifiScanContext.cursor;
		}
	}
}
