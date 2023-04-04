#include <stdbool.h>
#include <stdint.h>

#define GUI_MAN_POLL_PERIOD_MS 20

#define GUI_CHAR_PIX_W 5
#define GUI_CHAR_PIX_H 10

typedef struct GuiManContext_t_
{
	struct repeating_timer guiManUpdateTimer;
	uint8_t programSelectionIdx;
	bool running;
} GuiManContext_t;

void GuiMan_Init(void);
void GuiMan_Update(void);
void GuiMan_Draw(void);
void GuiMan_Start(void);
void GuiMan_Stop(void);
void GuiMan_StartPollTimer(void);
void GuiMan_StopPollTimer(void);
void GuiMan_TakeButtonInput(Button_e b, ButtonGesture_e g);
