#include "gui_list.h"
#include "oled_manager.h"
#include <string.h>

static void GuiList_CalculateRenderItems(GuiList_t *l)
{
	if (l->numItems <= GUI_LIST_MAX_VISIBLE_ITEMS)
	{
		return;
	}

	if (l->cursor > l->drawIndexEnd)
	{
		l->drawIndexEnd = l->cursor;
		l->drawIndexBegin = l->cursor - (GUI_LIST_MAX_VISIBLE_ITEMS-1);
	}
	if (l->cursor < l->drawIndexBegin)
	{
		l->drawIndexBegin = l->cursor;
		l->drawIndexEnd = l->cursor + (GUI_LIST_MAX_VISIBLE_ITEMS-1);
	}
}

GuiList_t GuiList_Create(char **strings, uint16_t numItems, void (*itemSelectedCallback)(uint16_t), void (*exitedCallback)(void))
{
	GuiList_t l;
	l.inFocus = false;
	l.cursor = 0;
	l.numItems = numItems;
	l.itemSelectedCallback = itemSelectedCallback;
	l.exitedCallback = exitedCallback;
	l.drawIndexBegin = 0;
	l.drawIndexEnd = 0;
	GuiList_CalculateRenderItems(&l);
	GuiList_SetStrings(&l, strings, numItems);
	return l;
}

void GuiList_SetStrings(GuiList_t *l, char **strings, uint16_t numItems)
{
	if (numItems == 0)
	{
		return;
	}

	memcpy(l->strings, strings, sizeof(char *) * numItems);
	if (numItems-1 < l->cursor)
	{
		l->cursor = numItems-1;
	}
	l->numItems = numItems;
	GuiList_CalculateRenderItems(l);
}

void GuiList_SetCursor(GuiList_t *l, uint16_t c)
{
	if (c < l->numItems)
	{
		l->cursor = c;
	}
	else
	{
		printf("Guilist setcursor bad val! %d\n", c);
	}
}

void GuiList_Update(GuiList_t *l)
{
	if (!l->inFocus)
	{
		return;
	}
	// TODO
}

void GuiList_Draw(GuiList_t *l)
{
	// First find which index range we should draw
	uint8_t cursor = l->cursor;
	uint8_t x, y, rectW, rectH;
	rectW = 117;
	rectH = GUI_LIST_CHAR_PIX_H;

	if (l->numItems == 0)
	{
		OledMan_DrawRectangle(GUI_LIST_STRING_BEGIN_X, GUI_LIST_CHAR_PIX_H, rectW, rectH, 0);	
		return;
	}

	// printf("LIST C:%d, BEGIN:%d, END %d\n", cursor, l->drawIndexBegin, l->drawIndexEnd);
	
	// Draw our items
	for (uint8_t currDrawnIdx = 0; currDrawnIdx < GUI_LIST_MAX_VISIBLE_ITEMS; currDrawnIdx++)
	{
		uint8_t itemAbsoluteIdx = currDrawnIdx + l->drawIndexBegin;
		if (itemAbsoluteIdx >= l->numItems)
		{
			break;
		}
		x = GUI_LIST_STRING_BEGIN_X;
		y = currDrawnIdx * GUI_LIST_CHAR_PIX_H;
		char *name = l->strings[itemAbsoluteIdx];
		OledMan_SetColor(WHITE);
		if (cursor == itemAbsoluteIdx)
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
			OledMan_DrawString(x, y + 2, name);
		}
	}
}

void GuiList_TakeActionInput(GuiList_t *l, GuiItemActions_e a)
{
	switch(a)
	{
		case GUI_ITEM_ACTION_UP:
			{
				if (l->cursor > 0)
				{
					l->cursor--;
				}
				else
				{
					l->cursor = l->numItems-1;
				}
				GuiList_CalculateRenderItems(l);
				break;
			}
		case GUI_ITEM_ACTION_DOWN:
			{
				if (l->cursor >= l->numItems-1)
				{
					l->cursor = 0;
				}
				else
				{
					l->cursor++;
				}
				GuiList_CalculateRenderItems(l);
				break;
			}
		case GUI_ITEM_ACTION_SELECT:
			{
				// printf("GUILIST 0x%x SELECTIDX %d \n", l, l->cursor);
				if (l->itemSelectedCallback)
				{
					l->itemSelectedCallback(l->cursor);
				}
				break;
			}
		case GUI_ITEM_ACTION_EXIT:
			{
				if (l->exitedCallback)
				{
					l->exitedCallback();	
				}
				break;
			}
		default:
			{
				
			}
	}
}

GuiItemActions_e GuiList_DefaultButtonMap(Button_e b, ButtonGesture_e g)
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
