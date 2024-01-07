#include <stdbool.h>
#include <stdint.h>
#include "misc_manager.h"

#define SCREENSAVER_UPDATE_PERIOD_MS 50

#define W_RECT_MARGIN 3
#define H_RECT_MARGIN 3

#define CHAR_PIX_W 5
#define CHAR_PIX_H 6

#define SS_STR_MAX 20

#define SCREEN_MIN_X 5
#define SCREEN_MAX_X 127
#define SCREEN_MIN_Y 2
#define SCREEN_MAX_Y 63

#define DEFAULT_X_VEL 1
#define DEFAULT_Y_VEL 1

#define SCREEN_SAVER_DEFAULT_STRING "~~TEST~~"

typedef enum Direction_e_
{
	DOWN_RIGHT,
	DOWN_LEFT,
	UP_LEFT,
	UP_RIGHT,
	DIRECTION_MAX
} Direction_e;

typedef enum Side_e_
{
	UP,
	RIGHT,
	DOWN,
	LEFT,
	SIDE_INVALID
} Side_e;

typedef struct ScreenSaverContext_t
{
	bool initialized;
	bool running;
	uint8_t x1;
	uint8_t y1;
	uint8_t x2;
	uint8_t y2;
	uint8_t xVel;
	uint8_t yVel;
	char *string;
	uint8_t stringLen;
	Direction_e direction;
} ScreenSaverContext_s;

bool ScreenSaver_Init(void *arg);
void ScreenSaver_Deinit(void);
void ScreenSaver_Update(void);
void ScreenSaver_Draw(void);
void ScreenSaver_Start(void);
void ScreenSaver_Stop(void);
void ScreenSaver_ButtonInput(Button_e b, ButtonGesture_e g);

bool ScreenSaver_IsRunning(void);

