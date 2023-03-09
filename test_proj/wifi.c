#include "wifi.h"
#include "pico/stdlib.h"
#include "pico/cyw43_arch.h"

static int scan_result(void *env, const cyw43_ev_scan_result_t *result) {
	if (result) {
		printf("ssid: %-32s rssi: %4d chan: %3d mac: %02x:%02x:%02x:%02x:%02x:%02x sec: %u\n",
				 result->ssid, result->rssi, result->channel,
				 result->bssid[0], result->bssid[1], result->bssid[2], result->bssid[3], result->bssid[4], result->bssid[5],
				 result->auth_mode);
	}
	return 0;
}

void Wifi_Init(void)
{
	cyw43_arch_enable_sta_mode();
}

void Wifi_Scan(void)
{
	cyw43_wifi_scan_options_t scan_options = {0};
	int err = cyw43_wifi_scan(&cyw43_state, &scan_options, NULL, scan_result);
	if (err == 0)
		{
			printf("Performing wifi scan\n");
		}
	else
		{
		printf("Failed to start scan: %d\n", err);
	}
}
