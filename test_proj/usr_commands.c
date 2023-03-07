#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "usr_commands.h"
#include "pico/bootrom.h"
#include "test_functionality.h"

UserCommand_t userCommands[] = {
	{"reset", UserCommand_Reset, "Reset the board into boot mode"},
	{"test", UserCommand_Test, "test"},
};

volatile bool userCommandParsingRequested = false;

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

void UserCommand_ProcessInputLine(char *buf)
{
	char *argv[USER_COMMAND_MAX_ARGS];
	uint16_t argc = 0;
	char *parseIdx = buf;
	char *arg = parseIdx;
	
	// see if this is a single command or if we have args
	while(true)
	{
		while(*parseIdx != ' ' && *parseIdx != '\0')
		{
			parseIdx++; // TODO point of interest. potential inf loop
		}

		// We skipped the word. parseIdx now points to a space or a null
		argv[argc] = arg;
		argc++;
		
		// If this is the end of the line
		if (*parseIdx == '\0')
		{
			// Parsing done
			break;
		}

		// if this is not the end we're at a space. skip all the spaces
		*parseIdx = '\0';
		parseIdx++;
		while (*parseIdx == ' ')
		{
			parseIdx++;
		}
		arg = parseIdx;
		if (*parseIdx == '\0')
		{
			// there were trailing spaces at the very end of the cmd line
			break;
		}
	}
	// UserCommand_PrintCommand(argc, argv);

	// We have the command and args parsed now. do a lookup to see
	// which command is entered and call its function
	for (uint16_t i = 0; i < (sizeof(userCommands) / sizeof(userCommands[0])); i++)
	{
		if (strcmp(userCommands[i].command, argv[0]) == 0)
		{
			printf("COMMAND %s\n", argv[0]); // TODO
		}
	}
}

void UserCommand_Init(void)
{

}

void UserCommand_Service(void)
{
	if (!userCommandParsingRequested)
	{
		return;
	}
	userCommandParsingRequested = false;
}

void UserCommand_RequestService(void)
{
	userCommandParsingRequested = true;
}

void UserCommand_Reset(uint8_t argc, char **argv)
{
	reset_usb_boot(0,0);
}

void UserCommand_Test(uint8_t argc, char **argv)
{
	toggleLed();
}
