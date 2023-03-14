#include "bitmap.h"
#include <string.h>
#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>

/*
* Bitmap utility
*
* 0,0 ->
* | --------------------
* v |                  |
*   |                  |
*   |                  |
*   --------------------
*  										maxX, maxY
*/

static uint16_t Bitmap_GetPixIdx(Bitmap_t *b, uint16_t x, uint16_t y)
{
	return (b->width * y) + x;
}

static uint16_t Bitmap_GetByteIdx(Bitmap_t *b, uint16_t x, uint16_t y)
{
	return Bitmap_GetPixIdx(b, x, y) / 8;
}

static uint16_t Bitmap_GetBitIdx(Bitmap_t *b, uint16_t x, uint16_t y)
{
	return Bitmap_GetPixIdx(b, x, y) % 8;
}

uint8_t Bitmap_GetPixel(Bitmap_t *bitmap, uint16_t x, uint16_t y)
{
	uint8_t byteIdx = Bitmap_GetByteIdx(bitmap, x, y);
	uint8_t bitIdx = Bitmap_GetBitIdx(bitmap, x, y);
	return (bitmap->buf[byteIdx] & (1 << bitIdx)) > 0 ? 1 : 0;
}

void Bitmap_SetPixel(Bitmap_t *bitmap, uint16_t x, uint16_t y, uint8_t val)
{
	uint8_t byteIdx = Bitmap_GetByteIdx(bitmap, x, y);
	uint8_t bitIdx = Bitmap_GetBitIdx(bitmap, x, y);
	bitmap->buf[byteIdx] = (val) ? (bitmap->buf[byteIdx] | (1 << bitIdx)) : (bitmap->buf[byteIdx] & ~(1 << bitIdx));
}

#if 0
void main(void) // tester
{
	uint8_t buf[3];
	memset(buf, 0x00, sizeof(buf));
	for (int i = 0; i < sizeof(buf); i++)
	{
		printf("0x%x ", buf[i]);
	}
	printf("\n\n");
	Bitmap_t bitmap = (Bitmap_t) {.buf = buf, .width = 6, .height = 4};
	Bitmap_SetPixel(&bitmap, 1, 0, 1);
	Bitmap_SetPixel(&bitmap, 0, 0, 1);
	Bitmap_SetPixel(&bitmap, 5, 3, 1);
	for (int i = 0; i < bitmap.height; i++)
	{
		for (int j = 0; j < bitmap.width; j++)
		{
			uint8_t v = Bitmap_GetPixel(&bitmap,j,i);
			printf("%s", (v > 0 ? "." : "_"));
		}
		printf("\n");
	}
}
#endif 
