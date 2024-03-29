#include "misc_manager.h"
#include "pico/stdlib.h"
#include "hardware/timer.h"
#include "screen_saver.h"
#include "game_of_life.h"
#include "wifi_scan.h"
#include "picture.h"
#include "misc_test.h"
#include "usr_commands.h"
#include <stdio.h>
#include <string.h>

#define MISC_SCREEN_SAVER_ENABLED 1
#define MISC_GAME_OF_LIFE_ENABLED 1
#define MISC_PICTURE_ENABLED 1
#define MISC_WIFI_SCAN_ENABLED 1
#define MISC_TEST_ENABLED 1

MiscProgram_t miscPrograms[] = 
{
	#if MISC_SCREEN_SAVER_ENABLED
	[MISC_SCREEN_SAVER] = {
		.name = "screen_saver",
		.init = ScreenSaver_Init,
		.deinit = ScreenSaver_Deinit,
		.start = ScreenSaver_Start,
		.stop = ScreenSaver_Stop,
		.update = ScreenSaver_Update,
		.draw = ScreenSaver_Draw,
		.buttonInput = ScreenSaver_ButtonInput,
		.isRunning = false,
		.updatePeriodMs = SCREENSAVER_UPDATE_PERIOD_MS,
		},
	#endif
	#if MISC_GAME_OF_LIFE_ENABLED
	[MISC_GAME_OF_LIFE] = {
		.name = "gol",
		.init = Gol_Init,
		.deinit = Gol_Deinit,
		.start = Gol_Start,
		.stop = Gol_Stop,
		.update = Gol_Update,
		.draw = Gol_Draw,
		.buttonInput = Gol_ButtonInput,
		.isRunning = false,
		.updatePeriodMs = GOL_UPDATE_PERIOD_MS,
		},
	#endif
	#if MISC_PICTURE_ENABLED
	[MISC_PICTURE] = {
		.name = "picture",
		.init = Picture_Init,
		.deinit = Picture_Deinit,
		.start = Picture_Start,
		.stop = Picture_Stop,
		.update = Picture_Update,
		.draw = Picture_Draw,
		.buttonInput = Picture_ButtonInput,
		.isRunning = false,
		.updatePeriodMs = PICTURE_UPDATE_PERIOD_MS,
		},
	#endif
	#if MISC_WIFI_SCAN_ENABLED
	[MISC_WIFI_SCAN] = {
		.name = "wifi_scan",
		.init = WifiScan_Init,
		.deinit = WifiScan_Deinit,
		.start = WifiScan_Start,
		.stop = WifiScan_Stop,
		.update = WifiScan_Update,
		.draw = WifiScan_Draw,
		.buttonInput = WifiScan_ButtonInput,
		.isRunning = false,
		.updatePeriodMs = WIFI_SCAN_UPDATE_PERIOD_MS,
		},
	#endif
	#if MISC_TEST_ENABLED
	[MISC_TEST] = {
		.name = "misc_test",
		.init = MiscTest_Init,
		.deinit = MiscTest_Deinit,
		.start = MiscTest_Start,
		.stop = MiscTest_Stop,
		.update = MiscTest_Update,
		.draw = MiscTest_Draw,
		.buttonInput = MiscTest_ButtonInput,
		.isRunning = false,
		.updatePeriodMs = MISC_TEST_UPDATE_PERIOD_MS,
		},
	#endif
};

MiscIdx_e currentProgramIdx = MISC_MAX;
MiscProgram_t *currentProgram = NULL;
struct repeating_timer miscUpdateTimer;

static bool Misc_TimerCallback(struct repeating_timer *t)
{
	if (currentProgramIdx >= MISC_MAX)
	{
		return false;
	}
	miscPrograms[currentProgramIdx].update();
	miscPrograms[currentProgramIdx].draw();
	return true;
}

void Misc_ManagerInit(void)
{
	// TODO?
}

void Misc_TimerStart(void)
{
	add_repeating_timer_ms(miscPrograms[currentProgramIdx].updatePeriodMs, Misc_TimerCallback, NULL, &miscUpdateTimer);
}

void Misc_TimerStop(void)
{
	cancel_repeating_timer(&miscUpdateTimer);
}

void Misc_StopProgram(void)
{
	if (currentProgramIdx < MISC_MAX)
	{
		printf("Stopping program %d:%s\n", currentProgramIdx, miscPrograms[currentProgramIdx]);
		Misc_TimerStop();
		miscPrograms[currentProgramIdx].isRunning = false;
		miscPrograms[currentProgramIdx].stop();
		miscPrograms[currentProgramIdx].deinit();
		currentProgramIdx = MISC_MAX;
	}
	else 
	{
		printf("No running misc program\n");
	}
}

void Misc_StartProgram(MiscIdx_e idx, void *arg)
{
	// First stop the ongoing program
	if (idx >= MISC_MAX)
	{
		printf("Bad idx for Misc_StartProgram %d\n", idx);
		return;
	}
	Misc_StopProgram();
	currentProgramIdx = MISC_MAX;

	// Pass whatever's been passed to the arg to our program
	bool ret = miscPrograms[idx].init(arg);
	if (ret)
	{
		printf("Starting program %d:%s\n", idx, miscPrograms[idx]);
		miscPrograms[idx].isRunning = true;
		miscPrograms[idx].start();
		currentProgramIdx = idx;

		// Check if this misc program requires periodic updates/draws. if not, just run its update/draw once and call it
		if (miscPrograms[idx].updatePeriodMs == 0)
		{
			miscPrograms[idx].update();
			miscPrograms[idx].draw();
		}
		else
		{
			Misc_TimerStart();
		}
	}
	else
	{
		printf("Program %d:%s failed to start\n", idx, miscPrograms[idx].name);
		return;
	}
}

MiscIdx_e Misc_StrToMiscIdx(char *str)
{
	MiscIdx_e ret = MISC_MAX;
	for (uint8_t i = 0; i < sizeof(miscPrograms)/sizeof(miscPrograms[0]); i++)
	{
		if (strcmp(str, miscPrograms[i].name) == 0)
		{
			ret = i;
			break;
		}
	}
	return ret;
}

static void Misc_ProcessSubprogramInput(MiscIdx_e idx, uint8_t argc, char **argv)
{
	printf("Misc command to %s\n", miscPrograms[idx].name);
	UserCommand_PrintCommand(argc, argv);
	ASSERT_ARGS(1);
	if (strcmp(argv[0], "start") == 0)
	{
		GuiMan_Stop();
		Misc_StartProgram(idx, (argc > 1) ? argv[1] : NULL);
	}
	else if (strcmp(argv[0], "stop") == 0)
	{
		Misc_StopProgram();
		GuiMan_Start();
	}
	else 
	{
		BAD_ARG();
	}
}

void Misc_TakeTextInput(uint8_t argc, char **argv)
{
	ASSERT_ARGS(1);
	UserCommand_PrintCommand(argc, argv);
	if (strcmp(argv[0], "list") == 0)
	{
		Misc_PrintPrograms();
	}
	else if (strcmp(argv[0], "stop") == 0)
	{
		Misc_StopProgram();
	}
	else
	{
		MiscIdx_e idx = Misc_StrToMiscIdx(argv[0]);
		if (idx < MISC_MAX)
		{
			Misc_ProcessSubprogramInput(idx, argc-1, &argv[1]);
		}
	}
}

void Misc_TakeButtonInput(Button_e button, ButtonGesture_e gesture)
{
	if (button == BUTTON_0 && gesture == GESTURE_VVLONG_PRESS)
	{
		MiscIdx_e tmpIdx = currentProgramIdx;
		Misc_StopProgram();
		currentProgramIdx = tmpIdx+1;
		if (currentProgramIdx >= MISC_MAX)
		{
			currentProgramIdx = MISC_SCREEN_SAVER;
		}
		Misc_StartProgram(currentProgramIdx, NULL);
	}
	else
	{
		miscPrograms[currentProgramIdx].buttonInput(button, gesture);
	}
}

void Misc_PrintPrograms(void)
{
	for (uint16_t i = 0; i < sizeof(miscPrograms)/sizeof(miscPrograms[0]); i++)
	{
		printf("%d: %s %c\n", i, miscPrograms[i].name, (i == currentProgramIdx) ? '*' : ' ');
	}
}

uint16_t Misc_GetNumPrograms(void)
{
	return sizeof(miscPrograms)/sizeof(miscPrograms[0]);
}

MiscProgram_t* Misc_GetProgramPtrByIdx(uint16_t i)
{
	if (i >= MISC_MAX)
	{
		printf("Bad idx for get program by id %d\n", i);
		return NULL;
	}
	return &miscPrograms[i];
}

bool Misc_IsProgramRunning(void)
{
	return (currentProgramIdx < MISC_MAX);
}

MiscIdx_e Misc_GetCurrentRunningProgramIdx(void)
{
	return currentProgramIdx;
}
