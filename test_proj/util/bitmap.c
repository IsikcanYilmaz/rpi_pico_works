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
*                      maxX, maxY
*/

static inline uint32_t Bitmap_GetPixIdx(Bitmap_t *b, uint16_t x, uint16_t y)
{
	return (b->width * y) + x;
}

static inline uint32_t Bitmap_GetByteIdx(Bitmap_t *b, uint16_t x, uint16_t y)
{
	return Bitmap_GetPixIdx(b, x, y) / 8;
}

static inline uint32_t Bitmap_GetBitIdx(Bitmap_t *b, uint16_t x, uint16_t y)
{
	return Bitmap_GetPixIdx(b, x, y) % 8;
}

uint8_t Bitmap_GetPixel(Bitmap_t *bitmap, uint16_t x, uint16_t y)
{
	uint32_t byteIdx = Bitmap_GetByteIdx(bitmap, x, y);
	uint32_t bitIdx = Bitmap_GetBitIdx(bitmap, x, y);
	return (bitmap->buf[byteIdx] & (1 << bitIdx)) > 0 ? 1 : 0;
}

void Bitmap_SetPixel(Bitmap_t *bitmap, uint16_t x, uint16_t y, uint8_t val)
{
	uint32_t byteIdx = Bitmap_GetByteIdx(bitmap, x, y);
	uint32_t bitIdx = Bitmap_GetBitIdx(bitmap, x, y);
	bitmap->buf[byteIdx] = (val) ? (bitmap->buf[byteIdx] | (1 << bitIdx)) : (bitmap->buf[byteIdx] & ~(1 << bitIdx));
}

void Bitmap_Clear(Bitmap_t *bitmap)
{
	memset(bitmap->buf, 0x00, (bitmap->width * bitmap->height / 8));
}

#if 0
void main(int argc, char **argv) // tester
{
	uint8_t buf[64*64/8];
	memset(buf, 0x00, sizeof(buf));
	printf("\n\n");
	Bitmap_t bitmap = (Bitmap_t) {.buf = buf, .width = 64, .height = 64};
	Bitmap_SetPixel(&bitmap, 0, 0, 1);
	Bitmap_SetPixel(&bitmap, 2, 0, 1);
	Bitmap_SetPixel(&bitmap, 4, 0, 1);
	Bitmap_SetPixel(&bitmap, 6, 0, 1);
	// Bitmap_SetPixel(&bitmap, 0, 0, 1);
	// Bitmap_SetPixel(&bitmap, 5, 3, 1);
	for (int i = 0; i < bitmap.height; i++)
	{
		printf("%d ", i);
		for (int j = 0; j < bitmap.width; j++)
		{
			uint8_t v = Bitmap_GetPixel(&bitmap,j,i);
			printf("%s", (v > 0 ? "." : "_"));
		}
		printf("\n");
	}
}
#endif 
