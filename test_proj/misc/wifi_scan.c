#include "wifi_scan.h"
#include "oled_manager.h"
#include "pico/stdlib.h"
#include "hardware/timer.h"

WifiScanContext_s wifiScanContext;

static void WifiScan_PopulateWifiInfoString(cyw43_ev_scan_result_t *result)
{
	// Setup
	// char *ssidStr = &wifiScanContext.wifiInfoStringBuf[WIFI_SCAN_INFO_SSID_IDX];
	// char *rssiStr = &wifiScanContext.wifiInfoStringBuf[WIFI_SCAN_INFO_RSSI_IDX]; 
	// char *chanStr = &wifiScanContext.wifiInfoStringBuf[WIFI_SCAN_INFO_CHAN_IDX];
	// char *macStr = &wifiScanContext.wifiInfoStringBuf[WIFI_SCAN_INFO_MAC_IDX];
	// char *authStr = &wifiScanContext.wifiInfoStringBuf[WIFI_SCAN_INFO_AUTH_IDX];
	//
	// snprintf(ssidStr, WIFI_SCAN_INFO_SSID_LEN, "ssid: %s", result->ssid);
	// snprintf(rssiStr, WIFI_SCAN_INFO_RSSI_LEN, "rssi: %4d", result->rssi);
	// snprintf(chanStr, WIFI_SCAN_INFO_CHAN_LEN, "chan: %3d", result->channel);
	// snprintf(macStr, WIFI_SCAN_INFO_MAC_LEN, "mac: %02x:%02x:%02x:%02x:%02x:%02x", result->bssid[0], result->bssid[1], result->bssid[2], result->bssid[3], result->bssid[4], result->bssid[5]);
	// snprintf(authStr, WIFI_SCAN_INFO_AUTH_LEN, "auth: 0x%x", result->auth_mode);

	snprintf(wifiScanContext.wifiInfoStringBuf, WIFI_SCAN_INFO_STR_LEN, "ssid: %s \nrssi: %4d \nchan: %3d \nmac: %02x:%02x:%02x:%02x:%02x:%02x \nauth: 0x%x\n", result->ssid, result->rssi, result->channel, result->bssid[0], result->bssid[1], result->bssid[2], result->bssid[3], result->bssid[4], result->bssid[5], result->auth_mode);
}

static void WifiScan_GuiListItemSelectedCallback(uint16_t i)
{
	printf("Wifi scan selected idx %d\n", i);
	
	// sanity check
	if (i >= Wifi_GetNumScanRecords())
	{
		printf("Wifi scan bad idx. num wifi records %d\n", Wifi_GetNumScanRecords());
		return;
	}

	// A wifi record is selected. make the list not in focus and the textbox in focus and display its info.
	cyw43_ev_scan_result_t* r = Wifi_GetScanRecordByIdx(i);
	if (r)
	{
		WifiScan_PopulateWifiInfoString(r);
		printf("%s\n", wifiScanContext.wifiInfoStringBuf);
		wifiScanContext.wifiListbox.inFocus = false;
		wifiScanContext.wifiInfoTextbox.inFocus = true;
		wifiScanContext.ssidSelectionIdx = i;
	}
}

static void WifiScan_GuiListExitedCallback(void)
{
}

static void WifiScan_GuiTextboxExitedCallback(void)
{
	wifiScanContext.wifiListbox.inFocus = true;
	wifiScanContext.wifiInfoTextbox.inFocus = false;
}

bool WifiScan_Init(void *arg)
{
	wifiScanContext.cursor = 0;
	wifiScanContext.ssidSelectionIdx = WIFI_SCAN_NO_SSID_SELECTED;
	Wifi_Scan();
	wifiScanContext.tsSinceLastScan = get_absolute_time();
	wifiScanContext.knownNumScanRecords = 0;
	wifiScanContext.wifiListbox = GuiList_Create(NULL, 0, WifiScan_GuiListItemSelectedCallback, WifiScan_GuiListExitedCallback);
	wifiScanContext.wifiInfoTextbox = GuiTextbox_Create(wifiScanContext.wifiInfoStringBuf, WifiScan_GuiTextboxExitedCallback);
	memset(wifiScanContext.wifiInfoStringBuf, 0x00, WIFI_SCAN_INFO_STR_LEN);
	
	wifiScanContext.wifiListbox.inFocus = true;
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
		GuiList_SetStrings(&wifiScanContext.wifiListbox, Wifi_GetStringsList(), Wifi_GetNumScanRecords());
	}
	wifiScanContext.knownNumScanRecords = Wifi_GetNumScanRecords();
}

void WifiScan_Draw(void)
{
	// cyw43_ev_scan_result_t* r = Wifi_GetScanRecordByIdx(wifiScanContext.ssidSelectionIdx);
	// if (r) WifiScan_PopulateWifiInfoString(Wifi_GetScanRecordByIdx(wifiScanContext.ssidSelectionIdx));

	OledMan_ClearBuf();
	if (wifiScanContext.wifiListbox.inFocus)
	{
		GuiList_Draw(&wifiScanContext.wifiListbox);
	}
	else if (wifiScanContext.wifiInfoTextbox.inFocus)
	{
		GuiTextbox_Draw(&wifiScanContext.wifiInfoTextbox);
	}
	else
	{
		printf("wifi scan no gui item in focus. this shouldnt happen!\n");
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
	if (wifiScanContext.wifiListbox.inFocus) // TODO bad lookin
	{
		GuiList_TakeActionInput(&(wifiScanContext.wifiListbox), guiItemAction);
	}
	else if (wifiScanContext.wifiInfoTextbox.inFocus)
	{
		GuiTextbox_TakeActionInput(&(wifiScanContext.wifiInfoTextbox), guiItemAction);
	}

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
