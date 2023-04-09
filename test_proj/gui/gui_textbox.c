#include "gui_textbox.h"
#include "oled_manager.h"
#include <string.h>

GuiTextbox_t GuiTextbox_Create(char *string, void (*exitedCallback)(void))
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
				break;
			}
		case GUI_ITEM_ACTION_EXIT:
			{
				t->exitedCallback();	
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
