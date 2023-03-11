#include <stdlib.h>
#include <stdbool.h>
#define WIFI_SCAN_POLL_PERIOD_MS 10
#define WIFI_CONNECT_TIMEOUT_MS 10000 

typedef struct WifiContext_s
{
	bool isConnected;
	char *ssid;
} WifiContext_t;

void Wifi_Init(void);
void Wifi_Scan(void);
void Wifi_PollTimerStart(void);
void Wifi_PollTimerStop(void);
bool Wifi_Connect(char *ssid, char *pass);

