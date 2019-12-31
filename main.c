#include "setup.h"
#include "cli.h"
#include "stm32f7xx.h"                  // Device header
#include "Board_GLCD.h"                 // ::Board Support:Graphic LCD
#include "GLCD_Config.h"                // Keil.STM32756G-EVAL::Board Support:Graphic LCD
#include "RTE_Device.h"                 // Keil::Device:STM32Cube Framework:Classic
#include "stm32f7xx_hal.h"              // Keil::Device:STM32Cube HAL:Common
#include "Driver_USART.h"               // ::CMSIS Driver:USART
#include "stm32746g_discovery_sdram.h"  // Keil.STM32F746G-Discovery::Board Support:Drivers:SDRAM
#include <stdlib.h>
#include <string.h>
#include <math.h>
 

extern ARM_DRIVER_USART Driver_USART6;
extern ARM_DRIVER_USART Driver_USART7;
extern GLCD_FONT GLCD_Font_16x24;
extern int match;
extern int going_forward;
extern int set_wheels(int var);

char CR = 10; //carriage return, enter
char *new_line = &CR;
int acc = 0;
int UART7_sent = 0, UART6_sent = 0;
char scan[3] = {0xA5, 0x20, 0};
char stop[3] = {0xA5, 0x25, 0};
int dist;

extern void myUSART6_callback(uint32_t event);
extern void myUSART7_callback(uint32_t event);

void show_map(unsigned char data[]);
void print_num(int x,int y, int num)
{
    char* string;
    sprintf(string,"%d",num);
    GLCD_DrawString(x,y,string);
}

int main(void)
{
	command_t cmd;
	int rc;
	Driver_USART6.Initialize(myUSART6_callback);
    Driver_USART7.Initialize(myUSART7_callback);
   
	setup_device();
    //RCC->DCKCFGR1 |= (1<<24); //double timer speed to 216
	
	while(Driver_USART7.GetStatus().tx_busy==1);

    // Initalize BluTooth
    if(0){
        Driver_USART7.Send("AT+NAMELidar5",13); //(sizeof("AT+NAMELidar2")-1)/sizeof(char));
        Driver_USART7.Send("AT+BAUD8",8);//sizeof("AT+BAUD8")/sizeof(char));
        Driver_USART7.Send("AT+RESET",sizeof("AT+RESET"));
    }

    
    //Interupt configuration
	RCC->APB2ENR |=(1<<14); //enable the SYSCFG peripheral for connecting GPIO to interupt lines
    SYSCFG->EXTICR[2] |= (5<<8); //Enable interupts on PF10
	EXTI->IMR |=(1<<10); //Demasking the input input request on line 10
	EXTI->RTSR |=(1<<10); //Enable rising edge trigger for line 10
    EXTI->FTSR |=(1<<10); //Enable falling edge trigger for line 10
	//Activate NVIC
    //change for motor count to 
	NVIC_SetPriority(EXTI15_10_IRQn,2);
	NVIC_EnableIRQ(EXTI15_10_IRQn);
    
    //GPIOI->ODR |= (1<<0);
    
	GLCD_DrawString(0,0,"Ready...");
    dist = 0;
    GPIOF->ODR |= (1<<9); //enable motor on PF9

	while(1){
        //PIOI->BSRR = (1<<0);
		add_command(&cmd);
        rc = get_command(&cmd);
		rc = parse_command(&cmd);
	}	
}

void EXTI15_10_IRQHandler(void){
	dist++;
    print_num(200,175,dist);
    if(dist >= match){
        if(going_forward == 1)
        {
                GLCD_DrawString(150,150,"OFF");
                TIM12->CCR2 = 0;
                going_forward = 0;
                dist = 0;
        }
        GLCD_DrawString(0,100,"Count done:");
        print_num(200,100,dist);
        match = 0;
    }
    EXTI->PR |=(1<<10); 		//clear the flag
    
}

