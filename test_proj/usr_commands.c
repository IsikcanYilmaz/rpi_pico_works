#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "usr_commands.h"
#include "test_functionality.h"
#include "wifi.h"
#include "oled_manager.h"
#include "GUI_Paint.h"
#include "game_of_life.h"

#define ASSERT_ARGS(argcExpected) {if (argc < argcExpected) {printf("Bad args! argc %d expected %d\n", argc, argcExpected); return;}}
#define BAD_ARG() {printf("Bad arg!\n"); UserCommand_PrintCommand(argc, argv);}

UserCommand_t userCommands[] = {
	{"reset", UserCommand_Reset, "Reset the board into boot mode"},
	{"test", UserCommand_Test, "test"},
	{"led", UserCommand_LedSet, "Set Led"},
	{"loopback", UserCommand_Loopback, "Loopback"},
	{"wifi", UserCommand_Wifi, "Wifi commands"},
	{"oled", UserCommand_Oled, "Oled commands"}
};

static void UserCommand_PrintCommand(uint16_t argc, char **argv)
{
	printf("COMMAND: ");
	for (uint16_t i = 0; i < argc; i++)
	{
		printf("%s ", argv[i]);
	}
	printf("\n");
}

void UserCommand_ProcessCommand(uint16_t argc, char **argv)
{
	// We have the command and args parsed now. do a lookup to see
	// which command is entered and call its function
	for (uint16_t i = 0; i < (sizeof(userCommands) / sizeof(userCommands[0])); i++)
	{
		if (strcmp(userCommands[i].command, argv[0]) == 0)
		{
			// printf("COMMAND %s argc %d\n", argv[0], argc); // TODO
			userCommands[i].fn(argc, argv);
			return;
		}
	}
	printf("Command not found\n");
}

// COMMANDS // 
void UserCommand_Reset(uint8_t argc, char **argv)
{
	if (argc == 2 && strcmp(argv[1], "boot") == 0)
	{
		softwareReset(true);
		return;
	}
	softwareReset(false);
}

void UserCommand_Test(uint8_t argc, char **argv)
{
	toggleLed();
}

void UserCommand_LedSet(uint8_t argc, char **argv)
{
	bool boolArg = (atoi(argv[1]) > 0);
	setLed(boolArg);
}

void UserCommand_Loopback(uint8_t argc, char **argv)
{
	for (uint16_t i = 0; i < argc; i++)
	{
		printf("%s ", argv[i]);
	}
	printf("\n");
}

void UserCommand_Wifi(uint8_t argc, char **argv)
{
	ASSERT_ARGS(2);

	// args
	if (strcmp(argv[1], "scan") == 0)
	{
		printf("Wifi scan\n");
		Wifi_Scan();
	}
	else if (strcmp(argv[1], "connect") == 0)
	{
		ASSERT_ARGS(4);
		Wifi_Connect(argv[2], argv[3]);
	}
	else
	{
		BAD_ARG();
	}
}

void UserCommand_Oled(uint8_t argc, char **argv)
{
	ASSERT_ARGS(2);

	// args
	if (strcmp(argv[1], "line") == 0)
	{
		ASSERT_ARGS(6);
		uint16_t x1 = atoi(argv[2]);
		uint16_t y1 = atoi(argv[3]);
		uint16_t x2 = atoi(argv[4]);
		uint16_t y2 = atoi(argv[5]);
		OledMan_DrawLine(x1, x2, y1, y2);
		printf("Draw line from %d %d to %d %d\n", x1, y1, x2, y2);
	}
	else if (strcmp(argv[1], "circle") == 0)
	{
		ASSERT_ARGS(6);
		uint16_t xCenter = atoi(argv[2]);
		uint16_t yCenter = atoi(argv[3]);
		uint16_t radius = atoi(argv[4]);
		bool fill = (atoi(argv[5]) > 0);
		OledMan_DrawCircle(xCenter, yCenter, radius, fill);
		printf("Draw circle at %d %d with r %d f %d\n", xCenter, yCenter, radius, fill);
	}
	else if (strcmp(argv[1], "string") == 0)
	{
		ASSERT_ARGS(5);
		uint16_t x = atoi(argv[2]);
		uint16_t y = atoi(argv[3]);
		char *str = argv[4];
		OledMan_DrawString(x, y, str);
		printf("Draw string \"%s\" at %d %d\n", str, x, y);
	}
	else if (strcmp(argv[1], "rect") == 0)
	{
		ASSERT_ARGS(7); // oled rect 0 0 5 5 1
		uint16_t x1 = atoi(argv[2]);
		uint16_t y1 = atoi(argv[3]);
		uint16_t x2 = atoi(argv[4]);
		uint16_t y2 = atoi(argv[5]);
		bool fill = (atoi(argv[6]) > 0);
		OledMan_DrawRectangle(x1, y1, x2, y2, fill);
		printf("Draw rect x1%d y1%d x2%d y2%d f%d\n", x1, y1, x2, y2, fill);
	}
	else if (strcmp(argv[1], "clear") == 0)
	{
		OledMan_ClearBuf();
	}
	else if (strcmp(argv[1], "pixel") == 0)
	{
		ASSERT_ARGS(4);
		uint16_t x = atoi(argv[2]);
		uint16_t y = atoi(argv[3]);
		OledMan_SetPixel(x, y, true);
		printf("Draw pixel to x%d y%d\n", x, y);
	}
	else
	{
		BAD_ARG();
	}
}
