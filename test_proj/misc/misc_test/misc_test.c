#include "misc_test.h"
#include "oled_manager.h"

GuiTextInput_t textInput;
GuiList_t guiList;
char *testStrs[] = {"aaa", "bbb", "ccc", "ddd", "eee", "fff", "ggg", "hhh", "iii"};

static void MiscTest_TextInputExited(void)
{
	printf("MISC TEST Text input exited! buf:%s\n", textInput.buf);
}

bool MiscTest_Init(void *arg)
{
	// textInput = GuiTextInput_Create(MiscTest_TextInputExited);
	// GuiTextInput_SetInfoText(&textInput, "TEST TEST TEST TEST TEST");

	guiList = GuiList_Create(testStrs, 9, NULL, NULL);

	return true;
}
void MiscTest_Deinit(void)
{
}

void MiscTest_Update(void)
{

}

void MiscTest_Draw(void)
{
	OledMan_ClearBuf();
	// GuiTextInput_Draw(&textInput);
	GuiList_Draw(&guiList);
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

