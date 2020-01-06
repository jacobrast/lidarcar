#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include "list.h"
#include "cli.h"
#include "command.h"

extern int set_wheels(int var);

int count(int n);
int go_forward(int mm);
int set_wheels(int pos);
void block(int ms);
void lidar_scan(void);

char CR = 10; //carriage return, enter
char *new_line = &CR;
int acc = 0;
int UART7_sent = 0, UART6_sent = 0;
char scan[3] = {0xA5, 0x20, 0};
char stop[3] = {0xA5, 0x25, 0};
int dist;

extern char *new_line;
int match = 0;
int going_forward = 0;
int drive_ccr2;
static int scan_done = 0;
extern int dist;

entry command_list[8] = {
	{ .key = "clear", 	.value = &clear_screen },
	{ .key = "turn", 	.value = &turn_cmd },
	{ .key = "scan", 	.value = &scan_cmd },
	{ .key = "set", 	.value = &set_cmd 	},
	{ .key = "drive", 	.value = &drive_cmd	},
	{ .key = "print", 	.value = &print_cmd	},
	{ NULL, NULL }
};

int clear_screen(List *argv){
	GLCD_ClearScreen();
	return(0);
}  


// TODO: Update the argv and argc into something familiar from Unix / Linux
int drive_cmd(List *argv)
{
	char **arg;
	parse_subcommand(argv, &arg);

	if(atoi(arg[1]) > 0 && atoi(arg[1]) <= 10000){
		GLCD_DrawString(300,20,arg[1]);
		match = 0;
		going_forward = 1;
		go_forward(timing_var*atoi(arg[1]));
	}
	else{
		Driver_USART7.Send("Invalid input. Please put a value between 0 and 100",50);
		return -1;
	}

	free(arg);
}

int turn_cmd(List *argv)
{
	char **arg;
	parse_subcommand(argv, &arg);
	// Add error catching...
	int degree = atoi(arg[1]);

	GLCD_DrawString(300,0,"turn");
	switch(degree){
		case 58:
			set_wheels(58);
			block(100);
			go_forward(duration_58);
			while(match!=0);
			block(200);
			set_wheels(25); //25 is forward 
			break;
		case 45:
			set_wheels(45);
			block(100);
			go_forward(870);
			while(match!=0);
			block(200);
			set_wheels(25); //25 is forward 
			break;
		case 40:
			set_wheels(40);
			block(100);
			go_forward(duration_40);
			while(match!=0);
			block(200);
			set_wheels(25); //25 is forward 
			break;
		case 35:
			set_wheels(35);
			block(100);
			go_forward(duration_35);
			while(match!=0);
			block(200);
			set_wheels(25); //25 is forward  
			break;
		case 12:
			set_wheels(12);
			block(100);
			go_forward(1350);
			while(match!=0);
			block(200);
			set_wheels(25); //25 is forward 
			break;
		case 8:
			set_wheels(8);
			block(100);
			go_forward(duration_8);
			while(match!=0);
			block(200);
			set_wheels(25); //25 is forward 
			break;
		default:
			Driver_USART7.Send("Invalid input. Please put a value between 0 and 100",51);
			return(-1);
	}

	free(arg);
}


int scan_cmd(List *argv)
{
    char scan[3] = {0xA5, 0x20, 0};
    char stop[3] = {0xA5, 0x25, 0};
    char text[100]; 
    unsigned char data[5*num_sample+7];
    unsigned short distance, angle;
    float distance_f, angle_f;
    char quality;
    int x_offset;
    int y_offset;
    int n, size;
    char data_out[100];
    GPIOI->BSRR = (1<<0);
	scan_done=0;
	Driver_USART6.Receive(data,(5*num_sample)+7);
	Driver_USART6.Send(scan,2);
	while(Driver_USART6.GetTxCount()!=2); //block unitl char is received
	while(scan_done!=1);
	size=sprintf(text,"%x %x %x %x %x %x %x",data[0],data[1],data[2],data[3],data[4],data[5],data[6]);
	GLCD_DrawString(0,30,text);
	Driver_USART7.Send(text,size);
	while(Driver_USART7.GetTxCount()!=size);
	Driver_USART6.Send(stop,2);
	while(Driver_USART6.GetTxCount()!=2);
	GPIOI->BSRR = (1<<16); 
	
	
	for(n=7; n<(5*num_sample); n++){
		quality = data[n]>>2;
		angle=(((unsigned short)data[n+1])>>1)+(((unsigned short)data[n+2])<<7);
		distance=((unsigned short)data[n+3])+(((unsigned short)data[n+4])<<8);
		Driver_USART7.Send(data_out,data[n]);
		while(Driver_USART7.GetTxCount()!=size);
	}
}


int set_cmd(List *argv)
{
	char **arg;
	parse_subcommand(argv, &arg);
	char *ARR_s, *CCR_s;
	int ARR, CCR;

	GLCD_DrawString(300,0,"set");

	if(arg[1] == "turn")
	{
		GLCD_DrawString(300,20,"turn");

		if(atoi(arg[2]) > 0 && atoi(arg[2]) <= 100)
		{
			GLCD_DrawString(300, 40, arg[2]);
			ARR = ((int)(((((double)(atoi(arg[2]))) * 2967) / 100) + 62134));
			CCR = ARR - 59177;	
			TIM13->ARR = ARR;
			TIM13->CCR1 = CCR;
			sprintf(ARR_s, "%d", ARR);
			sprintf(CCR_s, "%d", CCR);
			GLCD_DrawString(0, 24, ARR_s); 
			GLCD_DrawString(0, 48, CCR_s);
		}
		else if(!strcmp("pot", arg[2]))
		{
			GLCD_DrawString(300,0,"pot");
			//Start ACD Conversion
			ADC3->CR2 |= (1<<30);
			//Block and wait for conversion to finish
			while((ADC3->SR & (1<<1)) == 0);	
			ARR = ((int)(((((double)(ADC3->DR)) * 2967) / 4096) + 62134));
			CCR = ARR - 59177;	
			TIM13->ARR = ARR;
			TIM13->CCR1 = CCR;
			sprintf(ARR_s, "%d", ARR); 
			sprintf(CCR_s, "%d", CCR);
			GLCD_DrawString(0, 24, ARR_s); 
			GLCD_DrawString(0, 48, CCR_s);
		}
		else
		{
			GLCD_DrawString(300, 40, arg[2]);
			Driver_USART7.Send("Invalid input. Please put a value between 0 and 100",51);
			return(-1);
		}
	} 
	else if(arg[1] == "drive")
	{
		GLCD_DrawString(300, 20, "drive");
		if(atoi(arg[2]) >= 0 && atoi(arg[2]) <= 100){
			GLCD_DrawString(300, 40, arg[2]);
			CCR = (int)((((double)(atoi(arg[2])) / 100) * 8640));
			TIM12->CCR2 = CCR; //defines duty cycle
			sprintf(CCR_s, "%d", CCR); 
			GLCD_DrawString(0, 24, CCR_s); 
		}
		else{
			GLCD_DrawString(300, 40, arg[2]);
			Driver_USART7.Send("Invalid input. Please put a value between 0 and 100",51);

			return(-1);
		}
	}
	else
	{
		Driver_USART7.Send("Invalid subcommand.", 19);
		return -1;
	}

	free(arg);
}


int print_cmd(List *argv)
{
	int i, y;
	char **arg;
	parse_subcommand(argv, &arg);

	GLCD_ClearScreen();
	GLCD_DrawString(300, 0, "print");

	for(i=0; i<=list_size(argv); i++){
		GLCD_DrawString(0, y, arg[i]);
	}

	free(arg);

	return 0;
}


int count(int n)
{
    dist = 0;
    if(n > 0 && n <= 500000){
            match = n;
            //TIM5->CNT &= ~(0xffff); //reset count
            //TIM5->CR1 |= (1<<0); //start timer 5
        }    
    else{
        Driver_USART7.Send("Invalid input. Please put a value between 0 and 100",51);
        return(-1);
    }
};


int go_forward(int mm)
{
    match = 0;
    going_forward = 1;
    count((int)((87 * mm) / 10));
    drive_ccr2 = (int)((((double)(80) / 100) * 8640));
    TIM12->CCR2 = drive_ccr2; //defines duty cycle
};


int set_wheels(int pos)
{
    int pos_arr;
    int pos_ccr1;
    if(pos>= 8 && pos <= 58) //max turn values for car
    {
        pos_arr = ((int)((((double)(pos)) * 2967) / 100) + 62134);
        pos_ccr1 = pos_arr - 59177;	
        TIM13->ARR = pos_arr;
        TIM13->CCR1 = pos_ccr1;
    }
}


void block(int ms)
{
    int n;
    for(n = 1; n<(ms*216000); n++);
}


void myUSART6_callback(uint32_t event)
{
  if (event & ARM_USART_EVENT_RECEIVE_COMPLETE) {
        GLCD_DrawString(0, 20, "data recieved");
		scan_done=1;
	}
  
  if (event & ARM_USART_EVENT_SEND_COMPLETE) UART6_sent = 1;
}

void myUSART7_callback(uint32_t event)
{
    if (event & ARM_USART_EVENT_SEND_COMPLETE) UART7_sent = 1;
}
