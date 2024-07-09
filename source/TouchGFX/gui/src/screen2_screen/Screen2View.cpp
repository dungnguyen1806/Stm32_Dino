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

    isJumping = false;
	BASE_HEIGHT = 140;
	currentHeight = 1;
	timeJump = 0;

    currentEnemy = 0;
    enemyPositionX = SCREEN_WIDTH;
    enemySpeed = 2;

	// seed = 1;
	// jumpDis = -4;
	// MAX_HEIGHT = 70;
	// counter = 0;
	// obsNumber = 0;
	// BASE_BIRD_HEIGHT = 124;
	tick = 0;
	highestScore = 1222;
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
    dino.setUpdateTicksInterval(10);

    dinoc.setXY(21, 161);
    dinoc.startAnimation(false, true, true);
    dinoc.setUpdateTicksInterval(10);
    dinoc.setVisible(false);

    cacti1.setXY(SCREEN_WIDTH, 157);
    cacti1.setVisible(true);
    bird.setXY(SCREEN_WIDTH, 96);
    bird.startAnimation(false, true, true);
    bird.setUpdateTicksInterval(10);
}


extern osMessageQueueId_t joyStickQueueHandle;
void Screen2View::handleTickEvent() {
	Screen2ViewBase::handleTickEvent();

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
        // currentEnemy = rand() % 2;
        currentEnemy = 1 - currentEnemy;
        enemyPositionX = SCREEN_WIDTH;

        switch (currentEnemy) {
            case 0:
                cacti1.setX(enemyPositionX);
                cacti1.setVisible(true);
                bird.setVisible(false);
                break;
            case 1:
                bird.setX(enemyPositionX);
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
            timeJump = 0;
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
	timeJump++;
	int a = -15; //acceleration
	int v0  = 65; // base spd
	currentHeight = round(BASE_HEIGHT - a*timeJump*timeJump/100 - v0*timeJump/10);//y = a*t*t + v0*t + y0
	dino.setY(currentHeight);

	// reset state after landing
	if (dino.getY() >= BASE_HEIGHT){
		dino.setY(BASE_HEIGHT);
		dino.startAnimation(false, true, true);
		isJumping = false;
		currentHeight = 1;
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
		int deviation = BASE_HEIGHT - dino.getY(); // amount of deviation of the dino from the ground
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

static uint32_t GetSector(uint32_t Address)
{
  uint32_t sector = 0;

  if((Address < 0x08003FFF) && (Address >= 0x08000000))
  {
    sector = FLASH_SECTOR_0;
  }
  else if((Address < 0x08007FFF) && (Address >= 0x08004000))
  {
    sector = FLASH_SECTOR_1;
  }
  else if((Address < 0x0800BFFF) && (Address >= 0x08008000))
  {
    sector = FLASH_SECTOR_2;
  }
  else if((Address < 0x0800FFFF) && (Address >= 0x0800C000))
  {
    sector = FLASH_SECTOR_3;
  }
  else if((Address < 0x0801FFFF) && (Address >= 0x08010000))
  {
    sector = FLASH_SECTOR_4;
  }
  else if((Address < 0x0803FFFF) && (Address >= 0x08020000))
  {
    sector = FLASH_SECTOR_5;
  }
  else if((Address < 0x0805FFFF) && (Address >= 0x08040000))
  {
    sector = FLASH_SECTOR_6;
  }
  else if((Address < 0x0807FFFF) && (Address >= 0x08060000))
  {
    sector = FLASH_SECTOR_7;
  }
  else if((Address < 0x0809FFFF) && (Address >= 0x08080000))
  {
    sector = FLASH_SECTOR_8;
  }
  else if((Address < 0x080BFFFF) && (Address >= 0x080A0000))
  {
    sector = FLASH_SECTOR_9;
  }
  else if((Address < 0x080DFFFF) && (Address >= 0x080C0000))
  {
    sector = FLASH_SECTOR_10;
  }
  else if((Address < 0x080FFFFF) && (Address >= 0x080E0000))
  {
    sector = FLASH_SECTOR_11;
  }
  else if((Address < 0x08103FFF) && (Address >= 0x08100000))
  {
    sector = FLASH_SECTOR_12;
  }
  else if((Address < 0x08107FFF) && (Address >= 0x08104000))
  {
    sector = FLASH_SECTOR_13;
  }
  else if((Address < 0x0810BFFF) && (Address >= 0x08108000))
  {
    sector = FLASH_SECTOR_14;
  }
  else if((Address < 0x0810FFFF) && (Address >= 0x0810C000))
  {
    sector = FLASH_SECTOR_15;
  }
  else if((Address < 0x0811FFFF) && (Address >= 0x08110000))
  {
    sector = FLASH_SECTOR_16;
  }
  else if((Address < 0x0813FFFF) && (Address >= 0x08120000))
  {
    sector = FLASH_SECTOR_17;
  }
  else if((Address < 0x0815FFFF) && (Address >= 0x08140000))
  {
    sector = FLASH_SECTOR_18;
  }
  else if((Address < 0x0817FFFF) && (Address >= 0x08160000))
  {
    sector = FLASH_SECTOR_19;
  }
  else if((Address < 0x0819FFFF) && (Address >= 0x08180000))
  {
    sector = FLASH_SECTOR_20;
  }
  else if((Address < 0x081BFFFF) && (Address >= 0x081A0000))
  {
    sector = FLASH_SECTOR_21;
  }
  else if((Address < 0x081DFFFF) && (Address >= 0x081C0000))
  {
    sector = FLASH_SECTOR_22;
  }
  else if((Address < 0x081FFFFF) && (Address >= 0x081E0000))
  {
    sector = FLASH_SECTOR_23;
  }
  return sector;
}

void Flash_Read_Data (uint32_t StartSectorAddress, uint32_t *RxBuf, uint16_t numberofwords)
{
	while (1)
	{

		*RxBuf = *(__IO uint32_t *)StartSectorAddress;
		StartSectorAddress += 4;
		RxBuf++;
		if (!(numberofwords--)) break;
	}
}

int Screen2View::readHighestScore(){
	int result;
	uint32_t data[0];
	Flash_Read_Data(0x081E0000, data, 1);
	result = data[0];
	return result;
}
