#include "misc_test.h"
#include "gui_textbox.h"
#include "oled_manager.h"

GuiTextInput_t textInput;
GuiList_t guiList;
GuiTextbox_t guiTextbox;
char *testStrs[] = {"aaa", "bbb", "ccc", "ddd", "eee", "fff", "ggg", "hhh", "iii"};

static void MiscTest_TextInputExited(void)
{
	printf("MISC TEST Text input exited! buf:%s\n", textInput.buf);
}

static void MiscTest_TimedTextboxTimeoutCallback(bool ok)
{
	printf("%s\n", __FUNCTION__);
	guiTextbox.inFocus = false;
	guiList.inFocus = true;
}

bool MiscTest_Init(void *arg)
{
	// textInput = GuiTextInput_Create(MiscTest_TextInputExited);
	// GuiTextInput_SetInfoText(&textInput, "TEST TEST TEST TEST TEST");

	guiList = GuiList_Create(testStrs, 9, NULL, NULL);
	guiTextbox = GuiTextbox_Create("TESTTEST\nTIMEDMESSAGE", 5000, MiscTest_TimedTextboxTimeoutCallback);
	guiTextbox.inFocus = true;
	guiList.inFocus = false;

	return true;
}
void MiscTest_Deinit(void)
{
}

void MiscTest_Update(void)
{
	GuiTextbox_Update(&guiTextbox);	
}

void MiscTest_Draw(void)
{
	OledMan_ClearBuf();
	// GuiTextInput_Draw(&textInput);
	// GuiList_Draw(&guiList);
	if (guiTextbox.inFocus)
	{
		GuiTextbox_Draw(&guiTextbox);
	}
	else if (guiList.inFocus)
	{
		GuiList_Draw(&guiList);
	}
}

void MiscTest_Start(void)
{
}

void MiscTest_Stop(void)
{
}

void MiscTest_ButtonInput(Button_e b, ButtonGesture_e g)
{
	// GuiItemActions_e guiItemAction = GuiTextInput_DefaultButtonMap(b, g);
	// GuiTextInput_TakeActionInput(&textInput, guiItemAction);

	GuiItemActions_e guiAction = GuiList_DefaultButtonMap(b, g);
	GuiList_TakeActionInput(&guiList, guiAction);

}

