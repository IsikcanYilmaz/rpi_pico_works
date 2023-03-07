#define USER_COMMAND_MAX_ARGS 16

typedef struct UserCommand_s
{
	char *command;
	void (*fn)(uint8_t argc, void *argv);
	char *helptext;
} UserCommand_t;

void UserCommand_ProcessInputLine(char *buf);
void UserCommand_Reset(uint8_t argc, void *argv);
