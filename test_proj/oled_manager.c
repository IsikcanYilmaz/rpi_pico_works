#include "oled_manager.h"
#include "GUI_Paint.h"
#include "hardware/timer.h"
#include <stdlib.h>
#include <string.h>

// High level management code for the Pico-OLED-1.3 module
// has a 1.3 inch oled screen and two buttons on it.

OledManContext_t oledManContext = {
	.initialized = false,
	.pixelBufUpdated = false,
	.font = DEFAULT_FONT,
	.blocked = false,
};

static int OLED_1in3_C_test(void)
{
    DEV_Delay_ms(100);
    
    // UBYTE *BlackImage;
    // UWORD Imagesize = ((OLED_1in3_C_WIDTH%8==0)? (OLED_1in3_C_WIDTH/8): (OLED_1in3_C_WIDTH/8+1)) * OLED_1in3_C_HEIGHT;
    // if((BlackImage = (UBYTE *)malloc(Imagesize)) == NULL) {
        // while(1){
            // printf("Failed to apply for black memory...\r\n");
        // }
    // }
		UBYTE *BlackImage = oledManContext.pixelBuf;
    printf("Paint_NewImage\r\n");
    Paint_NewImage(BlackImage, OLED_1in3_C_WIDTH, OLED_1in3_C_HEIGHT, 0, WHITE);	
    
    printf("Drawing\r\n");
    //1.Select Image
    Paint_SelectImage(BlackImage);
    DEV_Delay_ms(500);
    Paint_Clear(BLACK);
    
    // 2.Drawing on the image
    printf("Drawing:page 1\r\n");
    Paint_DrawPoint(20, 10, WHITE, DOT_PIXEL_1X1, DOT_STYLE_DFT);
    Paint_DrawPoint(30, 10, WHITE, DOT_PIXEL_2X2, DOT_STYLE_DFT);
    Paint_DrawPoint(40, 10, WHITE, DOT_PIXEL_3X3, DOT_STYLE_DFT);
    Paint_DrawLine(10, 10, 10, 20, WHITE, DOT_PIXEL_1X1, LINE_STYLE_SOLID);
    Paint_DrawLine(20, 20, 20, 30, WHITE, DOT_PIXEL_1X1, LINE_STYLE_SOLID);
    Paint_DrawLine(30, 30, 30, 40, WHITE, DOT_PIXEL_1X1, LINE_STYLE_DOTTED);
    Paint_DrawLine(40, 40, 40, 50, WHITE, DOT_PIXEL_1X1, LINE_STYLE_DOTTED);
    Paint_DrawCircle(60, 30, 15, WHITE, DOT_PIXEL_1X1, DRAW_FILL_EMPTY);
    Paint_DrawCircle(100, 40, 20, WHITE, DOT_PIXEL_1X1, DRAW_FILL_FULL);
    Paint_DrawRectangle(50, 30, 60, 40, WHITE, DOT_PIXEL_1X1, DRAW_FILL_EMPTY);
    Paint_DrawRectangle(90, 30, 110, 50, BLACK, DOT_PIXEL_1X1, DRAW_FILL_FULL);
    BlackImage[0] = 0xf0;
    // 3.Show image on page1
    OLED_1in3_C_Display(BlackImage);
    DEV_Delay_ms(2000);
    Paint_Clear(BLACK);
    
    // Drawing on the image
    printf("Drawing:page 2\r\n");
    Paint_DrawString_EN(10, 0, "Pico-OLED", &Font16, WHITE, BLACK);
    Paint_DrawString_EN(10, 17, "hello world", &Font8, WHITE, BLACK);
    Paint_DrawNum(10, 30, 123.1, &Font8,2, WHITE, WHITE);
    Paint_DrawNum(10, 43, 987654.2, &Font12,2, WHITE, WHITE);
    // Show image on page2
    OLED_1in3_C_Display(BlackImage);
    DEV_Delay_ms(2000);
    Paint_Clear(BLACK);
    
    // Drawing on the image
    printf("Drawing:page 3\r\n");
    Paint_DrawString_CN(10, 0,"ÄãºÃAbc", &Font12CN, WHITE, WHITE);
    Paint_DrawString_CN(0, 20, "µç×Ó", &Font24CN, WHITE, WHITE);
    // Show image on page3
    OLED_1in3_C_Display(BlackImage);
    DEV_Delay_ms(2000);
    
    // Show image on page4
    // OLED_1in3_C_Display(gImage_1inch3_C_1);
		// OLED_1in3_C_Display(Alarm88);
		// Paint_DrawImage(Alarm88, 0, 0, 2, 4);
    DEV_Delay_ms(5000);
    
    
    // Paint_NewImage(BlackImage, OLED_1in3_C_WIDTH, OLED_1in3_C_HEIGHT, 180, WHITE);	
    // Paint_Clear(BLACK);
    // int key0 = 15; 
    // int key1 = 17;
    // int key = 0;
    // DEV_GPIO_Mode(key0, 0);
    // DEV_GPIO_Mode(key1, 0);
    // 
    // Paint_Clear(BLACK);
    // OLED_1in3_C_Display(BlackImage);
    
    
    // while(1){
    //     if(DEV_Digital_Read(key1 ) == 0){
    //         Paint_DrawRectangle(115, 5, 125, 15, WHITE, DOT_PIXEL_2X2,DRAW_FILL_FULL);
    //         key = 1;
    //     }else {
    //         Paint_DrawRectangle(115, 5, 125, 15, BLACK, DOT_PIXEL_2X2,DRAW_FILL_EMPTY);
    //     }
    //         
    //     if(DEV_Digital_Read(key0 ) == 0){
    //         Paint_DrawRectangle(115, 50, 125, 60, WHITE, DOT_PIXEL_2X2,DRAW_FILL_FULL);
    //     }else {
    //         Paint_DrawRectangle(115, 50, 125, 60, BLACK, DOT_PIXEL_2X2,DRAW_FILL_EMPTY);
    //         key = 1;
    //         
    //     }
    //     if(key == 1){
    //         OLED_1in3_C_Display(BlackImage);
    //         Paint_Clear(BLACK);
    //     }
    //     
    // }
    
    // DEV_Module_Exit();
    return 0;
}

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
	add_repeating_timer_ms(OLED_MAN_POLL_PERIOD_MS, OledMan_UpdateTimerCallback, NULL, &(oledManContext.oledManUpdateTimer));
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
		Paint_NewImage(oledManContext.pixelBuf, OLED_WIDTH, OLED_HEIGHT, 0, BLACK);
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
	memset(oledManContext.pixelBuf, 0x00, PIXEL_BUF_LEN);
	oledManContext.pixelBufUpdated = true;
}

void OledMan_SetFont(sFONT *font)
{
	oledManContext.font = font;
}

// WRAPPERS
void OledMan_DrawChar(uint16_t x, uint16_t y, char c)
{
	Paint_DrawChar(x, y, c, oledManContext.font, WHITE, BLACK);
	oledManContext.pixelBufUpdated = true;
}

void OledMan_DrawCircle(uint16_t xCenter, uint16_t yCenter, uint16_t radius, bool fill)
{
	Paint_DrawCircle(xCenter, yCenter, radius, WHITE, DOT_PIXEL_1X1, (fill ? DRAW_FILL_FULL : DRAW_FILL_EMPTY));
	oledManContext.pixelBufUpdated = true;
}

void OledMan_DrawString(uint16_t x, uint16_t y, char *str)
{
	Paint_DrawString_EN(x, y, (const char *) str, oledManContext.font, WHITE, BLACK);
	oledManContext.pixelBufUpdated = true;
}

void OledMan_DrawLine(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2)
{
	Paint_DrawLine(x1, y1, x2, y2, WHITE, DOT_PIXEL_1X1, LINE_STYLE_SOLID);
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
	Paint_DrawRectangle(x1, y1, x2, y2, WHITE, DOT_PIXEL_1X1, (fill ? DRAW_FILL_FULL : DRAW_FILL_EMPTY));
	oledManContext.pixelBufUpdated = true;
}

void OledMan_SetPixel(uint16_t x, uint16_t y, uint8_t set)
{
	Paint_SetPixel(x, y, (set > 0 ? WHITE : BLACK));
	oledManContext.pixelBufUpdated = true;
}

