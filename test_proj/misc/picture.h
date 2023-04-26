#include <stdbool.h>
#include "button.h"

#define PICTURE_UPDATE_PERIOD_MS 20

bool Picture_Init(void *arg);
void Picture_Deinit(void);
void Picture_Update(void);
void Picture_Draw(void);
void Picture_Start(void);
void Picture_Stop(void);
void Picture_ButtonInput(Button_e b, ButtonGesture_e g);

void Picture_SetUpdateHappened(void);

