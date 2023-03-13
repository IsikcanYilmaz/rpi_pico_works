#include <stdint.h>
#include <stdlib.h>

#define GOL_WIDTH 128
#define GOL_HEIGHT 64
#define GOL_BORDERLESS false

void Gol_Init(void);
void Gol_Update(void);
void Gol_SetAlive(uint8_t x, uint8_t y);
