#include "wifi_scan.h"
#include "oled_manager.h"
#include "pico/stdlib.h"
#include "hardware/timer.h"

WifiScanContext_s wifiScanContext;

static void WifiScan_PopulateWifiInfoString(cyw43_ev_scan_result_t *result)
{
	// Setup
	snprintf(wifiScanContext.wifiInfoStringBuf, WIFI_SCAN_INFO_STR_LEN, "ssid: %s\nrssi: %4d\nchan: %3d\nmac: %02x:%02x:%02x:%02x:%02x:%02x\nauth: 0x%x\n", result->ssid, result->rssi, result->channel, result->bssid[0], result->bssid[1], result->bssid[2], result->bssid[3], result->bssid[4], result->bssid[5], result->auth_mode);
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
	//Wifi_SetMode(WIFI_MODE_STATION);
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
	// if ((get_absolute_time() - wifiScanContext.tsSinceLastScan)/1000 >= WIFI_SCAN_PERIOD_MS)
	// {
	// 	Wifi_Scan();
	// 	wifiScanContext.tsSinceLastScan = get_absolute_time();
	// }
	
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
	if (b == BUTTON_1 && g == GESTURE_LONG_PRESS)
	{
		WifiScan_Init(NULL);
		return;
	}

	GuiItemActions_e guiItemAction = GuiMan_ButtonInputToGuiAction(b, g);
	if (wifiScanContext.wifiListbox.inFocus) // TODO bad lookin
	{
		GuiList_TakeActionInput(&(wifiScanContext.wifiListbox), guiItemAction);
	}
	else if (wifiScanContext.wifiInfoTextbox.inFocus)
	{
		GuiTextbox_TakeActionInput(&(wifiScanContext.wifiInfoTextbox), guiItemAction);
	}
}
