#include <stdbool.h>
#include "OLED_1in3_c.h"
#include "fonts.h"

#define OLED_WIDTH OLED_1in3_C_WIDTH //128
#define OLED_HEIGHT OLED_1in3_C_HEIGHT //64
#define OLED_PIXEL_BUF_LEN (OLED_WIDTH * OLED_HEIGHT)
#define OLED_DEFAULT_FONT (&Font8)
#define OLED_MAN_POLL_PERIOD_MS (10)

// /**
//  * The size of the point
// **/
// typedef enum {
//     DOT_PIXEL_1X1  = 1,	// 1 x 1
//     DOT_PIXEL_2X2  , 		// 2 X 2
//     DOT_PIXEL_3X3  ,		// 3 X 3
//     DOT_PIXEL_4X4  ,		// 4 X 4
//     DOT_PIXEL_5X5  , 		// 5 X 5
//     DOT_PIXEL_6X6  , 		// 6 X 6
//     DOT_PIXEL_7X7  , 		// 7 X 7
//     DOT_PIXEL_8X8  , 		// 8 X 8
// } DOT_PIXEL;
// #define DOT_PIXEL_DFT  DOT_PIXEL_1X1  //Default dot pilex
//
// /**
//  * Point size fill style
// **/
// typedef enum {
//     DOT_FILL_AROUND  = 1,		// dot pixel 1 x 1
//     DOT_FILL_RIGHTUP  , 		// dot pixel 2 X 2
// } DOT_STYLE;
// #define DOT_STYLE_DFT  DOT_FILL_AROUND  //Default dot pilex
//
// /**
//  * Line style, solid or dashed
// **/
// typedef enum {
//     LINE_STYLE_SOLID = 0,
//     LINE_STYLE_DOTTED,
// } LINE_STYLE;
//
// /**
//  * Whether the graphic is filled
// **/
// typedef enum {
//     DRAW_FILL_EMPTY = 0,
//     DRAW_FILL_FULL,
// } DRAW_FILL;
//

typedef struct OledManContext_s
{
	bool initialized;
	bool blocked;
	sFONT *font;
	struct repeating_timer oledManUpdateTimer;
	volatile bool pixelBufUpdated; 
	uint8_t pixelBuf[OLED_PIXEL_BUF_LEN];
} OledManContext_t;

void OledMan_Init(void);
void OledMan_DrawBuf(void);
void OledMan_ClearBuf(void);
void OledMan_StartPollTimer(void);
void OledMan_StopPollTimer(void);
void OledMan_SetFont(sFONT *font);
void OledMan_DrawChar(uint16_t x, uint16_t y, char c);
void OledMan_DrawCircle(uint16_t xCenter, uint16_t yCenter, uint16_t radius, bool fill);
void OledMan_DrawLine(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2);
void OledMan_DrawString(uint16_t x, uint16_t y, char *str);
void OledMan_DrawRectangle(uint16_t x, uint16_t y, uint16_t width, uint16_t height, bool fill);
void OledMan_DrawRectangleAbsolute(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2, bool fill);
void OledMan_SetPixel(uint16_t x, uint16_t y, uint8_t set);
