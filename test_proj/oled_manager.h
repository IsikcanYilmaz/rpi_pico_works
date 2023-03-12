#include <stdbool.h>
#include "OLED_1in3_c.h"

#define OLED_WIDTH OLED_1in3_C_WIDTH //128
#define OLED_HEIGHT OLED_1in3_C_HEIGHT //64
#define PIXEL_BUF_LEN (OLED_WIDTH * OLED_HEIGHT)

typedef struct OledManContext_s
{
	bool initialized;
	uint8_t pixelBuf[PIXEL_BUF_LEN];
} OledManContext_t;

void OledMan_Init(void);
void OledMan_DrawBuf(void);
void OledMan_ClearBuf(void);
void OledMan_Test0(void);
void OledMan_Test1(void);
