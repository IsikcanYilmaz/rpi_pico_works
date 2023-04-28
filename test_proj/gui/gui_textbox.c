#include "gui_textbox.h"
#include "oled_manager.h"
#include <string.h>

GuiTextbox_t GuiTextbox_Create(char *string, void (*exitedCallback)(bool ok))
{
	GuiTextbox_t ret;
	ret.inFocus = false;
	ret.string = string;
	ret.exitedCallback = exitedCallback;
	ret.cursor = 0;
	return ret;
}

void GuiTextbox_Update(GuiTextbox_t *t)
{
	//
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
