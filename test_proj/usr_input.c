#include "usr_input.h"
#include "hardware/timer.h"
#include "pico/stdlib.h"
#include "hardware/timer.h"
#include "pico/stdio_usb.h"
#include "pico/stdio/driver.h"
#include <stdio.h>
#include <string.h>
#include "test_functionality.h"
#include "usr_commands.h"

char usrInputBuf[USER_INPUT_BUF_LEN];
uint32_t usrInputIdx = 0;
uint32_t usrInputPrintedIdx = 0;
volatile bool usrInputCharAvail = false;
volatile uint8_t test = 0;
volatile int testchar = 0;
struct repeating_timer printCurrentLineTimer, userInputPollTimer;

extern stdio_driver_t stdio_usb;

static bool UserInput_PrintCurrentCharTimerCallback(struct repeating_timer *t)
{
	// while(usrInputPrintedIdx != usrInputIdx)
	// {
	// 	printf("%c", usrInputBuf[usrInputPrintedIdx]);
	// 	usrInputPrintedIdx++;
	// }
	// printf("Repeating alarm callback %d\n", time_us_64());
	// toggleLed();
	// printf("%s:%d->%d\n", "ASD", usrInputPrintedIdx, usrInputIdx);
	return true;
}

static void UserInput_CommandEntered(char *buf)
{
	printf("CMD: %s\n", buf);
}

static void UserInput_CharAvailCallback(void *args)
{
	// int c = 0;
	// while(c != PICO_ERROR_TIMEOUT) // TODO this may loop forever?
	// {
	// 	c = (int) getchar_timeout_us();
	// 	// if (c == USER_INPUT_ENTER_CHAR)
	// 	// {
	// 	// 	usrInputBuf[usrInputIdx] = 0x00;
	// 	// 	UserInput_CommandEntered((char *) &usrInputBuf);
	// 	// 	break;
	// 	// }
	// 	if (c == PICO_ERROR_TIMEOUT)
	// 	{
	// 		break;
	// 	}
	// 	toggleLed();
	// 	usrInputBuf[usrInputIdx] = (char)c;
	// 	usrInputIdx++;
	// 	if (usrInputIdx > USER_INPUT_BUF_LEN)
	// 	{
	// 		usrInputIdx = 0; // TODO 
	// 	}
	// }
	usrInputCharAvail = true;
	toggleLed();
	test++;
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
		UserCommand_ProcessInputLine((char *) &usrInputBuf);
		usrInputIdx = 0;
	}
	else if (c == USER_INPUT_BKSPACE_CHAR)
	{
		if (usrInputIdx > 0)
		{
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

void UserInput_Init(void)
{
	memset((void *) &usrInputBuf, 0x00, sizeof(usrInputBuf));
	// stdio_set_chars_available_callback(UserInput_CharAvailCallback, NULL);
	// add_repeating_timer_ms(USER_INPUT_PRINT_CURRENT_CHAR_PERIOD_MS, UserInput_PrintCurrentCharTimerCallback, NULL, &printCurrentLineTimer);
	add_repeating_timer_ms(USER_INPUT_POLL_PERIOD_MS, UserInput_InputPollTimerCallback, NULL, &userInputPollTimer);
}

void UserInput_Service(void)
{
	// static stdio_driver_t *drivers;
	// for (stdio_driver_t *driver = drivers; driver; driver = driver->next)
	// {
	//
	// }
	// stdio_usb.in_chars((char *) &testchar, 1);
	// printf("TEST %d TESTCHAR %d\n", test, testchar);
	// if (usrInputCharAvail)
	// {
	// 	usrInputCharAvail = false;
	// 	char c = (char) 0x00;
	// 	while (true)
	// 	{
	// 		c = getchar_timeout_us(0);
	// 		if (c == PICO_ERROR_TIMEOUT)
	// 		{
	// 			break;
	// 		}
	// 		
	// 		// printf("%c", c);
	// 		toggleLed();
	// 		if (c == USER_INPUT_ENTER_CHAR)
	// 		{
	// 			usrInputBuf[usrInputIdx] = '\0';
	// 			printf("CMD: %s\n", usrInputBuf);
	// 			usrInputIdx = 0;
	// 			break;
	// 		}
	//
	// 		usrInputBuf[usrInputIdx] = c;
	// 		usrInputIdx++;
	// 	}
	// }
	// printf("%d->%d %c\n", usrInputPrintedIdx, usrInputIdx, getchar_timeout_us(0));
	// while (usrInputPrintedIdx != usrInputIdx)
	// {
	// 	printf("%c", usrInputBuf[usrInputPrintedIdx]);
	// 	usrInputPrintedIdx++;
	// 	toggleLed();
	// }
}
