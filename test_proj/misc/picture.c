#include "picture.h"
#include "oled_manager.h"
#include "bitmap.h"

extern char img;
Bitmap_t imgBitmap;

bool Picture_Init(void *arg)
{
	imgBitmap.width = 128;
	imgBitmap.height = 64;
	imgBitmap.buf = &img;
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
	for (uint16_t i = 0; i < 128; i++)
	{
		for(uint16_t j = 0; j < 64; j++)
		{
			OledMan_SetPixel(i, j, Bitmap_GetPixel(&imgBitmap, i, j));
			// OledMan_SetPixel(i, j, 1);
		}
	}
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
