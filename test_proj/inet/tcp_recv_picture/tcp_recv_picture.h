#include <stdbool.h>

#define TCP_RECV_PICTURE_UPDATE_PERIOD_MS 200

typedef struct TcpRecvInitArgs_s_
{
	char *ipAddr;
	uint16_t port;
	void (*pictureReceivedCallback)(void);
} TcpRecvInitArgs_s;

bool TcpRecvPicture_Init(void *arg);
bool TcpRecvPicture_Deinit(void);
bool TcpRecvPicture_Update(void);
