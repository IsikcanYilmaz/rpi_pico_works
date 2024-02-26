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
	void (*exitedCallback)(bool ok);
	uint32_t timeoutMs;
	uint32_t liveUntilMs;
	bool isTimed;
} GuiTextbox_t;

GuiTextbox_t GuiTextbox_Create(char *string, uint32_t timeoutMs, void (*exitedCallback)(bool ok));
void GuiTextbox_Update(GuiTextbox_t *t);
void GuiTextbox_Draw(GuiTextbox_t *t);
void GuiTextbox_TakeActionInput(GuiTextbox_t *t, GuiItemActions_e a);
void GuiTextbox_SetString(GuiTextbox_t *t, char *string);
void GuiTextbox_SetInFocus(GuiTextbox_t *t, bool inFocus);
void GuiTextbox_SetTimer(GuiTextbox_t *t, uint32_t timeoutMs);
GuiItemActions_e GuiTextbox_DefaultButtonMap(Button_e b, ButtonGesture_e g);
#endif
