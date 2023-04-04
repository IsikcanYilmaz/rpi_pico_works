#include "oled_manager.h"
#include "hardware/timer.h"
#include "misc_manager.h"
#include "button.h"
#include "gui_manager.h"

GuiManContext_t guiManContext;

static bool GuiMan_UpdateTimerCallback(struct repeating_timer_t *t)
{
	GuiMan_Update();
	GuiMan_Draw();
	return true;
}

void GuiMan_Init(void)
{
	guiManContext = (GuiManContext_t) {
									.running = false,
									};
		
	guiManContext.programSelectionIdx = 0;
	GuiMan_Start();
}

void GuiMan_Update(void)
{
	
}

void GuiMan_Draw(void)
{
	OledMan_ClearBuf();
	uint8_t x, y, rectW, rectH;
	rectW = 100;
	rectH = CHAR_PIX_H;
	for (uint16_t i = 0; i < Misc_GetNumPrograms(); i++)
	{
		x = 10;
		y = i * CHAR_PIX_H;
		char *name = ((MiscProgram_t *) Misc_GetProgramPtrByIdx(i))->name;
		OledMan_SetColor(WHITE);
		if (i == guiManContext.programSelectionIdx)
		{
			// draw full rect white
			// draw string black
			OledMan_SetColor(WHITE);
			OledMan_DrawRectangle(x, y, rectW, rectH, 1);
			OledMan_SetColor(BLACK);
			OledMan_DrawString(x, y + 2, name);
			OledMan_SetColor(WHITE);
			OledMan_DrawRectangle(5, y + 4, 2, 2, 1);
		}
		else
		{
			// draw full rect black 
			// draw string white 
			OledMan_DrawRectangle(x, y, rectW, rectH, 0);
			OledMan_SetColor(WHITE);
			OledMan_DrawString(x, y, name);
		}
	}
}

void GuiMan_Start(void)
{
	GuiMan_StartPollTimer();
	guiManContext.running = true;
}

void GuiMan_Stop(void)
{
	GuiMan_StopPollTimer();
	guiManContext.running = false;
}

void GuiMan_StartPollTimer(void)
{
	add_repeating_timer_ms(GUI_MAN_POLL_PERIOD_MS, GuiMan_UpdateTimerCallback, NULL, &(guiManContext.guiManUpdateTimer));
}

void GuiMan_StopPollTimer(void)
{
	cancel_repeating_timer(&(guiManContext.guiManUpdateTimer));
}

void GuiMan_TakeButtonInput(Button_e b, ButtonGesture_e g)
{
	// TODO. lookin ugly.  
	if ((g == GESTURE_VLONG_PRESS || g == GESTURE_VVLONG_PRESS) && Misc_IsProgramRunning())
	{
		Misc_StopProgram();
		if (!guiManContext.running) GuiMan_Start();
		return;
	}

	///////
	if (Misc_IsProgramRunning())
	{
		Misc_TakeButtonInput(b, g);
		return;
	}	

	if (g == GESTURE_SINGLE_TAP)
	{
		if (b == BUTTON_0)
		{
			guiManContext.programSelectionIdx -= 1;
			if (guiManContext.programSelectionIdx >= Misc_GetNumPrograms())
			{
				guiManContext.programSelectionIdx = Misc_GetNumPrograms() - 1;
			}
		}
		else if (b == BUTTON_1)
		{
			guiManContext.programSelectionIdx += 1;
			if (guiManContext.programSelectionIdx >= Misc_GetNumPrograms())
			{
				guiManContext.programSelectionIdx = 0;
			}
		}
	}
	
	if (g == GESTURE_DOUBLE_TAP)
	{
		if (b == BUTTON_0)
		{
			Misc_StopProgram();
			GuiMan_Start();
		}
		else if (b == BUTTON_1)
		{
			GuiMan_Stop();
			Misc_StartProgram(guiManContext.programSelectionIdx, NULL);
		}
	}
}
