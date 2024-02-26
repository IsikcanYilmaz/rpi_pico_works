#include "wifi_scan.h"
#include "gui_text_input.h"
#include "oled_manager.h"
#include "pico/stdlib.h"
#include "hardware/timer.h"
#include "wifi.h"

WifiScanContext_s wifiScanContext;

WifiKnownAP_s wifiKnownAps[] = {
	{"RPI_WIFI", "asdfgqwert"},
};

// If known, will return pointer to our ssid->pw pair. If not null
static WifiKnownAP_s * WifiScan_CheckIfApIsKnown(WifiAccessPoint_s *result)
{
	for (uint16_t i = 0; i < sizeof(wifiKnownAps) / sizeof(WifiKnownAP_s); i++)
	{
		if (strcmp(result->ssid, wifiKnownAps[i].ssid) == 0)
		{
			return &wifiKnownAps[i];
		}
	}
	return NULL;
}

// Populate info textbox with selected AP's info
static void WifiScan_PopulateWifiInfoString(WifiAccessPoint_s *result)
{
	// Setup
	snprintf(wifiScanContext.messageBuffer, WIFI_SCAN_INFO_STR_LEN, "ssid: %s\nrssi: %4d\nchan: %3d\nmac: %02x:%02x:%02x:%02x:%02x:%02x\nauth: 0x%x\n", result->ssid, result->rssi, result->channel, result->bssid[0], result->bssid[1], result->bssid[2], result->bssid[3], result->bssid[4], result->bssid[5], result->auth_mode);
}

// Populate textbox with our info message "Connecting" // This is not needed
// static void WifiScan_PopulateConnectingInfoString(WifiAccessPoint_s *result)
// {
// 	snprinf(wifiScanContext.messageBuffer, WIFI_SCAN_INFO_STR_LEN, "Connecting to %s\n", result->ssid);
// }
//
// Wifi AP selected from list
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
	WifiAccessPoint_s* r = Wifi_GetScanRecordByIdx(i);
	if (r)
	{
		// AP selected. Check if this is a known AP. If so, prepopulate pw box
		WifiKnownAP_s *knownAp = WifiScan_CheckIfApIsKnown(r);
		if (knownAp)
		{
			GuiTextInput_SetString(&(wifiScanContext.wifiPwInputBox), knownAp->pw);
		}

		WifiScan_PopulateWifiInfoString(r);
		wifiScanContext.wifiListbox.inFocus = false;
		GuiTextbox_SetInFocus(&(wifiScanContext.wifiInfoTextbox), true); // TODO do the other gui elements like this
		wifiScanContext.ssidSelectionIdx = i;
	}
}

// Nothing connects to this callback right now
static void WifiScan_GuiListExitedCallback(void)
{
}

// Wifi info box exited. If user pressed lower button, ok, then go back. If user pressed upper button, go to 
// password entry screen
static void WifiScan_GuiTextboxExitedCallback(bool ok)
{
	printf("WifiScan GuiTextboxExited %d\n", ok);
	wifiScanContext.wifiInfoTextbox.inFocus = false;
	if (ok)
	{
		// Going back to wifi list
		wifiScanContext.wifiListbox.inFocus = true;
	}
	else
	{
		// Going to pw input box
		wifiScanContext.wifiPwInputBox.inFocus = true;
	}
}

// Wifi connecting notice text box exited callback
static void WifiScan_GuiConnectStatusTextboxExitedCallback(bool ok)
{
	wifiScanContext.wifiListbox.inFocus = true;
}

// Password for AP entered
static void WifiScan_GuiInputBoxExitedCallback(void)
{
	WifiAccessPoint_s* r = Wifi_GetScanRecordByIdx(wifiScanContext.wifiListbox.cursor);

	// Display our "Connecting.." text box
	snprintf(wifiScanContext.messageBuffer, WIFI_SCAN_INFO_STR_LEN, "[*] Connecting to:\n%s\n pw:\n%s", r->ssid, wifiScanContext.wifiPwInputBox.buf);

	char tmpbuf[wifiScanContext.wifiPwInputBox.inputLen];
	Wifi_RequestConnect(r->ssid, wifiScanContext.wifiPwInputBox.buf);
	GuiTextInput_ClearBuf(&wifiScanContext.wifiPwInputBox);

	// Focus handling
	wifiScanContext.connecting = true;
	GuiTextbox_SetInFocus(&(wifiScanContext.wifiConnectStatusTextbox), true);
	GuiTextbox_SetInFocus(&(wifiScanContext.wifiInfoTextbox), false);
	wifiScanContext.wifiPwInputBox.inFocus = false;
	wifiScanContext.wifiListbox.inFocus = false;
}

bool WifiScan_Init(void *arg)
{
	wifiScanContext.cursor = 0;
	wifiScanContext.ssidSelectionIdx = WIFI_SCAN_NO_SSID_SELECTED;
	Wifi_UnsetCurrentRoutine();
	Wifi_SetMode(WIFI_MODE_STATION);
	Wifi_Scan();

	wifiScanContext.tsSinceLastScan = get_absolute_time();
	wifiScanContext.knownNumScanRecords = 0;

	wifiScanContext.wifiListbox = GuiList_Create(NULL, \
																							0, \
																							WifiScan_GuiListItemSelectedCallback, \
																							WifiScan_GuiListExitedCallback);

	wifiScanContext.wifiInfoTextbox = GuiTextbox_Create(wifiScanContext.messageBuffer, \
																										 0, \
																										 WifiScan_GuiTextboxExitedCallback);

	wifiScanContext.wifiConnectStatusTextbox = GuiTextbox_Create(wifiScanContext.messageBuffer, \
																										 0, \
																										 WifiScan_GuiConnectStatusTextboxExitedCallback);

	wifiScanContext.wifiPwInputBox = GuiTextInput_Create(WifiScan_GuiInputBoxExitedCallback);

	GuiTextInput_SetInfoText(&wifiScanContext.wifiPwInputBox, "Password:");
	memset(wifiScanContext.messageBuffer, 0x00, WIFI_SCAN_INFO_STR_LEN);
	wifiScanContext.connecting = false;
	wifiScanContext.wifiListbox.inFocus = true;
}

void WifiScan_Deinit(void)
{
}

void WifiScan_Update(void)
{
	// If we're currently connecting, display connecting status
	if (wifiScanContext.wifiConnectStatusTextbox.inFocus)
	{
		// If the connecting status textbox is on, it means we're connecting.
		if (Wifi_IsConnected()) // TODO handle connection failure
		{
			// Ask wifi if we're connected.
			if (wifiScanContext.connecting)
			{
				// If the scanner gui just finds out we've connected, update the message
				snprintf(wifiScanContext.messageBuffer, WIFI_SCAN_INFO_STR_LEN, "[*] Connected!");
				wifiScanContext.connecting = false;
			}
			// If we've connected, you can start checking for timeouts in the status textbox 
			GuiTextbox_Update(&(wifiScanContext.wifiConnectStatusTextbox));
		}
		else
		{
			// If we've not connected, keep resetting the timer of the textbox. kinda lame of a solution but whatev
			GuiTextbox_SetTimer(&(wifiScanContext.wifiConnectStatusTextbox), 2000);
		}
		return;
	}
	
	// Update our list periodically
	uint16_t numScanRecords = Wifi_GetNumScanRecords();
	if (wifiScanContext.knownNumScanRecords != numScanRecords)
	{
		// printf("WIFI SCAN GUI CURSOR %d CURR NUMRECODS %d NEW NUMRECORDS %d\n",	wifiScanContext.wifiListbox.cursor, wifiScanContext.knownNumScanRecords, numScanRecords);
		GuiList_SetStrings(&wifiScanContext.wifiListbox, Wifi_GetStringsList(), numScanRecords);
		wifiScanContext.knownNumScanRecords = numScanRecords;
	}
}

void WifiScan_Draw(void)
{
	OledMan_ClearBuf();
	if (wifiScanContext.wifiConnectStatusTextbox.inFocus)
	{
		GuiTextbox_Draw(&wifiScanContext.wifiConnectStatusTextbox);
	}
	else if (wifiScanContext.wifiListbox.inFocus)
	{
		GuiList_Draw(&wifiScanContext.wifiListbox);
	}
	else if (wifiScanContext.wifiInfoTextbox.inFocus)
	{
		GuiTextbox_Draw(&wifiScanContext.wifiInfoTextbox);
	}
	else if (wifiScanContext.wifiPwInputBox.inFocus)
	{
		GuiTextInput_Draw(&wifiScanContext.wifiPwInputBox);
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

	GuiItemActions_e guiItemAction = GUI_ITEM_ACTION_MAX;
	if (wifiScanContext.wifiListbox.inFocus) // TODO bad lookin
	{
		GuiList_TakeActionInput(&(wifiScanContext.wifiListbox), GuiList_DefaultButtonMap(b, g));
	}
	else if (wifiScanContext.wifiInfoTextbox.inFocus)
	{
		GuiTextbox_TakeActionInput(&(wifiScanContext.wifiInfoTextbox), GuiTextbox_DefaultButtonMap(b, g));
	}
	else if (wifiScanContext.wifiPwInputBox.inFocus)
	{
		GuiTextInput_TakeActionInput(&(wifiScanContext.wifiPwInputBox), GuiTextInput_DefaultButtonMap(b, g));
	}
}
