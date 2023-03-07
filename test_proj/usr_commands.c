#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "usr_commands.h"
#include "pico/bootrom.h"
#include "test_functionality.h"

UserCommand_t userCommands[] = {
	{"reset", UserCommand_Reset, "Reset the board into boot mode"},
};

void UserCommand_PrintCommand(uint16_t argc, char **argv)
{
	printf("COMMAND: ");
	for (uint16_t i = 0; i < argc; i++)
	{
		// printf("%s ", argv[i]);
		printf("%s.\n", argv[i]);
	}
	printf("\n");
}

void UserCommand_ProcessInputLine(char *buf)
{
	char *command = buf;
	char *argv[USER_COMMAND_MAX_ARGS];
	uint16_t argc = 0;
	char *parseIdx = buf;
	char *arg = parseIdx;
	
	// see if this is a single command or if we have args
	while(true)
	{
		while(*parseIdx != ' ' && *parseIdx != '\0')
		{
			parseIdx++;
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
	UserCommand_PrintCommand(argc, argv);
}

void UserCommand_Reset(uint8_t argc, void *argv)
{
	reset_usb_boot(0,0);
}
