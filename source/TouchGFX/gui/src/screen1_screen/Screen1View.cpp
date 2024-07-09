#include <gui/screen1_screen/Screen1View.hpp>
#include <gui/screen2_screen/Screen2View.hpp>
#include <cmsis_os.h>

Screen1View::Screen1View()
{

}

void Screen1View::setupScreen()
{
    Screen1ViewBase::setupScreen();
}

extern osMessageQueueId_t buttonPressQueueHandle;
void Screen1View::handleTickEvent() {
	Screen1ViewBase::handleTickEvent();

    // Move to the second screen
	uint8_t count = osMessageQueueGetCount(buttonPressQueueHandle);
	if(count > 0){
        uint8_t Rx_Data;
        osMessageQueueGet(buttonPressQueueHandle, &Rx_Data, NULL, 0);
        static_cast<FrontendApplication*>(Application::getInstance())->gotoScreen2ScreenNoTransition();
	}
}

void Screen1View::tearDownScreen()
{
    Screen1ViewBase::tearDownScreen();
}
