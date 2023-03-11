#include "wifi.h"
#include "pico/stdlib.h"
#include "pico/cyw43_arch.h"
#include "hardware/timer.h"

static struct repeating_timer wifiPollTimer;

static const char *authModeStrings[] = {
	[0] = "WEP_PSK",
	[1] = "WPA",
	[2] = "WPA2",
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

static int Wifi_ScanResult(void *env, const cyw43_ev_scan_result_t *result) {
	if (result) {
		printf("ssid: %-32s rssi: %4d chan: %3d mac: %02x:%02x:%02x:%02x:%02x:%02x sec: 0x%x ",
				 result->ssid, result->rssi, result->channel,
				 result->bssid[0], result->bssid[1], result->bssid[2], result->bssid[3], result->bssid[4], result->bssid[5],
				 result->auth_mode);
		Wifi_PrintAuthMode(result->auth_mode);
		printf("\n");
	}
	return 0;
}

static bool Wifi_Poll(struct repeating_timer *t)
{
	cyw43_arch_poll();
}

void Wifi_Init(void)
{
	cyw43_arch_enable_sta_mode();
	Wifi_PollTimerStart();
}

void Wifi_Scan(void)
{
	cyw43_wifi_scan_options_t scan_options = {0};
	int err = cyw43_wifi_scan(&cyw43_state, &scan_options, NULL, Wifi_ScanResult);
	if (err == 0)
	{
		printf("Performing wifi scan\n");
	}
	else
	{
		printf("Failed to start scan: %d\n", err);
	}
}

void Wifi_PollTimerStart(void)
{
	add_repeating_timer_ms(WIFI_SCAN_POLL_PERIOD_MS, Wifi_Poll, NULL, &wifiPollTimer);
}

void Wifi_PollTimerStop(void)
{
	cancel_repeating_timer(&wifiPollTimer);
}

bool Wifi_Connect(char *ssid, char *pass)
{
	printf("Attempting to connect to AP SSID:%s\n", ssid);
	if (cyw43_arch_wifi_connect_timeout_ms(ssid, pass, CYW43_AUTH_WPA2_AES_PSK, WIFI_CONNECT_TIMEOUT_MS)) 
	{
		printf("Failed to connect!\n");
		return false;
	}
	else
	{
		printf("Connected!\n");
		return true;
	}
}
