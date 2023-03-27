#include <stdint.h>
#include <stdlib.h>
#include "bitmap.h"

#define GOL_BOARD_BUF_LEN (128 * 64 / 8)
#define GOL_WIDTH 126//126 
#define GOL_HEIGHT 62//62 
#define GOL_DRAW_BORDER false 

#define GOL_UPDATE_PERIOD_MS 50 

typedef struct GolContext_t
{
	Bitmap_t *currentBoard;
	Bitmap_t *nextBoard;
} GolContext_s;

bool Gol_Init(void *arg);
void Gol_Deinit(void);
void Gol_Update(void);
void Gol_Draw(void);
void Gol_Start(void);
void Gol_Stop(void);

uint8_t Gol_GetAliveNeighbors(uint8_t x, uint8_t y);
void Gol_SetCell(uint8_t x, uint8_t y, bool val);
