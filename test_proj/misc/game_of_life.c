#include "game_of_life.h"
#include "oled_manager.h"
#include "test_functionality.h"

uint8_t currentBoardBuf[GOL_BOARD_BUF_LEN];
uint8_t nextBoardBuf[GOL_BOARD_BUF_LEN];

Bitmap_t currentBoard = {.buf = currentBoardBuf, .width = GOL_WIDTH, .height = GOL_HEIGHT};
Bitmap_t nextBoard = {.buf = nextBoardBuf, .width = GOL_WIDTH, .height = GOL_HEIGHT};

GolContext_s golContext = {.currentBoard = &currentBoard, .nextBoard = &nextBoard};

bool Gol_Init(void *arg)
{
	Bitmap_Clear(&currentBoard);
	Bitmap_Clear(&nextBoard);

	Gol_SetCell(0, 3, true);
	Gol_SetCell(1, 3, true);
	Gol_SetCell(2, 3, true);
	Gol_SetCell(2, 2, true);
	Gol_SetCell(1, 1, true);
	return true;
}

void Gol_Deinit(void)
{
}

void Gol_Update(void)
{
	// toggleLed();
	for (uint8_t x = 0; x < GOL_WIDTH; x++)
	{
		for (uint8_t y = 0; y < GOL_HEIGHT; y++)
		{
			uint8_t numAliveNeighbors = Gol_GetAliveNeighbors(x, y);
			uint8_t currentState = Bitmap_GetPixel(golContext.currentBoard, x, y);
			uint8_t rule = 0;
			bool cellLives = false;

			// is alive and one or no neighbors
			if (currentState && numAliveNeighbors < 2)
			{
				cellLives = false;
				rule = 1;
			}

			// is alive and 2 or 3 alive neighbors
			else if (currentState && (numAliveNeighbors == 2 || numAliveNeighbors == 3))
			{
				cellLives = true;
				rule = 2;
			}

			// is alive and more than 4 alive neighbors
			else if (currentState && numAliveNeighbors >= 4)
			{
				cellLives = false;
				rule = 3;
			}

			// is dead and 3 alive neighbors
			else if (currentState == 0 && numAliveNeighbors == 3)
			{
				cellLives = true;
				rule = 4;
			}
			Bitmap_SetPixel(golContext.nextBoard, x, y, cellLives ? 1 : 0);
		}
	}
	Bitmap_t *tmp = golContext.currentBoard;
	golContext.currentBoard = golContext.nextBoard;
	golContext.nextBoard = tmp;
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
