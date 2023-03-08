#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "usr_commands.h"
#include "pico/bootrom.h"
#include "test_functionality.h"

UserCommand_t userCommands[] = {
	{"reset", UserCommand_Reset, "Reset the board into boot mode"},
	{"test", UserCommand_Test, "test"},
	{"led", UserCommand_LedSet, "Set Led"},
};

static void UserCommand_PrintCommand(uint16_t argc, char **argv)
{
	printf("COMMAND: ");
	for (uint16_t i = 0; i < argc; i++)
	{
		printf("%s ", argv[i]);
		// printf("%s.\n", argv[i]);
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
			// printf("COMMAND %s\n", argv[0]); // TODO
			userCommands[i].fn(argc, argv);
		}
	}
}

void UserCommand_Reset(uint8_t argc, char **argv)
{
	printf("Resetting!...\n");
	reset_usb_boot(0,0);
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
