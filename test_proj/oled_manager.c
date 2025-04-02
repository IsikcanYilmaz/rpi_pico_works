#include "oled_manager.h"
#include "paint.h"
#include "hardware/timer.h"
#include <stdlib.h>
#include <string.h>

// High level management code for the Pico-OLED-1.3 module
// has a 1.3 inch oled screen and two buttons on it.

OledManContext_t oledManContext = {
	.initialized = false,
	.pixelBufUpdated = false,
	.font = OLED_DEFAULT_FONT,
	.blocked = false,
};

static bool OledMan_UpdateTimerCallback(struct repeating_timer_t *t)
{
	if (oledManContext.pixelBufUpdated)
	{
		OledMan_DrawBuf();
		oledManContext.pixelBufUpdated = false;
	}
	return true;
}

void OledMan_StartPollTimer(void)
{
	add_repeating_timer_ms(OLED_MAN_POLL_PERIOD_MS, (repeating_timer_callback_t) OledMan_UpdateTimerCallback, NULL, &(oledManContext.oledManUpdateTimer));
}

void OledMan_StopPollTimer(void)
{
	cancel_repeating_timer(&(oledManContext.oledManUpdateTimer));
}

void OledMan_Init(void)
{
	uint8_t ret = DEV_Module_Init();
	if(ret != 0)
	{
		printf("Failed to initialize OLED! %d\n", ret); // TODO looking at the driver it seems like this only returns 0
	}
	else
	{
		OLED_1in3_C_Init();
		OLED_1in3_C_Clear();
		OledMan_ClearBuf();
		// Paint_NewImage(oledManContext.pixelBuf, OLED_WIDTH, OLED_HEIGHT, 0, BLACK);
		Paint_Init(oledManContext.pixelBuf, OLED_WIDTH, OLED_HEIGHT);
		OledMan_StartPollTimer();
		oledManContext.initialized = true;
	}
}

void OledMan_DrawBuf(void)
{
	OLED_1in3_C_Display(oledManContext.pixelBuf);
}

void OledMan_ClearBuf(void)
{
	memset(oledManContext.pixelBuf, 0x00, OLED_PIXEL_BUF_LEN);
	oledManContext.pixelBufUpdated = true;
}

void OledMan_SetFont(sFONT *font)
{
	oledManContext.font = font;
}

// WRAPPERS
void OledMan_DrawChar(uint16_t x, uint16_t y, char c)
{
	// Paint_DrawChar(x, y, c, oledManContext.font, WHITE, BLACK);
	Paint_DrawChar(x, y, c);
	oledManContext.pixelBufUpdated = true;
}

void OledMan_DrawCircle(uint16_t xCenter, uint16_t yCenter, uint16_t radius, bool fill)
{
	Paint_DrawCircle(xCenter, yCenter, radius, fill);
	oledManContext.pixelBufUpdated = true;
}

void OledMan_DrawString(uint16_t x, uint16_t y, char *str)
{
	Paint_DrawString(x, y, str);
	oledManContext.pixelBufUpdated = true;
}

void OledMan_DrawLine(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2)
{
	Paint_DrawLine(x1, y1, x2, y2);
	oledManContext.pixelBufUpdated = true;
}

void OledMan_DrawRectangle(uint16_t x, uint16_t y, uint16_t w, uint16_t h, bool fill)
{
	uint16_t x2 = x + w;
	uint16_t y2 = y + h;
	OledMan_DrawRectangleAbsolute(x, y, x2, y2, fill);
}

void OledMan_DrawRectangleAbsolute(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2, bool fill)
{
	// Paint_DrawRectangle(x1, y1, x2, y2, WHITE, DOT_PIXEL_1X1, (fill ? DRAW_FILL_FULL : DRAW_FILL_EMPTY));
	Paint_DrawRect(x1, y1, x2, y2, fill);
	oledManContext.pixelBufUpdated = true;
}

void OledMan_SetPixel(uint16_t x, uint16_t y, uint8_t set)
{
	Paint_SetPixel(x, y, set);
	oledManContext.pixelBufUpdated = true;
}

void OledMan_SetColor(PaintColor_e c)
{
	Paint_SetColor(c);
}
