#include "Driver_USART.h"               // ::CMSIS Driver:USART
#include "Board_GLCD.h"                 // ::Board Support:Graphic LCD
#include "GLCD_Config.h"                // Keil.STM32756G-EVAL::Board Support:Graphic LCD
#include "stm32f7xx_hal.h"              // Keil::Device:STM32Cube HAL:Common
#include "setup.h"

extern ARM_DRIVER_USART Driver_USART7;
extern ARM_DRIVER_USART Driver_USART6;
extern GLCD_FONT GLCD_Font_16x24;
static void SystemClock_Config(void);

void setup_device(void)
{
	SystemClock_Config();
	GLCD_Initialize();
	GLCD_SetFont(&GLCD_Font_16x24);

	Driver_USART7.PowerControl(ARM_POWER_FULL);
	Driver_USART7.Control(ARM_USART_MODE_ASYNCHRONOUS | 
												ARM_USART_DATA_BITS_8 |
												ARM_USART_PARITY_NONE |
												ARM_USART_STOP_BITS_1 |
												ARM_USART_FLOW_CONTROL_NONE, 115200);
	Driver_USART7.Control(ARM_USART_CONTROL_TX, 1);
	Driver_USART7.Control(ARM_USART_CONTROL_RX, 1);

	Driver_USART6.PowerControl(ARM_POWER_FULL);
	Driver_USART6.Control(ARM_USART_MODE_ASYNCHRONOUS | 
												ARM_USART_DATA_BITS_8 |
												ARM_USART_PARITY_NONE |
												ARM_USART_STOP_BITS_1 |
												ARM_USART_FLOW_CONTROL_NONE, 115200);
	Driver_USART6.Control(ARM_USART_CONTROL_TX, 1);
	Driver_USART6.Control(ARM_USART_CONTROL_RX, 1);
	
	GLCD_SetForegroundColor(GLCD_COLOR_BLACK);
	GLCD_ClearScreen();
    
  //GPIO Setup:
  //activates GPIOB GPIOI GPIOF
	RCC->AHB1ENR |= (1<<GPIOB_OFFSET)|(1<<GPIOF_OFFSET)|(1<<GPIOI_OFFSET); 
    
  GPIOB->MODER |= (unsigned int)(2U<<30); //PB15 in AF mode
  GPIOI->MODER |= (1<<0); //PI0 is an output
  GPIOF->MODER |= (1<<18); //PF9 is an output
  GPIOB->AFR[1] |= (unsigned int)(9U<<28); //PB15 in AF9 mode (TIM12_CH2)
    
  GPIOF->MODER |= (2<<16); //PF8 in AF mode
  GPIOF->AFR[1] |= (9<<0); //PF8 in AF9 mode (TIM13_CH1)
    
  //ADC Setup: For reading the voltage on the pot to control the servo motors
	RCC->APB2ENR |= (1<<10); //activate ADC3
	ADC3->SQR3 |= (8<<0); //first conversion on channel 8 (ADC3_IN8)
	ADC3->CR2 |= (1<<0); //enable ADC3
    
  //Timer Setup:
	//We will use TIM13_CH1 PF8 for steering
  RCC->APB1ENR |= (1<<7); //activates TIM13
  TIM13->CCMR1 |= (6<<4); //TIM13_CH1 in PWM1 mode
  TIM13->CCER |= (1<<0); //connects TIM13_CH1 to PB30 pin  
  TIM13->PSC = 36; //prescaler
  TIM13->ARR = 0; //defines PWM period
  TIM13->CCR1 = 0; //defines duty cycle
  TIM13->CR1 |= (1<<0); //starts counter
    	
  //For TIM12_CH2 PB15, wheel power
  RCC->APB1ENR |= (1<<6); //activates TIM12
  TIM12->CCMR1 |= (6<<12); //TIM12_CH2 in PWM1 mode
  TIM12->CCER |= (1<<4); //connects TIM12_CH2 to PB30 pin
  TIM12->PSC = 0; //prescaler
  TIM12->ARR = 8639; //defines PWM period
  TIM12->CCR2 = 0; //defines duty cycle
  TIM12->CR1 |= (1<<0); //starts counter

  //Timer interupt setup for drive funciton timing
  RCC->APB1ENR |= (1<<3); //Enable timer 5
  TIM5->DIER |= (1<<1); //enable interupts on capture channel 1
  TIM5->CCMR1 |= (6<<4); //Put timer 5 in toggle mode  
  TIM5->PSC = 60; 
	TIM5->ARR = 36000; 
	TIM5->CCR1 = 36000; 
    
  //TIM5->CR1 |= (1<<0); //start timer 5 
  NVIC_SetPriority(TIM5_IRQn,2); //set priority to 2(chosen arbitrarily) 
	NVIC_EnableIRQ(TIM5_IRQn);
}

void SysTick_Handler (void)
{
    HAL_IncTick();
}


// Boilerplate code for overclocking CPU
static void SystemClock_Config(void)
{
  RCC_ClkInitTypeDef RCC_ClkInitStruct;
  RCC_OscInitTypeDef RCC_OscInitStruct;
  HAL_StatusTypeDef ret = HAL_OK;

  /* Enable HSE Oscillator and activate PLL with HSE as source */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
  RCC_OscInitStruct.HSEState = RCC_HSE_ON;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
  RCC_OscInitStruct.PLL.PLLM = 25;
  RCC_OscInitStruct.PLL.PLLN = 432;
  RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV2;
  RCC_OscInitStruct.PLL.PLLQ = 9;

  ret = HAL_RCC_OscConfig(&RCC_OscInitStruct);
  if(ret != HAL_OK)
  {
    while(1) { ; }
  }

  /* Activate the OverDrive to reach the 216 MHz Frequency */
  ret = HAL_PWREx_EnableOverDrive();
  if(ret != HAL_OK)
  {
    while(1) { ; }
  }
  
  /* Select PLL as system clock source and configure the HCLK, PCLK1 and PCLK2 clocks dividers */
  RCC_ClkInitStruct.ClockType = (RCC_CLOCKTYPE_SYSCLK | RCC_CLOCKTYPE_HCLK | RCC_CLOCKTYPE_PCLK1 | RCC_CLOCKTYPE_PCLK2);
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV4;  
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV2;

  ret = HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_7);
  if(ret != HAL_OK)
  {
    while(1) { ; }
  }
}
