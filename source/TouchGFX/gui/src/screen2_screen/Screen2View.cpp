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

    SCREEN_WIDTH = 320;
	BASE_DINO_Y = 140;
	BASE_DINOC_Y = 161;
	BASE_CACTUS_Y = 156;
	MIN_BIRD_Y = 106;
	MAX_BIRD_Y = 140;
	BASE_ENEMY_SPEED = 3;

    isJumping = false;
	currentDinoHeight = 1;
	timeDinoJump = 0;

    enemySpeed = BASE_ENEMY_SPEED;
    enemyPositionX = SCREEN_WIDTH;
    currentEnemy = 0;

	tick = 0;
	highestScore = 10;
}

void Screen2View::setupScreen()
{
    Screen2ViewBase::setupScreen();

    dinoc.setVisible(false);
    highestScore = presenter->getHighestScore();
    Unicode::snprintf(highestScoreTextAreaBuffer, HIGHESTSCORETEXTAREA_SIZE, "%d", highestScore);
    highestScoreTextArea.invalidate();
    Unicode::snprintf(textAreaPointBuffer, TEXTAREAPOINT_SIZE, "%d", counter);
    textAreaPoint.invalidate();

    // Setup objects
    ground.setXY(0, 180);
    ground.startAnimation(false, true, true);

    dino.setXY(25, BASE_DINO_Y);
    dino.startAnimation(false, true, true);
    dino.setUpdateTicksInterval(10);

    dinoc.setXY(21, BASE_DINOC_Y);
    dinoc.startAnimation(false, true, true);
    dinoc.setUpdateTicksInterval(10);
    dinoc.setVisible(false);

    cacti1.setXY(SCREEN_WIDTH, BASE_CACTUS_Y);
    cacti1.setVisible(true);

    bird.setXY(SCREEN_WIDTH, MAX_BIRD_Y);
    bird.startAnimation(false, true, true);
    bird.setUpdateTicksInterval(10);
}


uint32_t Screen2View::randint(void) {
    static uint32_t seed = 0; 

    if (seed == 0) {
        seed = SysTick->VAL;
    }
    seed = (seed * 1103515245 + 12345) & 0x7FFFFFFF; 
    seed ^= SysTick->VAL;

    return seed;
}

extern osMessageQueueId_t joyStickQueueHandle;
void Screen2View::handleTickEvent() {
	Screen2ViewBase::handleTickEvent();
	tick++;

	// Check for collision
	bool gameOver = checkCollision();
	if (gameOver) {
		if(counter > highestScore) highestScore = counter;
		presenter->saveScore(counter);
		presenter->saveHighestScore(highestScore);
		static_cast<FrontendApplication*>(Application::getInstance())->gotoScreen3ScreenNoTransition();;
	}

    // Handle joystick input
	uint32_t count = osMessageQueueGetCount(joyStickQueueHandle);
	if(count > 0){
		handleJoystickEvent(count);
	}

    // Handle enemy movement
    switch (currentEnemy) {
        case 0:
            cacti1.setX(enemyPositionX);
            break;
        case 1:
            bird.setX(enemyPositionX);
            break;
    }
    enemyPositionX -= enemySpeed;

    if (enemyPositionX < -50) {
        currentEnemy = randint() % 2;
		enemySpeed = BASE_ENEMY_SPEED + randint() % 3; // randomize the speed of the enemy (2 - 4) pixels per tick
        enemyPositionX = SCREEN_WIDTH + randint() % 30; // randomize the position of the enemy (320 - 350) pixels from the right edge

        switch (currentEnemy) {
            case 0:
                cacti1.setX(enemyPositionX);
                cacti1.setVisible(true);
                bird.setVisible(false);
                break;
            case 1:
				bird.setX(enemyPositionX);
				bird.setY(randint() % (MAX_BIRD_Y - MIN_BIRD_Y + 1) + MIN_BIRD_Y);
                bird.setVisible(true);
                cacti1.setVisible(false);
                break;
        }
    }

    checkScore();

    if (isJumping) {
        dinoJump();
    }
	invalidate();
}

void Screen2View::handleJoystickEvent(uint32_t count){
    uint32_t Joystick;
    uint16_t JoystickX, JoystickY;
    for(int i = 0; i < count; i++){
        osMessageQueueGet(joyStickQueueHandle, &Joystick, NULL, osWaitForever);
        JoystickX = Joystick >> 16;
        JoystickY = Joystick & 0xFFFF;

        if ((JoystickY < 64) && (!isJumping)) {
            dino.setVisible(false);
            dinoc.setVisible(true);
        } else if ((JoystickY > 192) && (!isJumping)) {
            timeDinoJump = 0;
            isJumping = true;
        } else {
            dino.setVisible(true);
            dinoc.setVisible(false);
        }
    }
}

void Screen2View::dinoJump(){
	dinoc.setVisible(false);
	dino.setVisible(true);
	dino.stopAnimation();

	timeDinoJump++;
	int a = -15; //acceleration
	int v0  = 65; // base spd
	currentDinoHeight = round(BASE_DINO_Y - a*timeDinoJump*timeDinoJump/100 - v0*timeDinoJump/10);//y = a*t*t + v0*t + y0
	dino.setY(currentDinoHeight);

	// reset state after landing
	if (dino.getY() >= BASE_DINO_Y){
		dino.setY(BASE_DINO_Y);
		dino.startAnimation(false, true, true);
		isJumping = false;
		currentDinoHeight = 1;
	}
}

void Screen2View::checkScore(){
	//printout the point
	if(tick % 5 == 0) counter++;
	if(counter < highestScore){
		Unicode::snprintf(textAreaPointBuffer, TEXTAREAPOINT_SIZE, "%d", counter);
		textAreaPoint.invalidate();
		Unicode::snprintf(highestScoreTextAreaBuffer, HIGHESTSCORETEXTAREA_SIZE, "%d", highestScore);
		highestScoreTextArea.invalidate();
	}else{
		textAreaPoint.setVisible(false);
		Unicode::snprintf(highestScoreTextAreaBuffer, HIGHESTSCORETEXTAREA_SIZE, "%d", counter);
		highestScoreTextArea.invalidate();
	}
}

bool Screen2View::checkCollision(){
	int16_t dinoT, dinoR, dinoB, dinoL;
	if (dino.isVisible()){
		int deviation = BASE_DINO_Y - dino.getY(); // amount of deviation of the dino from the ground
		if (deviation > 8) deviation = 8; // max deviation is 8, which reaches dino's legs
		dinoL = dino.getX() + 10;
		dinoT = dino.getY() + 10;
		// simulate the skew of the dino's lower body using the deviation value
		dinoR = dino.getX() + dino.getWidth() - 10 - deviation;
		dinoB = dino.getY() + dino.getHeight() - 10;
	} else {
		dinoL = dinoc.getX();
		dinoT = dinoc.getY() + 10;
		dinoR = dinoc.getX() + dinoc.getWidth();
		dinoB = dinoc.getY() + dinoc.getHeight();
	}

	int16_t obsT, obsR, obsB, obsL;
	if (currentEnemy == 0){
		obsL = cacti1.getX();
		obsT = cacti1.getY();
		obsR = cacti1.getX() + cacti1.getWidth();
		obsB = cacti1.getY() + cacti1.getHeight();
	} else {
		obsL = bird.getX();
		obsT = bird.getY();
		obsR = bird.getX() + bird.getWidth();
		obsB = bird.getY() + bird.getHeight();
	}

	if (dinoR >= obsL && dinoL <= obsR && dinoB >= obsT && dinoT <= obsB){
		return true;
	}

	return false;
}


void Screen2View::tearDownScreen()
{
    Screen2ViewBase::tearDownScreen();
}
