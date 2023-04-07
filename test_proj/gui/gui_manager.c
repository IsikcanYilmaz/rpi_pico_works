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
									};
		
	guiManContext.programSelectionIdx = 0;
	char *programNames[Misc_GetNumPrograms() + 10];
	// for (uint16_t i; i < Misc_GetNumPrograms(); i++)
	// {
	// 	programNames[i] = Misc_GetProgramPtrByIdx(i)->name;
	// }
	for (int i = 0; i < Misc_GetNumPrograms() + 10; i++)
	{
		programNames[i] = ((i < Misc_GetNumPrograms()) ? Misc_GetProgramPtrByIdx(i)->name : ((i % 2) ? "ASD" : "QWE"));
	}

	guiManContext.programListBox = GuiList_Create(programNames, Misc_GetNumPrograms() + 10 /* Misc_GetNumPrograms() */, GuiMan_ProgramSelectedCallback, NULL);
	guiManContext.programListBox;
	guiManContext.programListBox.inFocus = true;
	guiManContext.inFocus = true;
	GuiMan_Start();
}

void GuiMan_Update(void)
{
	GuiList_Update(&guiManContext.programListBox);
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

GuiItemActions_e GuiMan_ButtonInputToGuiAction(Button_e b, ButtonGesture_e g)
{
	GuiItemActions_e guiAction = GUI_ITEM_ACTION_MAX;
	// map the button gesture to a gui item action and pass it on
	if (b == BUTTON_0 && g == GESTURE_SINGLE_TAP)
	{
		guiAction = GUI_ITEM_ACTION_UP;
	}
	else if (b == BUTTON_1 && g == GESTURE_SINGLE_TAP)
	{
		guiAction = GUI_ITEM_ACTION_DOWN;
	}
	else if (b == BUTTON_0 && g == GESTURE_DOUBLE_TAP)
	{
		guiAction = GUI_ITEM_ACTION_EXIT;
	}
	else if (b == BUTTON_1 && g == GESTURE_DOUBLE_TAP)
	{
		guiAction = GUI_ITEM_ACTION_SELECT;
	}
	return guiAction;
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
		GuiItemActions_e guiAction = GuiMan_ButtonInputToGuiAction(b, g);
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
