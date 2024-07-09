#include <gui/screen3_screen/Screen3View.hpp>
#include <gui/screen2_screen/Screen2View.hpp>
#include <cmsis_os.h>

Screen3View::Screen3View()
{
	score = 0;
}

void Screen3View::setupScreen()
{
    Screen3ViewBase::setupScreen();
    score = presenter->getScore();

    Unicode::snprintf(textArea1Buffer, TEXTAREA1_SIZE, "%d", score);

}

extern osMessageQueueId_t buttonPressQueueHandle;
void Screen3View::handleTickEvent() {
	Screen3ViewBase::handleTickEvent();

    // Move to the second screen
	uint8_t count = osMessageQueueGetCount(buttonPressQueueHandle);
	if(count > 0){
        uint8_t Rx_Data;
        osMessageQueueGet(buttonPressQueueHandle, &Rx_Data, NULL, 0);
        static_cast<FrontendApplication*>(Application::getInstance())->gotoScreen2ScreenNoTransition();
	}
}

void Screen3View::tearDownScreen()
{
    Screen3ViewBase::tearDownScreen();
}
