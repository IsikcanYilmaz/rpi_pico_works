#include "gui_text_input.h"
#include "oled_manager.h"
#include <string.h>
#include <ctype.h>

char *alphabet = "abcdefghijklmnopqrstuvwxyz0123456789!?@#$%^&*()_+-=.;:<>";

GuiTextInput_t GuiTextInput_Create(void (*exitedCallback)(void))
{
	GuiTextInput_t t;
	t.exitedCallback = exitedCallback;
	t.cursor = 0;
	t.inFocus = false;
	t.inputLen = 0;
	t.alphabetLen = strlen(alphabet);
	t.upper = false;
	memset(t.buf, 0x00, GUI_TEXT_INPUT_MAX_LEN + 1);
	GuiTextInput_ClearBuf(&t);
	GuiTextInput_ClearInfoText(&t);
	return t;
}

void GuiTextInput_ClearBuf(GuiTextInput_t *t)
{
	memset(t->buf, 0x00, GUI_TEXT_INPUT_MAX_LEN + 1);
	t->inputLen = 0;
}

void GuiTextInput_ClearInfoText(GuiTextInput_t *t)
{
	memset(t->infoTextBuf, 0x00, GUI_TEXT_INFO_TEXT_MAX_LEN + 1);
}

void GuiTextInput_Update(GuiTextInput_t *t)
{
	//
}

void GuiTextInput_Draw(GuiTextInput_t *t)
{
	static bool cursorVisible = true;
	// Draw Exterior frame
	OledMan_DrawRectangle(0, 0, 127, 63, 0);

	// Draw Info text
	OledMan_DrawString(2, GUI_TEXT_INPUT_INFO_Y_LOC, t->infoTextBuf);

	// Draw current input
	OledMan_DrawRectangle(2, GUI_TEXT_INPUT_STRING_Y_LOC - 3, 124, GUI_LIST_CHAR_PIX_H + 2, 0);
	for (uint8_t i = 0; i < t->inputLen; i++)
	{
		OledMan_DrawChar(2 + GUI_LIST_CHAR_PIX_W * i, GUI_TEXT_INPUT_STRING_Y_LOC, t->buf[i]);
	}
	if (cursorVisible) OledMan_DrawChar(2 + GUI_LIST_CHAR_PIX_W * (t->inputLen), GUI_TEXT_INPUT_STRING_Y_LOC + 1, '_');

	// Draw char selection
	uint8_t drawCharBeginIdx = (t->cursor - (GUI_LIST_NUM_VISIBLE_INPUT_CHARS/2) >= 0) 
		? (t->cursor - (GUI_LIST_NUM_VISIBLE_INPUT_CHARS/2)) 
		: ((t->alphabetLen) + (t->cursor - GUI_LIST_NUM_VISIBLE_INPUT_CHARS/2));
	uint8_t drawCharEndIdx = t->cursor + (GUI_LIST_NUM_VISIBLE_INPUT_CHARS/2) % t->alphabetLen; 
	uint8_t drawCharIdx = drawCharBeginIdx;
	for (uint8_t i = 0; i < GUI_LIST_NUM_VISIBLE_INPUT_CHARS; i++)
	{
		if (drawCharIdx == t->cursor)
		{
			OledMan_DrawRectangle(GUI_LIST_CHAR_PIX_W * i, GUI_TEXT_INPUT_SELECTOR_Y_LOC - 2, GUI_LIST_CHAR_PIX_W, GUI_LIST_CHAR_PIX_H, 0);
		}
		OledMan_DrawChar(2 + GUI_LIST_CHAR_PIX_W * i, GUI_TEXT_INPUT_SELECTOR_Y_LOC, (t->upper) ? toupper(alphabet[drawCharIdx]) : alphabet[drawCharIdx]);
		drawCharIdx = (drawCharIdx + 1) % t->alphabetLen;
	}
	// cursorVisible = !cursorVisible;
}

void GuiTextInput_TakeActionInput(GuiTextInput_t *t, GuiItemActions_e a)
{
	switch(a)
	{
		case GUI_ITEM_ACTION_LEFT:
		{
			t->cursor = (t->cursor - 1) >= 0 ? t->cursor - 1 : t->alphabetLen-1;
			break;
		}
		case GUI_ITEM_ACTION_RIGHT:
		{
			t->cursor = (t->cursor + 1) % t->alphabetLen;
			break;
		}
		case GUI_ITEM_ACTION_SELECT:
		{
			t->buf[t->inputLen] = (t->upper) ? toupper(alphabet[t->cursor]) : alphabet[t->cursor];
			t->inputLen++;
			break;
		}
		case GUI_ITEM_ACTION_BACKSPACE:
		{
			if (t->inputLen == 0)
			{
				break;
			}
			t->buf[t->inputLen - 1] = '\0';
			t->inputLen--;
			break;
		}
		case GUI_ITEM_ACTION_ALT1:
		{
			t->upper = !t->upper;
			break;
		}
		case GUI_ITEM_ACTION_EXIT:
		{
			if (t->exitedCallback)
			{
				t->exitedCallback();
			}
			break;
		}
		default:
		{
			break;
		}
	}
	printf("GuiTextInput curr buf %s\n", t->buf);
}

void GuiTextInput_SetInfoText(GuiTextInput_t *t, char *string)
{
	strncpy(t->infoTextBuf, string, GUI_TEXT_INFO_TEXT_MAX_LEN);
}

GuiItemActions_e GuiTextInput_DefaultButtonMap(Button_e b, ButtonGesture_e g)
{
	GuiItemActions_e guiItemAction = GUI_ITEM_ACTION_MAX;
	if (g == GESTURE_SINGLE_TAP)
	{
		if (b == BUTTON_0)
		{
			guiItemAction = GUI_ITEM_ACTION_LEFT;
		}
		else if (b == BUTTON_1)
		{
			guiItemAction = GUI_ITEM_ACTION_RIGHT;
		}
	}
	else if (g == GESTURE_DOUBLE_TAP)
	{
		if (b == BUTTON_0)
		{
			guiItemAction = GUI_ITEM_ACTION_BACKSPACE;
		}
		else if (b == BUTTON_1)
		{
			guiItemAction = GUI_ITEM_ACTION_SELECT;
		}
	}
	else if (g == GESTURE_TRIPLE_TAP)
	{
		if (b == BUTTON_0)
		{
			guiItemAction = GUI_ITEM_ACTION_ALT1;
		}
		else if (b == BUTTON_1)
		{
			guiItemAction = GUI_ITEM_ACTION_EXIT;
		}
	}
	return guiItemAction;
}

void GuiTextInput_SetString(GuiTextInput_t *t, char *string)
{
	memset(t->buf, 0x00, GUI_TEXT_INPUT_MAX_LEN);
	strcpy(t->buf, string);
	t->inputLen = strlen(string);
	printf("GuiTextInput setting string to %s\n", t->buf);
}
