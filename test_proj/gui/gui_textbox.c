#include "gui_textbox.h"
#include "hardware/timer.h"
#include "oled_manager.h"
#include <string.h>

/*
* Creates a "Textbox" structure
* The misc program needs to periodically call update on this module and handle inFocus. 
* The timer will be set from the time GuiTextbox_SetInFocus is called
* @param string String to display
* @param timeoutMs How long to display the message. If 0 there is no timeout
* @param exitedCallback Callback to call once exit happens. NULL for no callback
*/
GuiTextbox_t GuiTextbox_Create(char *string, uint32_t timeoutMs, void (*exitedCallback)(bool ok))
{
	GuiTextbox_t ret;
	ret.inFocus = false;
	ret.string = string;
	ret.exitedCallback = exitedCallback;
	ret.cursor = 0;
	ret.timeoutMs = timeoutMs;
	ret.liveUntilMs = timeoutMs + (time_us_32() / 1000);
	ret.isTimed = (timeoutMs > 0) ? true : false;
	// printf("%s timeout %d liveuntil %d isTimed %d\n", __FUNCTION__, timeoutMs, ret.liveUntilMs, ret.isTimed);
	return ret;
}

void GuiTextbox_Update(GuiTextbox_t *t)
{
	// If this is a timed pop up, check the time. if time out, exit
	if (t->isTimed && t->liveUntilMs < (time_us_32()/1000))
	{
		GuiTextbox_TakeActionInput(t, GUI_ITEM_ACTION_EXIT);
		t->isTimed = false;
	}
}

void GuiTextbox_Draw(GuiTextbox_t *t)
{
	// draw frame
	OledMan_DrawRectangle(0, 0, 127, 63, 0);

	// Draw our string // TODO handle scrolling and newlines
	OledMan_DrawString(1, 1, t->string);
}

void GuiTextbox_TakeActionInput(GuiTextbox_t *t, GuiItemActions_e a)
{
	switch(a)
	{
		case GUI_ITEM_ACTION_UP:
			{
				break;
			}
		case GUI_ITEM_ACTION_DOWN:
			{
				break;
			}
		case GUI_ITEM_ACTION_SELECT:
			{
				t->exitedCallback(true);	
				break;
			}
		case GUI_ITEM_ACTION_EXIT:
			{
				t->exitedCallback(false);	
				break;
			}
		default:
			{
				
			}
	}
}

void GuiTextbox_SetString(GuiTextbox_t *t, char *string)
{
	t->string = string;
	t->cursor = 0;
}

void GuiTextbox_SetInFocus(GuiTextbox_t *t, bool inFocus)
{
	// If this is a timed textbox, and we're going in focus, its time to set the time to live field
	if (t->isTimed && inFocus)
	{
		t->liveUntilMs = t->timeoutMs + (time_us_32() / 1000);
		printf("Setting textbox in focus. liveuntil %d now %d\n", t->liveUntilMs);
	}

	t->inFocus = inFocus;
}

GuiItemActions_e GuiTextbox_DefaultButtonMap(Button_e b, ButtonGesture_e g)
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

void GuiTextbox_SetTimer(GuiTextbox_t *t, uint32_t timeoutMs)
{
	t->timeoutMs = timeoutMs;
	// ret.liveUntilMs = timeoutMs + (time_us_32() / 1000);
	t->isTimed = (timeoutMs > 0) ? true : false;
}
