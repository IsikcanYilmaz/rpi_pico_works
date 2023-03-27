#include <stdbool.h>
#include "pico/stdlib.h"
#include "hardware/timer.h"

#define BUTTON_KEY_0_GPIO 15
#define BUTTON_KEY_1_GPIO 17

#define BUTTON_STATE_PRESSED 0 // polarity
#define BUTTON_STATE_RELEASED 1

#define BUTTON_POLL_PERIOD_MS 100

typedef enum ButtonEventBitMask_e_
{
	BUTTON_EVENT_LEVEL_LOW,
	BUTTON_EVENT_LEVEL_HIGH,
	BUTTON_EVENT_EDGE_FALL,
	BUTTON_EVENT_EDGE_RISE
} ButtonEventBitMask_e;

typedef enum Button_e_
{
	BUTTON_0,
	BUTTON_1,
	BUTTON_BOTH,
	BUTTON_NONE
} Button_e;

typedef struct ButtonContext_s_
{
	struct repeating_timer buttonTimer;
	bool button0State;
	bool button1State;
	bool timerRunning;
} ButtonContext_s;

void Button_Init(void);
bool Button_IsPressed(Button_e b);
Button_e Button_GetPressedButtons(void);
