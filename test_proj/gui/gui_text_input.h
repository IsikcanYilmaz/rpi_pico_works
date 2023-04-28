#ifndef __GUI_TEXT_INPUT_H
#define __GUI_TEXT_INPUT_H
#include <stdint.h>
#include <stdbool.h>
#include "button.h"
#include "gui_item_interface.h"

#define GUI_TEXT_INPUT_MAX_LEN 30
#define GUI_TEXT_INFO_TEXT_MAX_LEN 30

#define GUI_TEXT_INPUT_INFO_Y_LOC 3
#define GUI_TEXT_INPUT_STRING_Y_LOC 30
#define GUI_TEXT_INPUT_SELECTOR_Y_LOC 50

#define GUI_LIST_CHAR_PIX_H 10
#define GUI_LIST_CHAR_PIX_W 8

#define GUI_LIST_NUM_VISIBLE_INPUT_CHARS 15

typedef struct GuiTextInput_t_
{
	bool inFocus;
	char buf[GUI_TEXT_INPUT_MAX_LEN + 1]; // Maybe make this a ptr? // +1s for the \0
	char infoTextBuf[GUI_TEXT_INFO_TEXT_MAX_LEN + 1];
	uint8_t cursor;
	uint8_t inputLen;
	uint8_t alphabetLen;
	bool upper;
	void (*exitedCallback)(void);
} GuiTextInput_t;

GuiTextInput_t GuiTextInput_Create(void (*exitedCallback)(void));
void GuiTextInput_Update(GuiTextInput_t *t);
void GuiTextInput_Draw(GuiTextInput_t *t);
void GuiTextInput_TakeActionInput(GuiTextInput_t *t, GuiItemActions_e a);
void GuiTextInput_SetString(GuiTextInput_t *t, char *string);
GuiItemActions_e GuiTextInput_DefaultButtonMap(Button_e b, ButtonGesture_e g);
void GuiTextInput_SetInfoText(GuiTextInput_t *t, char *string);

#endif
