#include "picture.h"
#include "oled_manager.h"
#include "bitmap.h"
#include "wifi.h"
#include "tcp_recv_picture/tcp_recv_picture.h"

#define TCP_RECV_ENABLED 1

extern char picture_buffer;
Bitmap_t imgBitmap;
bool pictureUpdateHappened;

static void Picture_BufferFilled(void)
{
	printf("Picture tcp buffer filled!\n");
	pictureUpdateHappened = true;
}

bool Picture_Init(void *arg)
{
	if (TCP_RECV_ENABLED)
	{
		TcpRecvInitArgs_s tcpArgs = {.ipAddr = "192.168.50.222", .port = 9999, .pictureReceivedCallback = Picture_BufferFilled};
		if (!Wifi_SetRoutine(WIFI_ROUTINE_TCP_RECV_PICTURE, &tcpArgs))
		{
			printf("Picture program couldnt run wifi routine TCP_RECV_PICTURE\n");
		}
	}
	pictureUpdateHappened = true;
	imgBitmap.width = 128;
	imgBitmap.height = 64;
	imgBitmap.buf = &picture_buffer;
	return true;
}
void Picture_Deinit(void)
{
}
void Picture_Update(void)
{
}
void Picture_Draw(void)
{
	if (!pictureUpdateHappened)
	{
		return;
	}
	for (uint16_t i = 0; i < 128; i++)
	{
		for(uint16_t j = 0; j < 64; j++)
		{
			OledMan_SetPixel(i, j, Bitmap_GetPixel(&imgBitmap, i, j));
			// OledMan_SetPixel(i, j, 1);
		}
	}
	pictureUpdateHappened = false;
}
void Picture_Start(void)
{
}
void Picture_Stop(void)
{
}
void Picture_ButtonInput(Button_e b, ButtonGesture_e g)
{
}

void Picture_SetUpdateHappened(void)
{
	pictureUpdateHappened = true;
}
