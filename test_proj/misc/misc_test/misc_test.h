#include <stdbool.h>
#include <stdint.h>
#include "misc_manager.h"
#include "button.h"
#include "gui_manager.h"
#include "gui_text_input.h"

#define MISC_TEST_UPDATE_PERIOD_MS 250

bool MiscTest_Init(void *arg);
void MiscTest_Deinit(void);
void MiscTest_Update(void);
void MiscTest_Draw(void);
void MiscTest_Start(void);
void MiscTest_Stop(void);
void MiscTest_ButtonInput(Button_e b, ButtonGesture_e g);

