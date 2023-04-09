#ifndef __GUI_TEXTBOX_H
#define __GUI_TEXTBOX_H
#include <stdint.h>
#include <stdbool.h>
#include "button.h"
#include "gui_item_interface.h"

typedef struct GuiTextbox_t_
{
	bool inFocus;
	char *string;
	uint16_t cursor;
	void (*exitedCallback)(void);
} GuiTextbox_t;

GuiTextbox_t GuiTextbox_Create(char *string, void (*exitedCallback)(void));
void GuiTextbox_Update(GuiTextbox_t *t);
void GuiTextbox_Draw(GuiTextbox_t *t);
void GuiTextbox_TakeActionInput(GuiTextbox_t *t, GuiItemActions_e a);
void GuiTextbox_SetString(GuiTextbox_t *t, char *string);
#endif