#include "paint.h"
#include <stdio.h>
#include <string.h>

PaintContext_e paintContext;

#define ASSERT_COORDS(x,y) {if (!Paint_CheckValidCoords(x,y)) { /*printf("Bad coords %d %d\n", x, y); */ return; }}

static inline bool Paint_CheckValidCoords(uint16_t x, uint16_t y)
{
	return (x < paintContext.width && y < paintContext.height);
}

static uint16_t Paint_CoordToByteIdx(uint16_t x, uint16_t y)
{
	uint16_t addr = 0;
	#if PAINT_SCALE_ENABLED
	addr = x/8 + y*paintContext.widthByte;
	#endif
	return addr;
}

// Basic
void Paint_Init(uint8_t *buf, uint16_t w, uint16_t h)
{
	paintContext = (PaintContext_e)
									{.buf = buf, 
									 .width = w, 
									 .height = h, 
									 .widthByte = (w % 8 == 0) ? (w/8) : (w/8+1),
									 .heightByte = 0,
									 .font = PAINT_DEFAULT_FONT};
	Paint_Clear();
}

void Paint_SetColor(PaintColor_e c)
{
	if (c <= COLOR_MAX)
	{
		paintContext.color = c;
	}
}

void Paint_SetPixel(uint16_t x, uint16_t y, uint8_t val)
{
	ASSERT_COORDS(x,y);
	uint16_t addr = Paint_CoordToByteIdx(x,y);
	uint8_t data = paintContext.buf[addr];
	#if PAINT_SCALE_ENABLED
	if (val)
	{
		data = data | (0x80 >> (x%8));
	}
	else
	{
		data = data & ~(0x80 >> (x%8));
	}
	#else
	data = val;
	#endif
	paintContext.buf[addr] = data;
}

void Paint_Clear(void)
{
	memset(paintContext.buf, 0x00, sizeof(paintContext.width * paintContext.height));
}

void Paint_SetFont(sFONT *font)
{
	paintContext.font = font;
}

// Geometry
void Paint_DrawLine(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2)
{
	// ASSERT_COORDS(x1, y1);
	// ASSERT_COORDS(x2, y2);

	// Gotten/modded the pico oled driver
	uint16_t Xpoint = x1;
	uint16_t Ypoint = y1;

	int dx = (int)x2 - (int)x1 >= 0 ? x2 - x1 : x1 - x2;
	int dy = (int)y2 - (int)y1 <= 0 ? y2 - y1 : y1 - y2;

	// Increment direction, 1 is positive, -1 is counter;
	int XAddway = x1 < x2 ? 1 : -1;
	int YAddway = y1 < y2 ? 1 : -1;

	//Cumulative error
	int Esp = dx + dy;
	// char Dotted_Len = 0;
	uint8_t val = (paintContext.color == WHITE ? 1 : 0);
	for (;;) {
		Paint_SetPixel(Xpoint, Ypoint, val);
		if (2 * Esp >= dy) {
			if (Xpoint == x2)
				break;
			Esp += dy;
			Xpoint += XAddway;
		}
		if (2 * Esp <= dx) {
			if (Ypoint == y2)
				break;
			Esp += dx;
			Ypoint += YAddway;
		}
	}
}

void Paint_DrawRect(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2, bool fill)
{
	// ASSERT_COORDS(x1, y1);
	// ASSERT_COORDS(x2, y2);

	// Gotten/modded the pico oled driver
	if (fill) {
		for(uint16_t Ypoint = y1; Ypoint < y2; Ypoint++) {
			Paint_DrawLine(x1, Ypoint, x2, Ypoint);
		}
	} else {
		Paint_DrawLine(x1, y1, x2, y1);
		Paint_DrawLine(x1, y1, x1, y2);
		Paint_DrawLine(x2, y2, x2, y1);
		Paint_DrawLine(x2, y2, x1, y2);
	}
}

void Paint_DrawCircle(uint16_t xCenter, uint16_t yCenter, uint16_t r, bool fill)
{
	// ASSERT_COORDS(xCenter, yCenter);

	// Gotten/modded the pico oled driver
	//Draw a circle from(0, R) as a starting point
	int16_t XCurrent, YCurrent;
	XCurrent = 0;
	YCurrent = r;

	//Cumulative error,judge the next point of the logo
	int16_t Esp = 3 - (r << 1 );

	int16_t sCountY;
	uint8_t val = (paintContext.color == WHITE ? 1 : 0);
	if (fill) 
	{
		while (XCurrent <= YCurrent ) //Realistic circles
		{ 
			for (sCountY = XCurrent; sCountY <= YCurrent; sCountY ++ ) 
			{
				Paint_SetPixel(xCenter + XCurrent, yCenter + sCountY, val);//1
				Paint_SetPixel(xCenter - XCurrent, yCenter + sCountY, val);//2
				Paint_SetPixel(xCenter - sCountY, yCenter + XCurrent, val);//3
				Paint_SetPixel(xCenter - sCountY, yCenter - XCurrent, val);//4
				Paint_SetPixel(xCenter - XCurrent, yCenter - sCountY, val);//5
				Paint_SetPixel(xCenter + XCurrent, yCenter - sCountY, val);//6
				Paint_SetPixel(xCenter + sCountY, yCenter - XCurrent, val);//7
				Paint_SetPixel(xCenter + sCountY, yCenter + XCurrent, val);
			}
			if (Esp < 0 )
				Esp += 4 * XCurrent + 6;
			else {
				Esp += 10 + 4 * (XCurrent - YCurrent);
				YCurrent --;
			}
			XCurrent ++;
		}
	} 
	else  //Draw a hollow circle
	{ 
		while (XCurrent <= YCurrent ) 
		{
			Paint_SetPixel(xCenter + XCurrent, yCenter + YCurrent, val);//1
			Paint_SetPixel(xCenter - XCurrent, yCenter + YCurrent, val);//2
			Paint_SetPixel(xCenter - YCurrent, yCenter + XCurrent, val);//3
			Paint_SetPixel(xCenter - YCurrent, yCenter - XCurrent, val);//4
			Paint_SetPixel(xCenter - XCurrent, yCenter - YCurrent, val);//5
			Paint_SetPixel(xCenter + XCurrent, yCenter - YCurrent, val);//6
			Paint_SetPixel(xCenter + YCurrent, yCenter - XCurrent, val);//7
			Paint_SetPixel(xCenter + YCurrent, yCenter + XCurrent, val);//0

			if (Esp < 0 )
			{
				Esp += 4 * XCurrent + 6;
			}
			else 
			{
				Esp += 10 + 4 * (XCurrent - YCurrent );
				YCurrent --;
			}
			XCurrent ++;
		}
	}
}

// Strings
void Paint_DrawChar(uint16_t x, uint16_t y, char c)
{
	// ASSERT_COORDS(x, y);
	uint16_t Page, Column;

	uint32_t Char_Offset = (c - ' ') * paintContext.font->Height * (paintContext.font->Width / 8 + (paintContext.font->Width % 8 ? 1 : 0));
	const unsigned char *ptr = &paintContext.font->table[Char_Offset];

	uint8_t val = (paintContext.color == WHITE ? 1 : 0);
	for (Page = 0; Page < paintContext.font->Height; Page ++ ) {
		for (Column = 0; Column < paintContext.font->Width; Column ++ ) {

			//To determine whether the font background color and screen background color is consistent
			// if (FONT_BACKGROUND == Color_Background) { //this process is to speed up the scan
			// 	if (*ptr & (0x80 >> (Column % 8)))
			// 		Paint_SetPixel(Xpoint + Column, Ypoint + Page, Color_Foreground);
			// 	// Paint_DrawPoint(Xpoint + Column, Ypoint + Page, Color_Foreground, DOT_PIXEL_DFT, DOT_STYLE_DFT);
			// } else {
			// 	if (*ptr & (0x80 >> (Column % 8))) {
			// 		Paint_SetPixel(Xpoint + Column, Ypoint + Page, Color_Foreground);
			// 		// Paint_DrawPoint(Xpoint + Column, Ypoint + Page, Color_Foreground, DOT_PIXEL_DFT, DOT_STYLE_DFT);
			// 	} else {
			// 		Paint_SetPixel(Xpoint + Column, Ypoint + Page, Color_Background);
			// 		// Paint_DrawPoint(Xpoint + Column, Ypoint + Page, Color_Background, DOT_PIXEL_DFT, DOT_STYLE_DFT);
			// 	}
			// }
			if (*ptr & (0x80 >> (Column % 8)))
			{
				Paint_SetPixel(x + Column, y + Page, val);
			}
			//One pixel is 8 bits
			if (Column % 8 == 7)
				ptr++;
		}// Write a line
		if (paintContext.font->Width % 8 != 0)
			ptr++;
	}// Write all
}

void Paint_DrawString(uint16_t x, uint16_t y, char *str)
{
	// ASSERT_COORDS(x,y);
	
	uint16_t Xpoint = x;
	uint16_t Ypoint = y;

	while (*str != '\0') {
		//if X direction filled , reposition to(Xstart,Ypoint),Ypoint is Y direction plus the Height of the character
		if ((Xpoint + paintContext.font->Width ) > paintContext.width ) {
			Xpoint = x;
			Ypoint += paintContext.font->Height;
		}	

		// If the Y direction is full, reposition to(Xstart, Ystart)
		if ((Ypoint  + paintContext.font->Height ) > paintContext.height ) {
			Xpoint = x;
			Ypoint = y;
		}
		
		//do this also if the char is a newline
		if (*str == '\n')
		{
			Xpoint = x;
			Ypoint += paintContext.font->Height;
		}
		else
		{
			Paint_DrawChar(Xpoint, Ypoint, *str);
			
			//The next word of the abscissa increases the font of the broadband
			Xpoint += paintContext.font->Width;
		}

		//The next character of the address
		str++;
	}
}

