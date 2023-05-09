#include "misc_test.h"
#include "oled_manager.h"

GuiTextInput_t textInput;

static void MiscTest_TextInputExited(void)
{
	printf("MISC TEST Text input exited! buf:%s\n", textInput.buf);
}

bool MiscTest_Init(void *arg)
{
	textInput = GuiTextInput_Create(MiscTest_TextInputExited);
	GuiTextInput_SetInfoText(&textInput, "TEST TEST TEST TEST TEST");
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
	GuiTextInput_Draw(&textInput);
}
void MiscTest_Start(void)
{
}
void MiscTest_Stop(void)
{
}
void MiscTest_ButtonInput(Button_e b, ButtonGesture_e g)
{
	GuiItemActions_e guiItemAction = GuiTextInput_DefaultButtonMap(b, g);
	GuiTextInput_TakeActionInput(&textInput, guiItemAction);
}

