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
//		writeHighestScore(highestScore);
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

//static uint32_t GetSector(uint32_t Address)
//{
//  uint32_t sector = 0;
//
//  if((Address < 0x08003FFF) && (Address >= 0x08000000))
//  {
//    sector = FLASH_SECTOR_0;
//  }
//  else if((Address < 0x08007FFF) && (Address >= 0x08004000))
//  {
//    sector = FLASH_SECTOR_1;
//  }
//  else if((Address < 0x0800BFFF) && (Address >= 0x08008000))
//  {
//    sector = FLASH_SECTOR_2;
//  }
//  else if((Address < 0x0800FFFF) && (Address >= 0x0800C000))
//  {
//    sector = FLASH_SECTOR_3;
//  }
//  else if((Address < 0x0801FFFF) && (Address >= 0x08010000))
//  {
//    sector = FLASH_SECTOR_4;
//  }
//  else if((Address < 0x0803FFFF) && (Address >= 0x08020000))
//  {
//    sector = FLASH_SECTOR_5;
//  }
//  else if((Address < 0x0805FFFF) && (Address >= 0x08040000))
//  {
//    sector = FLASH_SECTOR_6;
//  }
//  else if((Address < 0x0807FFFF) && (Address >= 0x08060000))
//  {
//    sector = FLASH_SECTOR_7;
//  }
//  else if((Address < 0x0809FFFF) && (Address >= 0x08080000))
//  {
//    sector = FLASH_SECTOR_8;
//  }
//  else if((Address < 0x080BFFFF) && (Address >= 0x080A0000))
//  {
//    sector = FLASH_SECTOR_9;
//  }
//  else if((Address < 0x080DFFFF) && (Address >= 0x080C0000))
//  {
//    sector = FLASH_SECTOR_10;
//  }
//  else if((Address < 0x080FFFFF) && (Address >= 0x080E0000))
//  {
//    sector = FLASH_SECTOR_11;
//  }
//  else if((Address < 0x08103FFF) && (Address >= 0x08100000))
//  {
//    sector = FLASH_SECTOR_12;
//  }
//  else if((Address < 0x08107FFF) && (Address >= 0x08104000))
//  {
//    sector = FLASH_SECTOR_13;
//  }
//  else if((Address < 0x0810BFFF) && (Address >= 0x08108000))
//  {
//    sector = FLASH_SECTOR_14;
//  }
//  else if((Address < 0x0810FFFF) && (Address >= 0x0810C000))
//  {
//    sector = FLASH_SECTOR_15;
//  }
//  else if((Address < 0x0811FFFF) && (Address >= 0x08110000))
//  {
//    sector = FLASH_SECTOR_16;
//  }
//  else if((Address < 0x0813FFFF) && (Address >= 0x08120000))
//  {
//    sector = FLASH_SECTOR_17;
//  }
//  else if((Address < 0x0815FFFF) && (Address >= 0x08140000))
//  {
//    sector = FLASH_SECTOR_18;
//  }
//  else if((Address < 0x0817FFFF) && (Address >= 0x08160000))
//  {
//    sector = FLASH_SECTOR_19;
//  }
//  else if((Address < 0x0819FFFF) && (Address >= 0x08180000))
//  {
//    sector = FLASH_SECTOR_20;
//  }
//  else if((Address < 0x081BFFFF) && (Address >= 0x081A0000))
//  {
//    sector = FLASH_SECTOR_21;
//  }
//  else if((Address < 0x081DFFFF) && (Address >= 0x081C0000))
//  {
//    sector = FLASH_SECTOR_22;
//  }
//  else if((Address < 0x081FFFFF) && (Address >= 0x081E0000))
//  {
//    sector = FLASH_SECTOR_23;
//  }
//  return sector;
//}
//
//uint32_t Flash_Write_Data (uint32_t StartSectorAddress, uint32_t *Data, uint16_t numberofwords)
//{
//
//	static FLASH_EraseInitTypeDef EraseInitStruct;
//	uint32_t SECTORError;
//	int sofar=0;
//
//
//	 /* Unlock the Flash to enable the flash control register access *************/
//	  HAL_FLASH_Unlock();
//
//	  /* Erase the user Flash area */
//
//	  /* Get the number of sector to erase from 1st sector */
//
//	  uint32_t StartSector = GetSector(StartSectorAddress);
//	  uint32_t EndSectorAddress = StartSectorAddress + numberofwords*4;
//	  uint32_t EndSector = GetSector(EndSectorAddress);
//
//	  /* Fill EraseInit structure*/
//	  EraseInitStruct.TypeErase     = FLASH_TYPEERASE_SECTORS;
//	  EraseInitStruct.VoltageRange  = FLASH_VOLTAGE_RANGE_3;
//	  EraseInitStruct.Sector        = StartSector;
//	  EraseInitStruct.NbSectors     = (EndSector - StartSector) + 1;
//
//	  /* Note: If an erase operation in Flash memory also concerns data in the data or instruction cache,
//	     you have to make sure that these data are rewritten before they are accessed during code
//	     execution. If this cannot be done safely, it is recommended to flush the caches by setting the
//	     DCRST and ICRST bits in the FLASH_CR register. */
//	  if (HAL_FLASHEx_Erase(&EraseInitStruct, &SECTORError) != HAL_OK)
//	  {
//		  return HAL_FLASH_GetError ();
//	  }
//
//	  /* Program the user Flash area word by word
//	    (area defined by FLASH_USER_START_ADDR and FLASH_USER_END_ADDR) ***********/
//
//	   while (sofar<numberofwords)
//	   {
//	     if (HAL_FLASH_Program(FLASH_TYPEPROGRAM_WORD, StartSectorAddress, Data[sofar]) == HAL_OK)
//	     {
//	    	 StartSectorAddress += 4;  // use StartPageAddress += 2 for half word and 8 for double word
//	    	 sofar++;
//	     }
//	     else
//	     {
//	       /* Error occurred while writing data in Flash memory*/
//	    	 return HAL_FLASH_GetError ();
//	     }
//	   }
//
//	  /* Lock the Flash to disable the flash control register access (recommended
//	     to protect the FLASH memory against possible unwanted operation) *********/
//	  HAL_FLASH_Lock();
//
//	   return 0;
//}
//
//void Flash_Read_Data (uint32_t StartSectorAddress, uint32_t *RxBuf, uint16_t numberofwords)
//{
//	while (1)
//	{
//
//		*RxBuf = *(__IO uint32_t *)StartSectorAddress;
//		StartSectorAddress += 4;
//		RxBuf++;
//		if (!(numberofwords--)) break;
//	}
//}
//
//void Screen2View::writeHighestScore(int score){
//	uint32_t data[3];
//	data[0] = (uint32_t) score;
//	Flash_Write_Data(0x081E0000, data, 3);
//}
//
//int Screen2View::readHighestScore(){
//	int result;
//	uint32_t data[0];
//	Flash_Read_Data(0x081E0000, data, 1);
//	result = data[0];
//	return result;
//}
