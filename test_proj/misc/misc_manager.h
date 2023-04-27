#ifndef __MISC_MAN_H_
#define __MISC_MAN_H_
#include <stdbool.h>
#include <stdint.h>
#include "button.h"

typedef enum MiscIdx_e_
{
	MISC_SCREEN_SAVER,
	MISC_GAME_OF_LIFE,
	MISC_PICTURE,
	MISC_WIFI_SCAN,
	MISC_UTIL,
	MISC_MAX,
} MiscIdx_e;

typedef enum InputType_e_
{
	INPUT_STRING,
	INPUT_BUTTON,
	INPUT_INET,
} InputType_e;

typedef struct Misc_s
{
	char *name;
	bool (*init)(void *arg);
	void (*deinit)(void);
	void (*start)(void);
	void (*stop)(void);
	void (*update)(void);
	void (*draw)(void);
	void (*buttonInput)(Button_e b, ButtonGesture_e g);
	bool isRunning;
	uint16_t updatePeriodMs;
	// TODO at some point make all misc programs use the same memory pool.
} MiscProgram_t;

void Misc_ManagerInit(void);
void Misc_StopProgram(void);
void Misc_StartProgram(MiscIdx_e idx, void *arg);
void Misc_TakeTextInput(uint8_t argc, char **argv);
void Misc_TakeButtonInput(Button_e button, ButtonGesture_e gesture);
void Misc_TimerStart(void);
void Misc_TimerStop(void);
void Misc_PrintPrograms(void);
uint16_t Misc_GetNumPrograms(void);
MiscProgram_t* Misc_GetProgramPtrByIdx(MiscIdx_e i);
bool Misc_IsProgramRunning(void);
MiscIdx_e Misc_GetCurrentRunningProgramIdx(void);
#endif
