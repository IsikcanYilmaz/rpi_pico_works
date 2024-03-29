#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "usr_commands.h"
#include "misc_manager.h"
#include "test_functionality.h"
#include "wifi.h"
#include "oled_manager.h"
// #include "GUI_Paint.h"
#include "paint.h"
#include "tcp_recv_picture/tcp_recv_picture.h"

UserCommand_t userCommands[] = {
	{"help", UserCommand_Help, "Print help text"},
	{"reset", UserCommand_Reset, "Reset the board into boot mode"},
	{"test", UserCommand_Test, "test"},
	{"time", UserCommand_GetTime, "Get timestamp"},
	{"led", UserCommand_LedSet, "Set Led"},
	{"loopback", UserCommand_Loopback, "Loopback"},
	{"wifi", UserCommand_Wifi, "Wifi commands"},
	{"oled", UserCommand_Oled, "Oled commands"},
	{"misc", UserCommand_Misc, "Misc programs"}
};

void UserCommand_PrintCommand(uint16_t argc, char **argv)
{
	printf("COMMAND: ");
	for (uint16_t i = 0; i < argc; i++)
	{
		printf("%s ", argv[i]);
	}
	printf("\n");
}

bool UserCommand_ProcessCommand(uint16_t argc, char **argv)
{
	// We have the command and args parsed now. do a lookup to see
	// which command is entered and call its function
	for (uint16_t i = 0; i < (sizeof(userCommands) / sizeof(userCommands[0])); i++)
	{
		if (strcmp(userCommands[i].command, argv[0]) == 0)
		{
			// printf("COMMAND %s argc %d\n", argv[0], argc); // TODO
			userCommands[i].fn(argc, argv);
			return true;
		}
	}
	// printf("Command not found\n");
	return false;
}

// COMMANDS // 

void UserCommand_Help(uint8_t argc, char **argv)
{
	printf("Available commands:\n");
	for (uint16_t i = 0; i < sizeof(userCommands)/sizeof(userCommands[0]); i++)
	{
		printf("%d) %s : %s\n", i, userCommands[i].command, userCommands[i].helptext);
	}
}

void UserCommand_Reset(uint8_t argc, char **argv)
{
	ASSERT_ARGS(2);
	if (strcmp(argv[1], "boot") == 0)
	{
		softwareReset(true);
		return;
	}
	else if (strcmp(argv[1], "sw") == 0)
	{
		softwareReset(false);
		return;
	}
}

void UserCommand_Test(uint8_t argc, char **argv)
{
	OledMan_DrawRectangle(0, 0, 50, 50, true);
}

void UserCommand_GetTime(uint8_t argc, char **argv)
{
	uint32_t t = time_us_32()/1000;
	printf("time %d\n", t);
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

void UserCommand_Wifi(uint8_t argc, char **argv) // TODO make this mode modular. what is this pile of ifs dawg
{
	ASSERT_ARGS(2);

	// args
	if (strcmp(argv[1], "info") == 0)
	{
		Wifi_PrintInfo();
		return;
	}
	if (strcmp(argv[1], "mode") == 0)
	{
		ASSERT_ARGS(3)
		bool ret = false;
		if (strcmp(argv[2], "none") == 0)
		{
			ret = Wifi_UnsetCurrentMode();
		}
		else if (strcmp(argv[2], "ap") == 0)
		{
			ret = Wifi_SetMode(WIFI_MODE_ACCESS_POINT);
		}
		else if (strcmp(argv[2], "sta") == 0)
		{
			ret = Wifi_SetMode(WIFI_MODE_STATION);
		}
		else
		{
			printf("Bad arg to \"wifi mode\". Avail modes: none, ap, sta\n");
			BAD_ARG();
			return;
		}
		printf("Set mode result: %d\n", ret);
		return;
	}
	if (strcmp(argv[1], "routine") == 0)
	{
		ASSERT_ARGS(3)
		bool ret = false;
		if (strcmp(argv[2], "none") == 0)
		{
			ret = Wifi_UnsetCurrentRoutine();
		}
		else if (strcmp(argv[2], "ap_example") == 0)
		{
			ret = Wifi_SetRoutine(WIFI_ROUTINE_ACCESS_POINT_EXAMPLE, NULL);
		}
		else if (strcmp(argv[2], "tcp_recv") == 0)
		{
			if (argc == 5)
			{
				TcpRecvInitArgs_s args = (TcpRecvInitArgs_s) {.ipAddr = argv[3], .port = atoi(argv[4]), .pictureReceivedCallback = NULL};
				ret = Wifi_SetRoutine(WIFI_ROUTINE_TCP_RECV_PICTURE, &args);
			}
			else
			{
				ret = Wifi_SetRoutine(WIFI_ROUTINE_TCP_RECV_PICTURE, NULL);
			}
		}
		else
		{
			printf("Bad arg to \"wifi routine\". Avail modes: none, ap_example\n");
			BAD_ARG();
			return;
		}
		printf("Set mode result: %d\n", ret);
		return;
	}
	if (strcmp(argv[1], "scan") == 0)
	{
		printf("Wifi scan\n");
		Wifi_Scan();
		return;
	}
	if (strcmp(argv[1], "list") == 0)
	{
		Wifi_PrintRecords();
		return;
	}
	if (strcmp(argv[1], "connect") == 0)
	{
		ASSERT_ARGS(4);
		Wifi_Connect(argv[2], argv[3]);
		return;
	}
	else
	{
		BAD_ARG();
		return;
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
		OledMan_DrawLine(x1, y1, x2, y2);
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
		uint16_t x = atoi(argv[2]);
		uint16_t y = atoi(argv[3]);
		uint16_t w = atoi(argv[4]);
		uint16_t h = atoi(argv[5]);
		bool fill = (atoi(argv[6]) > 0);
		OledMan_DrawRectangle(x, y, w, h, fill);
		printf("Draw rect x%d y%d w%d h%d f%d\n", x, y, w, h, fill);
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
		uint8_t val = 1;
		if (argc > 4) val = atoi(argv[4]);
		OledMan_SetPixel(x, y, val);
		printf("Draw pixel to x%d y%d v%d\n", x, y, val);
	}
	else
	{
		BAD_ARG();
	}
}

void UserCommand_Misc(uint8_t argc, char **argv)
{
	ASSERT_ARGS(2);
	Misc_TakeTextInput(argc-1, &argv[1]); // pass only the arguments to "misc"
}
