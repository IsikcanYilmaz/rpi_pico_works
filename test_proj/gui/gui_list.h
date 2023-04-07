#ifndef __GUI_LIST_H_
#define __GUI_LIST_H_
#include <stdint.h>
#include <stdbool.h>
#include "button.h"
#include "gui_item_interface.h"

#define GUI_LIST_MAX_ITEMS 20
#define GUI_LIST_MAX_VISIBLE_ITEMS 6
#define GUI_LIST_STRING_BEGIN_X 10
#define GUI_LIST_CHAR_PIX_H 10

typedef struct GuiList_t_
{
	bool inFocus;
	uint16_t cursor;
	uint8_t itemBoxHeight;
	uint8_t itemBoxWidth;
	uint16_t numItems;
	uint16_t drawIndexBegin;
	uint16_t drawIndexEnd;
	char* strings[GUI_LIST_MAX_ITEMS]; // TODO inefficient, see if we have malloc
	void (*itemSelectedCallback)(uint16_t selectedItemIdx);
	void (*exitedCallback)(void);
} GuiList_t;

GuiList_t GuiList_Create(char **strings, uint16_t numItems, void (*itemSelectedCallback)(uint16_t), void (*exitedCallback)(void));
void GuiList_Update(GuiList_t *l);
void GuiList_Draw(GuiList_t *l);
void GuiList_TakeActionInput(GuiList_t *l, GuiItemActions_e a);
#endif
