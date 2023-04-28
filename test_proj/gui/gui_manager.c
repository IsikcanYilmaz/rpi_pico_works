#include "oled_manager.h"
#include "hardware/timer.h"
#include "misc_manager.h"
#include "button.h"
#include "gui_manager.h"
#include "test_functionality.h"

GuiManContext_t guiManContext;

static bool GuiMan_UpdateTimerCallback(struct repeating_timer_t *t)
{
	GuiMan_Update();
	GuiMan_Draw();
	return true;
}

static void GuiMan_ProgramSelectedCallback(uint16_t selectedItemIdx)
{
	printf("Program list selected item:%d\n", selectedItemIdx);
	if (selectedItemIdx < Misc_GetNumPrograms())
	{
		GuiMan_Stop();
		Misc_StartProgram(selectedItemIdx, NULL);
	}
	else
	{
		printf("Program list returned bad value %d\n", selectedItemIdx);
	}
}

void GuiMan_Init(void)
{
	guiManContext = (GuiManContext_t) {
									.running = false,
									.inFocus = false,
									};
		
	guiManContext.programSelectionIdx = 0;
	char *programNames[Misc_GetNumPrograms()];
	for (uint16_t i; i < Misc_GetNumPrograms(); i++)
	{
		programNames[i] = Misc_GetProgramPtrByIdx(i)->name;
	}

	guiManContext.programListBox = GuiList_Create(programNames, Misc_GetNumPrograms(), GuiMan_ProgramSelectedCallback, NULL);
	guiManContext.programListBox;
	guiManContext.programListBox.inFocus = true;
	GuiMan_Start();
}

void GuiMan_Update(void)
{
}

void GuiMan_Draw(void)
{
	OledMan_ClearBuf();
	GuiList_Draw(&(guiManContext.programListBox));
}

void GuiMan_Start(void)
{
	GuiMan_StartPollTimer();
	guiManContext.running = true;
	guiManContext.inFocus = true;
}

void GuiMan_Stop(void)
{
	GuiMan_StopPollTimer();
	guiManContext.running = false;
	guiManContext.inFocus = false;
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
	if (g == GESTURE_SINGLE_TAP && b == BUTTON_BOTH)
	{
		softwareReset(true);
		return;
	}

	// If the gui is on, process button action and pass it
	if (guiManContext.inFocus)
	{
		GuiItemActions_e guiAction = GuiList_DefaultButtonMap(b, g);
		GuiList_TakeActionInput(&guiManContext.programListBox, guiAction);
		return;
	}

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
}
