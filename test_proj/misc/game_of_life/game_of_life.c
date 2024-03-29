#include "game_of_life.h"
#include "oled_manager.h"
#include "test_functionality.h"
#include "button.h"
#include "pico/rand.h"
#include <stdlib.h>

uint8_t currentBoardBuf[GOL_BOARD_BUF_LEN];
uint8_t nextBoardBuf[GOL_BOARD_BUF_LEN];

Bitmap_t currentBoard = {.buf = currentBoardBuf, .width = GOL_WIDTH, .height = GOL_HEIGHT};
Bitmap_t nextBoard = {.buf = nextBoardBuf, .width = GOL_WIDTH, .height = GOL_HEIGHT};

GolContext_s golContext = {.currentBoard = &currentBoard, .nextBoard = &nextBoard, .autoBlocks = true};

bool Gol_Init(void *arg)
{
	Bitmap_Clear(&currentBoard);
	Bitmap_Clear(&nextBoard);
	return true;
}

void Gol_Deinit(void)
{
}

void Gol_Update(void)
{
	for (uint8_t x = 0; x < GOL_WIDTH; x++)
	{
		for (uint8_t y = 0; y < GOL_HEIGHT; y++)
		{
			uint8_t numAliveNeighbors = Gol_GetAliveNeighbors(x, y);
			uint8_t currentState = Bitmap_GetPixel(golContext.currentBoard, x, y);
			bool cellLives = false;

			// is alive and one or no neighbors
			if (currentState && numAliveNeighbors < 2)
			{
				cellLives = false;
			}

			// is alive and 2 or 3 alive neighbors
			else if (currentState && (numAliveNeighbors == 2 || numAliveNeighbors == 3))
			{
				cellLives = true;
			}

			// is alive and more than 4 alive neighbors
			else if (currentState && numAliveNeighbors >= 4)
			{
				cellLives = false;
			}

			// is dead and 3 alive neighbors
			else if (currentState == 0 && numAliveNeighbors == 3)
			{
				cellLives = true;
			}
			Bitmap_SetPixel(golContext.nextBoard, x, y, cellLives ? 1 : 0);
		}
	}
	Bitmap_t *tmp = golContext.currentBoard;
	golContext.currentBoard = golContext.nextBoard;
	golContext.nextBoard = tmp;

	if (golContext.autoBlocks)
	{
		Gol_SpawnRandomBlockByChance();
	}
}

void Gol_Draw(void)
{
	OledMan_ClearBuf();
	for (uint8_t x = 0; x < GOL_WIDTH; x++)
	{
		for (uint8_t y = 0; y < GOL_HEIGHT; y++)
		{
			uint8_t val = Bitmap_GetPixel(&currentBoard, x, y);
			// printf("%d %d: %d\n", x, y, val);
			// OledMan_SetPixel(x, y, val);
			if (val) OledMan_SetPixel(x, y, 1);
		}
	}
}

void Gol_Start(void)
{
}

void Gol_Stop(void)
{
}

void Gol_ButtonInput(Button_e b, ButtonGesture_e g)
{
	if (g == GESTURE_DOUBLE_TAP)
	{
		golContext.autoBlocks != golContext.autoBlocks;
	}
	else
	{
		Gol_SpawnRandomBlock();
	}
}

uint8_t Gol_GetAliveNeighbors(uint8_t x, uint8_t y)
{
	uint8_t aliveNeighbors = 0;
	for (int8_t xDir = -1; xDir <= 1; xDir++)
	{
		for (int8_t yDir = -1; yDir <= 1; yDir++)
		{
			if (xDir == 0 && yDir == 0)
			{
				continue;
			}
			if (xDir == -1 && x == 0)
			{
				continue;
			}
			if (xDir == 1 && x == GOL_WIDTH - 1)
			{
				continue;
			}
			if (yDir == -1 && y == 0)
			{
				continue;
			}
			if (yDir == 1 && y == GOL_HEIGHT - 1)
			{
				continue;
			}
			aliveNeighbors += Bitmap_GetPixel(golContext.currentBoard, x+xDir, y+yDir);
		}
	}
	return aliveNeighbors;
}

void Gol_SetCell(uint8_t x, uint8_t y, bool val)
{
	Bitmap_SetPixel(golContext.currentBoard, x, y, val);
} 

void Gol_SetBlockAbsolute(uint8_t x1, uint8_t y1, uint8_t x2, uint8_t y2)
{
	if (x1 > GOL_WIDTH || x2 > GOL_WIDTH || y1 > GOL_HEIGHT || y2 > GOL_HEIGHT)
	{
		printf("Bad block args! %d %d, %d %d\n", x1, y1, x2, y2);
		return;
	}
	for (uint8_t x = x1; x < x2; x++)
	{
		for (uint8_t y = y1; y < y2; y++)
		{
			Gol_SetCell(x, y, 1);
		}
	}
}

void Gol_SetBlock(uint8_t x, uint8_t y, uint8_t w, uint8_t h)
{
	Gol_SetBlockAbsolute(x, y, x+w, y+h);
}

void Gol_SpawnRandomBlock(void)
{
	uint32_t randNum = get_rand_32();
	uint8_t x = * ((uint8_t *) &randNum) % GOL_WIDTH;
	uint8_t y = * ((uint8_t *) &randNum + 1) % GOL_HEIGHT;
	uint8_t w = * ((uint8_t *) &randNum + 2) % (GOL_WIDTH-x);
	uint8_t h = * ((uint8_t *) &randNum + 3) % (GOL_HEIGHT-y);
	Gol_SetBlock(x, y, w, h);
}

void Gol_SpawnRandomBlockByChance(void)
{
	uint32_t roll = get_rand_32() % 100;
	if (100 - GOL_SPAWN_BLOCK_CHANCE < roll)
	{
		Gol_SpawnRandomBlock();
	}
}
