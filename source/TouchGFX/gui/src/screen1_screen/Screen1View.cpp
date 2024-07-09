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
	uint16_t count = osMessageQueueGetCount(buttonPressQueueHandle);
	if(count > 0){
        osMessageQueueReset(buttonPressQueueHandle);        
        static_cast<FrontendApplication*>(Application::getInstance())->gotoScreen2ScreenWipeTransitionEast();
	}
}

void Screen1View::tearDownScreen()
{
    Screen1ViewBase::tearDownScreen();
}
