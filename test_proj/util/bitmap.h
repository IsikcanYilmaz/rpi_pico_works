#include <stdint.h>
#include <stdbool.h>

typedef struct Bitmap_s
{
	uint8_t *buf;
	uint16_t width;
	uint16_t height;
} Bitmap_t;

uint8_t Bitmap_GetPixel(Bitmap_t *bitmap, uint16_t x, uint16_t y);
void Bitmap_SetPixel(Bitmap_t *bitmap, uint16_t x, uint16_t y, uint8_t val);
void Bitmap_Clear(Bitmap_t *bitmap);
