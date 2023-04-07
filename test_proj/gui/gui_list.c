#include "gui_list.h"
#include "oled_manager.h"
#include <string.h>

GuiList_t GuiList_Create(char **strings, uint16_t numItems, void (*itemSelectedCallback)(uint16_t), void (*exitedCallback)(void))
{
	GuiList_t l;
	l.inFocus = false;
	l.cursor = 0;
	l.numItems = numItems;
	l.itemSelectedCallback = itemSelectedCallback;
	l.exitedCallback = exitedCallback;
	memcpy(&l.strings, strings, sizeof(char *) * numItems);
	return l;
}

void GuiList_Update(GuiList_t *l)
{
	//
}

void GuiList_Draw(GuiList_t *l)
{
	// First find which index range we should draw
	uint8_t cursor = l->cursor;
	uint8_t drawIndexBegin = ((cursor < GUI_LIST_MAX_VISIBLE_ITEMS - 1) ? 0 : cursor - GUI_LIST_MAX_VISIBLE_ITEMS - 1);
	uint8_t drawIndexEnd = drawIndexBegin + GUI_LIST_MAX_VISIBLE_ITEMS;
	uint8_t x, y, rectW, rectH;
	rectW = 117;
	rectH = GUI_LIST_CHAR_PIX_H;
	
	// Draw our items
	for (uint8_t i = 0; i <= (l->numItems <= GUI_LIST_MAX_VISIBLE_ITEMS ? l->numItems-1 : GUI_LIST_MAX_VISIBLE_ITEMS); i++)
	{
		uint8_t itemIdx = drawIndexBegin + i;
		x = GUI_LIST_STRING_BEGIN_X;
		y = i * GUI_LIST_CHAR_PIX_H;
		char *name = l->strings[i];
		// printf("%s | num%d\n", name);
		OledMan_SetColor(WHITE);
		if (cursor == itemIdx)
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
