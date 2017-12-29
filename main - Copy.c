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

char CR = 10; //carriage return, enter
char *new_line = &CR;
int acc = 0;
int done = 0, UART7_sent = 0, UART6_sent = 0;
char scan[3] = {0xA5, 0x20, 0};
char stop[3] = {0xA5, 0x25, 0};


void show_map(unsigned char data[]);
void print_num(int x,int y, int num)
{
    char* string;
    sprintf(string,"%d",num);
    GLCD_DrawString(x,y,string);
}

void myUSART6_callback(uint32_t event)
{
  if (event & ARM_USART_EVENT_RECEIVE_COMPLETE) {
        GLCD_DrawString(0,20,"data recieved");
		done=1;
	}
  
  if (event & ARM_USART_EVENT_SEND_COMPLETE) UART6_sent = 1;
}

void myUSART7_callback(uint32_t event)
{
    if (event & ARM_USART_EVENT_SEND_COMPLETE) UART7_sent = 1;
}

/*int uart6_send(void* message)
{   
    int n;
    Driver_USART6.Send(message,strlen(message));
    n = Driver_USART6.GetTxCount();
    while(n != strlen(message)){
        n = Driver_USART6.GetTxCount();
    }
}
void uart6_send_byte(void* message)
{
    Driver_USART6.Send(message,1);
    while(UART6_sent!=1);
    UART6_sent = 0;
}
void uart6_send_cmd(void* message)
{
    uart6_send_byte(&sync_cmd);
    uart6_send_byte(message);
}
void uart7_send(void* message)
{
    int n;
    Driver_USART7.Send(message,strlen(message));
    n = Driver_USART7.GetTxCount();
    while(n != strlen(message)){
        n = Driver_USART7.GetTxCount();
    }
}
*/
int main(void)
{
	command_t cmd;
	int rc;
    int i, data_int, n=0, size;
    char text[100]; 
    char data_out[100];
    unsigned char data[5*num_sample+7];
    unsigned short distance, angle;
    float distance_f, angle_f;
    char quality;
    int x_offset;
    int y_offset;
    
	
    Driver_USART6.Initialize(myUSART6_callback);
    Driver_USART7.Initialize(myUSART7_callback);
	setup_device();
    RCC->DCKCFGR1 |= (1<<24); //double timer speed to 216
	
	while(Driver_USART7.GetStatus().tx_busy==1);
//    Driver_USART7.Send("AT+NAMELidar2",13); //(sizeof("AT+NAMELidar2")-1)/sizeof(char));
//    Driver_USART7.Send("AT+BAUD8",8);//sizeof("AT+BAUD8")/sizeof(char));
   // Driver_USART7.Send("AT+RESET",sizeof("AT+RESET"));
    
	GLCD_DrawString(0,0,"Ready...");
    GPIOI->BSRR = (1<<0);
    //GPIOI->BSRR = (1<<16); 

	//while(1){
        done=0;
        Driver_USART6.Receive(data,(5*num_sample)+7);
        Driver_USART6.Send(scan,2);
        while(Driver_USART6.GetTxCount()!=2);
        //while(Driver_USART6.GetRxCount()<7); //block until char is received
        while(done!=1);
        //GPIOI->BSRR = (1<<16); 
        size=sprintf(text,"%x %x %x %x %x %x %x",data[0],data[1],data[2],data[3],data[4],data[5],data[6]);
        GLCD_DrawString(0,30,text);
        Driver_USART7.Send(text,size);
		while(Driver_USART7.GetTxCount()!=size);
        Driver_USART6.Send(stop,2);
        while(Driver_USART6.GetTxCount()!=2);
        GPIOI->BSRR = (1<<16); 
        
        
        for(n=1; n<(5*num_sample); n++){
            quality = data[n]>>2;
			angle=(((unsigned short)data[n+1])>>1)+(((unsigned short)data[n+2])<<7);
			distance=((unsigned short)data[n+3])+(((unsigned short)data[n+4])<<8);
            if (distance!=0)
			{
				angle_f = ((float)angle)/64.0;
				distance_f = ((float)distance)/4.0;

				size=sprintf(data_out,"%d %3.4f %6.2f\n",quality,angle_f,distance_f);
                
                //GLCD_DrawString(0,(n-3)*15,data_out);
				Driver_USART7.Send(data_out,size);
				while(Driver_USART7.GetTxCount()!=size);
                
			}
        }
        
        /*
		add_command(&cmd);
        rc = get_command(&cmd);
		rc = parse_command(&cmd);
        */
       
        
        
	//}	
}

