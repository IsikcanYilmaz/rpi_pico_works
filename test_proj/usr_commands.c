#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "usr_commands.h"
#include "test_functionality.h"
#include "wifi.h"
#include "oled_manager.h"
#include "GUI_Paint.h"

#define ASSERT_ARGS(argcExpected) {if (argc < argcExpected) {printf("Bad args! argc %d\n", argc); return;}}
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
	if (strcmp(argv[1], "test") == 0)
	{
		uint8_t testType = (argc > 2 ? atoi(argv[2]) : 0);
		switch(testType)
		{
			case 0:
			{
				OledMan_Test0();
				break;
			}
			case 1:
			{
				OledMan_Test1();
				break;
			}
			default:
			{
				printf("Test unknown\n");
				break;
			}
		}
	}
	else if (strcmp(argv[1], "clear") == 0)
	{
		OledMan_ClearBuf();
		OledMan_DrawBuf();
	}
	else if (strcmp(argv[1], "pixel") == 0)
	{
		ASSERT_ARGS(4);
		uint8_t x = atoi(argv[2]);
		uint8_t y = atoi(argv[3]);
		Paint_SetPixel(x, y, BLACK);
		OledMan_DrawBuf();
		printf("Draw pixel to x%d y%d\n", x, y);
	}
	else
	{
		BAD_ARG();
	}
}
