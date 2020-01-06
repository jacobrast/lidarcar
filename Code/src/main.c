#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <stdio.h>
 
#include "setup.h"
#include "cli.h"
#include "stm32f7xx.h"                  // Device header
#include "Board_GLCD.h"                 // ::Board Support:Graphic LCD
#include "GLCD_Config.h"                // Keil.STM32756G-EVAL::Board Support:Graphic LCD
#include "RTE_Device.h"                 // Keil::Device:STM32Cube Framework:Classic
#include "stm32f7xx_hal.h"              // Keil::Device:STM32Cube HAL:Common
#include "Driver_USART.h"               // ::CMSIS Driver:USART
#include "stm32746g_discovery_sdram.h"  // Keil.STM32F746G-Discovery::Board Support:Drivers:SDRAM

int main(void)
{

	int rc;
	List cmd;

	setup_device();
	cli_init();
	
	while(1){
		rc = get_command(&cmd);
		rc = parse_command(&cmd);
	}	
}

