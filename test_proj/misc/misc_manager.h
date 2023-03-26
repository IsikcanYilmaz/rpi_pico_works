#include <stdbool.h>
#include <stdint.h>

typedef enum MiscIdx_e_
{
	MISC_SCREEN_SAVER,
	MISC_GAME_OF_LIFE,
	MISC_MAX,
} MiscIdx_e;

typedef struct Misc_s
{
	char *name;
	bool (*init)(void *arg);
	void (*deinit)(void);
	void (*start)(void);
	void (*stop)(void);
	void (*update)(void);
	void (*draw)(void);
	bool isRunning;
	uint16_t updatePeriodMs;
	// TODO at some point make all misc programs use the same memory pool.
} Misc_t;

void Misc_ManagerInit(void);
void Misc_StopProgram(void);
void Misc_StartProgram(MiscIdx_e idx, void *arg);
void Misc_TakeTextInput(uint8_t argc, char **argv);
void Misc_TimerStart(void);
void Misc_TimerStop(void);
void Misc_PrintPrograms(void);