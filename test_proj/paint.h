#ifndef __PAINT_H_
#define __PAINT_H_
#include <stdint.h>
#include <stdbool.h>
#include "oled_lib/Fonts/fonts.h"

#define PAINT_DEFAULT_FONT (&Font8)
#define PAINT_SCALE_ENABLED true
#define PAINT_OLED_SCALE 2
#define PAINT_DEFAULT_COLOR (WHITE)

typedef enum PaintColor_e_
{
	BLACK,
	WHITE,
	COLOR_MAX
} PaintColor_e;

typedef struct PaintContext_e_
{
	uint8_t *buf;
	uint16_t width;
	uint16_t height;
	uint16_t widthByte;
	uint16_t heightByte;
	PaintColor_e color;
	sFONT *font;
} PaintContext_e;

// Basic
void Paint_Init(uint8_t *buf, uint16_t w, uint16_t h);
void Paint_SetPixel(uint16_t x, uint16_t y, uint8_t val);
void Paint_Clear(void);
void Paint_SetFont(sFONT *font);
void Paint_SetColor(PaintColor_e c);

// Geometry
void Paint_DrawLine(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2);
void Paint_DrawRect(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2, bool fill);
void Paint_DrawCircle(uint16_t xCenter, uint16_t yCenter, uint16_t r, bool fill);

// Strings
void Paint_DrawChar(uint16_t x, uint16_t y, char c);
void Paint_DrawString(uint16_t x, uint16_t y, char *str);



#endif
