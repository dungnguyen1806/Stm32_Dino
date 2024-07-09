#include <gui/screen2_screen/Screen2View.hpp>
#include <cmsis_os.h>

Screen2View::Screen2View()
{

}

void Screen2View::setupScreen()
{
    Screen2ViewBase::setupScreen();

    // Setup objects
    ground.setXY(0, 180);
    ground.startAnimation(false, true, true);

    dino.setXY(25, 140);
    dino.startAnimation(false, true, true);

    dinoc.setXY(21, 161);
    dinoc.startAnimation(false, true, true);
    dinoc.setVisible(false);
}

// **From main.c**
// void start_hardware_polling_task(void *argument)
// {
//   /* USER CODE BEGIN start_hardware_polling_task */
//   /* Infinite loop */
// 	uint8_t data[] = {0, 0, 0, 0};
// 	uint16_t JoystickX, JoystickY;
// 	uint32_t Joystick;

// 	for(;;)
// 	{
// 		if (HAL_GPIO_ReadPin(B1_GPIO_Port, B1_Pin) == GPIO_PIN_SET)
// 		{
// 		  osMessageQueuePut(joyStickQueueHandle, &data, 0, 10);
// 		}

// 		HAL_ADC_Start(&hadc1);
// 		HAL_ADC_Start(&hadc2);
// 		HAL_ADC_PollForConversion(&hadc1, 1);
// 		HAL_ADC_PollForConversion(&hadc2, 1);
// 		JoystickX = HAL_ADC_GetValue(&hadc1);
// 		JoystickY = HAL_ADC_GetValue(&hadc2);
// 		Joystick = (JoystickY << 16) | JoystickX;
// 		osMessageQueuePut(joyStickQueueHandle, &Joystick, 0, 10);
// 		osDelay(100);
// 	}
//   /* USER CODE END start_hardware_polling_task */
// }

// /* USER CODE END Header_start_uart_task */
// void start_uart_task(void *argument)
// {
//   /* USER CODE BEGIN start_uart_task */
//   /* Infinite loop */
//   for(;;)
//   {
// 	  char mess[20];
// 	  if (osMessageQueueGetCount(joyStickQueueHandle) > 0)
// 	  {
// 		  uint32_t x;
// 		  osMessageQueueGet(joyStickQueueHandle, &x, 0, 10);
// 		  sprintf(mess, "%3d, %3d\n", x>>16, x&0xFFFF);
// 		  HAL_UART_Transmit(&huart1, mess, strlen(mess), 10);
// 	  }
//   }
//   /* USER CODE END start_uart_task */
// }


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
