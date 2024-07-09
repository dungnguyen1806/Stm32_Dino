#include <gui/screen2_screen/Screen2View.hpp>
#include <cmsis_os.h>
#include <gui/screen3_screen/Screen3View.hpp>
#include <cmsis_os.h>
#include "stm32f4xx_hal.h"
#include "string.h"
#include "stdio.h"
#include <math.h>
Screen2View::Screen2View()
{
	tick = 0;
	horizontal = 320;
	currentObs = 0;
	seed = 1;
	jumpDis = -4;
	isJumping = false;
	BASE_HEIGHT = 134;
	MAX_HEIGHT = 70;
	currentHeight = 1;
	counter = 0;
	enemySpeed = 3;
	obsNumber = 0;
	BASE_BIRD_HEIGHT = 124;
	timeJump = 0;
	highestScore = 0;
}

void Screen2View::setupScreen()
{
    Screen2ViewBase::setupScreen();

    dinoc.setVisible(false);
//    highestScore = readHighestScore();
    Unicode::snprintf(highestScoreTextAreaBuffer, HIGHESTSCORETEXTAREA_SIZE, "%d", highestScore);
    highestScoreTextArea.invalidate();
    Unicode::snprintf(textAreaPointBuffer, TEXTAREAPOINT_SIZE, "%d", counter);
    textAreaPoint.invalidate();

    // Setup objects
    ground.setXY(0, 180);
    ground.startAnimation(false, true, true);

    dino.setXY(25, 140);
    dino.startAnimation(false, true, true);

    dinoc.setXY(21, 161);
    dinoc.startAnimation(false, true, true);
    dinoc.setVisible(false);
}


extern osMessageQueueId_t joyStickQueueHandle;
void Screen2View::handleTickEvent() {
	Screen2ViewBase::handleTickEvent();

    uint32_t Joystick;
    uint16_t JoystickX, JoystickY;

    osMessageQueueGet(joyStickQueueHandle, &Joystick, NULL, osWaitForever);
    JoystickX = Joystick >> 16;
    JoystickY = Joystick & 0xFFFF;

    if (JoystickY < 64) {
    	dino.setVisible(false);
    	dinoc.setVisible(true);
    } else {
    	dino.setVisible(true);
    	dinoc.setVisible(false);
    }
}

void Screen2View::tearDownScreen()
{
    Screen2ViewBase::tearDownScreen();
}
