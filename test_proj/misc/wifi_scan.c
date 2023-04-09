#include "wifi_scan.h"
#include "oled_manager.h"
#include "pico/stdlib.h"
#include "hardware/timer.h"

WifiScanContext_s wifiScanContext;

static void WifiScan_DrawScanList(void)
{
	OledMan_ClearBuf();
	GuiList_Draw(&(wifiScanContext.wifiListBox));
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

static void WifiScan_GuiListItemSelectedCallback(uint16_t i)
{
	printf("Wifi scan selected idx %d\n", i);
}

static void WifiScan_GuiListExitedCallback(void)
{
	//
}

bool WifiScan_Init(void *arg)
{
	wifiScanContext.cursor = 0;
	wifiScanContext.ssidSelectionIdx = WIFI_SCAN_NO_SSID_SELECTED;
	Wifi_Scan();
	wifiScanContext.tsSinceLastScan = get_absolute_time();
	wifiScanContext.knownNumScanRecords = 0;
	wifiScanContext.wifiListBox = GuiList_Create(NULL, 0, WifiScan_GuiListItemSelectedCallback, WifiScan_GuiListExitedCallback);
	wifiScanContext.wifiListBox.inFocus = true;
}

void WifiScan_Deinit(void)
{
}

void WifiScan_Update(void)
{
	if ((get_absolute_time() - wifiScanContext.tsSinceLastScan)/1000 >= WIFI_SCAN_PERIOD_MS)
	{
		Wifi_Scan();
		wifiScanContext.tsSinceLastScan = get_absolute_time();
	}
	
	if (wifiScanContext.knownNumScanRecords != Wifi_GetNumScanRecords())
	{
		GuiList_SetStrings(&wifiScanContext.wifiListBox, Wifi_GetStringsList(), Wifi_GetNumScanRecords());
	}
	wifiScanContext.knownNumScanRecords = Wifi_GetNumScanRecords();
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
	GuiItemActions_e guiItemAction = GuiMan_ButtonInputToGuiAction(b, g);
	if (wifiScanContext.wifiListBox.inFocus)
	{
		GuiList_TakeActionInput(&(wifiScanContext.wifiListBox), guiItemAction);
	}
	// else if (wifiScanContext.wifiInfoText.inFocus)
	// {
	//
	// }

	// if (g == GESTURE_SINGLE_TAP)
	// {
	// 	if (b == BUTTON_0)
	// 	{
	// 		wifiScanContext.cursor -= 1;
	// 		if (wifiScanContext.cursor >= Wifi_GetNumScanRecords())
	// 		{
	// 			wifiScanContext.cursor = Wifi_GetNumScanRecords() - 1;
	// 		}
	// 	}
	// 	else if (b == BUTTON_1)
	// 	{
	// 		wifiScanContext.cursor += 1;
	// 		if (wifiScanContext.cursor >= Wifi_GetNumScanRecords())
	// 		{
	// 			wifiScanContext.cursor = 0;
	// 		}
	// 	}
	// }
	// 
	// if (g == GESTURE_DOUBLE_TAP)
	// {
	// 	if (b == BUTTON_0 && wifiScanContext.ssidSelectionIdx != WIFI_SCAN_NO_SSID_SELECTED)
	// 	{
	// 		wifiScanContext.ssidSelectionIdx = WIFI_SCAN_NO_SSID_SELECTED;
	// 	}
	// 	else if (b == BUTTON_1 && wifiScanContext.ssidSelectionIdx == WIFI_SCAN_NO_SSID_SELECTED)
	// 	{
	// 		wifiScanContext.ssidSelectionIdx = wifiScanContext.cursor;
	// 	}
	// }
}
