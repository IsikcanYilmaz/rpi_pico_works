#include <stdbool.h>

typedef struct OledManContext_s
{
	bool initialized;
} OledManContext_t;

void OledMan_Init(void);
void OledMan_Test0(void);
void OledMan_Test1(void);
