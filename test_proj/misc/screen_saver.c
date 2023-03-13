#include "screen_saver.h"
#include "oled_manager.h"
#include <stdio.h>
#include <string.h>
#include "hardware/timer.h"
#include "test_functionality.h"

ScreenSaverContext_s screenSaverContext = {.initialized = false};
char screenSaverString[SS_STR_MAX];
struct repeating_timer screenSaverUpdateTimer;

const char *directionStrings[] = {
	[DOWN_RIGHT] = "DOWN_RIGHT",
	[DOWN_LEFT] = "DOWN_LEFT",
	[UP_LEFT] = "UP_LEFT",
	[UP_RIGHT] = "UP_RIGHT"
};

const char *sideStrings[] = {
	[UP] = "UP",
	[RIGHT] = "RIGHT",
	[DOWN] = "DOWN",
	[LEFT] = "LEFT"
};

static bool ScreenSaver_TimerCallback(struct repeating_timer *t)
{
	ScreenSaver_Update();
	ScreenSaver_Draw();
	return true;
}

static Side_e ScreenSaver_CollisionDetect(void)
{
	Side_e retSide = SIDE_INVALID;
	if (screenSaverContext.x1 <= SCREEN_MIN_X && (screenSaverContext.direction == UP_LEFT || screenSaverContext.direction == DOWN_LEFT))
	{
		retSide = LEFT;
	}
	else if (screenSaverContext.y1 <= SCREEN_MIN_Y && (screenSaverContext.direction == UP_LEFT || screenSaverContext.direction == UP_RIGHT))
	{
		retSide = UP;
	}
	else if (screenSaverContext.x2 >= SCREEN_MAX_X && (screenSaverContext.direction == UP_RIGHT || screenSaverContext.direction == DOWN_RIGHT))
	{
		retSide = RIGHT;
	}
	else if (screenSaverContext.y2 >= SCREEN_MAX_Y && (screenSaverContext.direction == DOWN_RIGHT || screenSaverContext.direction == DOWN_LEFT))
	{
		retSide = DOWN;
	}
	// if (retSide != SIDE_INVALID)
		// printf("COLLISION %s\n", sideStrings[retSide]);
	return retSide;
}

void ScreenSaver_Init(char *str)
{
	uint8_t stringLen = strlen(str);
	if (stringLen > SS_STR_MAX)
	{
		printf("Screensaver string length too long! %d > %d\n", stringLen, SS_STR_MAX);
		return;
	}
	memset(screenSaverString, 0x00, sizeof(screenSaverString));
	strcpy(screenSaverString, str);
	uint8_t rectWidth = (3 * W_RECT_MARGIN) + (stringLen * CHAR_PIX_W);
	uint8_t rectHeight = (3 * H_RECT_MARGIN) + (CHAR_PIX_H);
	screenSaverContext = (ScreenSaverContext_s) {
		.initialized = true,
		.running = false,
		.x1 = SCREEN_MIN_X,
		.y1 = SCREEN_MIN_Y,
		.x2 = rectWidth,
		.y2 = rectHeight,
		.xVel = DEFAULT_X_VEL,
		.yVel = DEFAULT_Y_VEL,
		.string = screenSaverString,
		.stringLen = strlen(str),
		.direction = DOWN_RIGHT
	};
}

void ScreenSaver_Start(void)
{
	OledMan_ClearBuf();
	if (!screenSaverContext.initialized)
	{
		printf("Need to init screensaver first!\n");
		return;
	}
	add_repeating_timer_ms(SCREENSAVER_UPDATE_PERIOD_MS,ScreenSaver_TimerCallback, NULL, &screenSaverUpdateTimer);
	screenSaverContext.running = true;
}

void ScreenSaver_Stop(void)
{
	cancel_repeating_timer(&screenSaverUpdateTimer);
	screenSaverContext.running = false;
	printf("Screen saver stopped!\n");
	OledMan_ClearBuf();
}

void ScreenSaver_Update(void)
{ 
	switch(screenSaverContext.direction)
	{
		case DOWN_RIGHT:
		{
			screenSaverContext.x1 += 1;
			screenSaverContext.y1 += 1;
			screenSaverContext.x2 += 1;
			screenSaverContext.y2 += 1;
			break;
		}
		case DOWN_LEFT:
		{
			screenSaverContext.x1 -= 1;
			screenSaverContext.y1 += 1;
			screenSaverContext.x2 -= 1;
			screenSaverContext.y2 += 1;
			break;
		}
		case UP_LEFT:
		{
			screenSaverContext.x1 -= 1;
			screenSaverContext.y1 -= 1;
			screenSaverContext.x2 -= 1;
			screenSaverContext.y2 -= 1;
			break;
		}
		case UP_RIGHT:
		{
			screenSaverContext.x1 += 1;
			screenSaverContext.y1 -= 1;
			screenSaverContext.x2 += 1;
			screenSaverContext.y2 -= 1;
			break;
		}
		default:{}
	}

	Side_e newSide = ScreenSaver_CollisionDetect();
	switch(newSide)
	{
		case UP:
		{
			screenSaverContext.direction = (screenSaverContext.direction == UP_LEFT ? DOWN_LEFT : DOWN_RIGHT);
			break;
		}
		case RIGHT:
		{
			screenSaverContext.direction = (screenSaverContext.direction == DOWN_RIGHT ? DOWN_LEFT : UP_LEFT);
			break;
		}
		case DOWN:
		{
			screenSaverContext.direction = (screenSaverContext.direction == DOWN_LEFT ? UP_LEFT : UP_RIGHT);
			break;
		}
		case LEFT:
		{
			screenSaverContext.direction = (screenSaverContext.direction == UP_LEFT ? UP_RIGHT : DOWN_RIGHT);
			break;
		}
		case SIDE_INVALID:
		default:{}
	}
	// if (newSide != SIDE_INVALID)
		// printf("NEW DIR %s\n", directionStrings[screenSaverContext.direction]);
}

void ScreenSaver_Draw(void)
{
	OledMan_ClearBuf();
	OledMan_DrawRectangleAbsolute(screenSaverContext.x1,
															 screenSaverContext.y1, 
															 screenSaverContext.x2, 
															 screenSaverContext.y2, 
															 false);
	OledMan_DrawString(screenSaverContext.x1 + W_RECT_MARGIN, 
										screenSaverContext.y1 + H_RECT_MARGIN,
										screenSaverContext.string);
	OledMan_DrawRectangleAbsolute(SCREEN_MIN_X, SCREEN_MIN_Y, 
															 SCREEN_MAX_X, SCREEN_MAX_Y, false);
}

bool ScreenSaver_IsRunning(void)
{
	return screenSaverContext.running;
}
