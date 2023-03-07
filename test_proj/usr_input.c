#include "usr_input.h"
#include "hardware/timer.h"
#include "pico/stdlib.h"
#include "hardware/timer.h"
#include <stdio.h>
#include <string.h>
#include "test_functionality.h"
#include "usr_commands.h"

char usrInputBuf[USER_INPUT_BUF_LEN];
uint32_t usrInputIdx = 0;
uint32_t usrInputPrintedIdx = 0;
struct repeating_timer printCurrentLineTimer, userInputPollTimer;

static void UserInput_CharAvailCallback(void *args)
{
}

static void UserInput_ParseInputLine(char *buf)
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
	// for (uint16_t i = 0; i < (sizeof(userCommands) / sizeof(userCommands[0])); i++)
	// {
	// 	if (strcmp(userCommands[i].command, argv[0]) == 0)
	// 	{
	// 		printf("COMMAND %s\n", argv[0]); // TODO
	// 	}
	// }
}

static bool UserInput_InputPollTimerCallback(struct repeating_timer_t *t)
{
	int c = getchar_timeout_us(0);
	if (c == PICO_ERROR_TIMEOUT)
	{
		return true;
	}
	if (usrInputIdx == 0 && c == USER_INPUT_SPACE_CHAR)
	{
		return true;
	}
	if (c == USER_INPUT_ENTER_CHAR)
	{
		usrInputBuf[usrInputIdx] = '\0';
		printf("\nCMD:%s\n", usrInputBuf);
		// UserCommand_ProcessInputLine((char *) &usrInputBuf);
		// UserCommand_RequestService();
		UserInput_ParseInputLine((char *) &usrInputBuf);
		usrInputIdx = 0;
	}
	else if (c == USER_INPUT_BKSPACE_CHAR)
	{
		if (usrInputIdx > 0)
		{
			usrInputBuf[usrInputIdx] = '\0';
			printf("\b \b");
			usrInputIdx -= (usrInputIdx > 0 ? 1 : 0);
		}
	}
	else
	{
		usrInputBuf[usrInputIdx] = c;
		usrInputIdx++;
		printf("%c", c);
	}
	return true;
}

void UserInput_StartPollTimer(void)
{
	add_repeating_timer_ms(USER_INPUT_POLL_PERIOD_MS, UserInput_InputPollTimerCallback, NULL, &userInputPollTimer);
}

void UserInput_StopPollTimer(void)
{
	cancel_repeating_timer(&userInputPollTimer);
}

void UserInput_Init(void)
{
	memset((void *) &usrInputBuf, 0x00, sizeof(usrInputBuf));
	// stdio_set_chars_available_callback(UserInput_CharAvailCallback, NULL);
	UserInput_StartPollTimer();
}

void UserInput_Service(void)
{
}
