#include "setup.h"
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include "Driver_USART.h"               // ::CMSIS Driver:USART
#include "Board_GLCD.h"                 // ::Board Support:Graphic LCD
#include "cli.h"
#include "stm32f7xx_hal.h"              // Keil::Device:STM32Cube HAL:Common

extern ARM_DRIVER_USART Driver_USART7;
extern ARM_DRIVER_USART Driver_USART6;

extern char *new_line;
int match = 0;
int going_forward = 0;
int drive_ccr2;
int scan_done = 0;
extern int dist;
extern int UART6_sent, UART7_sent;  

int count(int n);
int go_forward(int time);
int set_wheels(int pos);
void block(int ms);
void lidar_scan(void);

void add_command(command_t *command){
		strcpy(command->head,"");
		strcpy(command->sub1,"");
		strcpy(command->sub2,"");
		strcpy(command->sub3,"");
		strcpy(command->sub4,"");
		command->length = 0;
}

//Implement delete command here

int get_command(command_t *command){
	int acc = 0, i;
	int word = 0;
	char raw[5];
	char buffer[5][100];
    char word_char[30];

	memset(raw,0,strlen(raw));
	for(i=0; i<5; i++){
		memset(buffer[i],0,100);
	}

	while(1){
		Driver_USART7.Receive(raw,1);
		while(Driver_USART7.GetRxCount()!=1); //block until char is received 
		if((int)raw[0] != 13){								//13 is ascii for enter
				if((int)raw[0] == 127){							//127 is ascii for delete
						if(acc > 0){										
								buffer[word][acc-1] = '\0';			
								acc--;
						}
						/*
						else if(acc == 0){						//need to deal with case of delete for new word
								word--;
							*/
				}
				else if((int)raw[0] == 32){					//23 is ascii for space
						if (word < 5){
								switch (word){
										case 0:
                                                buffer[word][acc+1] = '\0';
												strcpy(command->head,buffer[0]);
                                                GLCD_DrawString(200,0,command->head);
												break;
										case 1:
                                                buffer[word][acc+1] = '\0';
												strcpy(command->sub1,buffer[1]);
                                                GLCD_DrawString(200,20,command->sub1);
												break;
										case 2:
                                                buffer[word][acc+1] = '\0';
												strcpy(command->sub2,buffer[2]);
                                                GLCD_DrawString(200,40,command->sub2);
												break;
										case 3:
                                                buffer[word][acc+1] = '\0';
												strcpy(command->sub3,buffer[3]);
                                                GLCD_DrawString(200,60,command->sub3);
												break;
										case 4:
                                                buffer[word][acc+1] = '\0';
												strcpy(command->sub4,buffer[4]);
                                                GLCD_DrawString(200,80,command->sub4);
												break;
								}		
								word++;
								acc = 0;
						}
						else return (-1);
				}

				else{
				
						strncat(buffer[word],raw,sizeof(raw)/sizeof(char));
						acc++;
				}
				
		}
		else{
				command->length = word;
                sprintf(word_char,"%d",word);
                GLCD_DrawString(100,200,word_char);
				switch (word){
										case 0:
                                                buffer[word][acc+1] = '\0';
												strcpy(command->head, buffer[0]);
                                                GLCD_DrawString(200,0,command->head);
												break;
										case 1:
                                                buffer[word][acc+1] = '\0';
												strcpy(command->sub1,buffer[1]);
                                                GLCD_DrawString(200,20,command->sub1);
												break;
										case 2:
                                                buffer[word][acc+1] = '\0';
												strcpy(command->sub2,buffer[2]);
                                                GLCD_DrawString(200,40,command->sub2);
												break;
										case 3:
                                                buffer[word][acc+1] = '\0';
												strcpy(command->sub3,buffer[3]);
                                                GLCD_DrawString(200,60,command->sub3);
												break;
										case 4:
                                                buffer[word][acc+1] = '\0';
												strcpy(command->sub4,buffer[4]);
                                                GLCD_DrawString(200,80,command->sub4);
												break;
				}			
			  return (0);
		}
	}
}


int parse_command(command_t *command){
    //clear screen
    int pos_arr, pos_ccr1;
    int i, size, n;
    char pos_arr_str[30],pos_ccr1_str[30], drive_ccr1_str[30];
    int done;
    char* head = command->head;
    char* sub1 = command->sub1;
    char* sub2 = command->sub2;
    char *sub3 = command->sub3;
    char *sub4 = command->sub4;
    char scan[3] = {0xA5, 0x20, 0};
    char stop[3] = {0xA5, 0x25, 0};
    char text[100]; 
    unsigned char data[5*num_sample+7];
    unsigned short distance, angle;
    float distance_f, angle_f;
    char quality;
    int x_offset, count;
    int y_offset;
    char data_out[100];
    
    Driver_USART7.Send(new_line,1);
    Driver_USART7.Send("Car: ",5);
    //GLCD_ClearScreen();
    
	if(!strcmp("clear",head)){
            GLCD_ClearScreen();
            GLCD_DrawString(300,0,"clear");
            return(0);
        }  
    else if(!strcmp("forward",head)){
        if(atoi(command->sub1) > 0 && atoi(sub1) <= 10000){
                GLCD_DrawString(300,20,sub1);
                match = 0;
                going_forward = 1;
                go_forward(timing_var*atoi(sub1));
                
        }
        else{
            Driver_USART7.Send("Invalid input. Please put a value between 0 and 100",50);
            return(-1);
        }
    }
    else if(!strcmp("turn",head)){
        GLCD_DrawString(300,0,"turn");
        if(!strcmp("58",sub1)){
            set_wheels(58);
            block(100);
            go_forward(duration_58);
            while(match!=0);
            block(200);
            set_wheels(25); //25 is forward 
        }
        
        else if(!strcmp("45",sub1)){
            set_wheels(45);
            block(100);
            go_forward(870);
            while(match!=0);
            block(200);
            set_wheels(25); //25 is forward 
        }
        
        else if(!strcmp("40",sub1)){ 
            set_wheels(40);
            block(100);
            go_forward(duration_40);
            while(match!=0);
            block(200);
            set_wheels(25); //25 is forward 
        }
        
        else if(!strcmp("35",sub1)){
            set_wheels(35);
            block(100);
            go_forward(duration_35);
            while(match!=0);
            block(200);
            set_wheels(25); //25 is forward  
        }
        else if(!strcmp("12",sub1)){
            set_wheels(12);
            block(100);
            go_forward(1350);
            while(match!=0);
            block(200);
            set_wheels(25); //25 is forward 
        }
        else if(!strcmp("8",sub1)){
            set_wheels(8);
            block(100);
            go_forward(duration_8);
            while(match!=0);
            block(200);
            set_wheels(25); //25 is forward 
        }
        
        else{
            Driver_USART7.Send("Invalid input. Please put a value between 0 and 100",51);
            return(-1);
        }
    }
    else if(!strcmp("SCAN",command->head)){
        
        //lidar_scan();
        
        GPIOI->ODR |= (1<<0);
        scan_done=0;
        Driver_USART6.Receive(data,(5*num_sample)+7);
        Driver_USART6.Send(scan,2);
        while(Driver_USART6.GetTxCount()!=2);
        //while(Driver_USART6.GetRxCount()<7); //block until char is received
        while(scan_done!=1);
        //GPIOI->BSRR = (1<<16); 
        size=sprintf(text,"%x %x %x %x %x %x %x",data[0],data[1],data[2],data[3],data[4],data[5],data[6]);
        GLCD_DrawString(0,30,text);
        //Driver_USART7.Send(text,size);
		//while(Driver_USART7.GetTxCount()!=size);
        Driver_USART6.Send(stop,2);
        while(Driver_USART6.GetTxCount()!=2);
        GPIOI->ODR &= ~(1<<0); 
        
        
        for(count = 7;count<(5*num_sample + 7);count=count+5)
		{
            
			quality = data[count]>>2;
			
			angle=(((unsigned short)data[count+1])>>1)+(((unsigned short)data[count+2])<<7);
            distance=((unsigned short)data[count+3])+(((unsigned short)data[count+4])<<8);
			
            
			if (distance!=0)
		{
                
				angle_f = ((float)angle)/64.0;
				distance_f = ((float)distance)/4.0;
				
				//n=sprintf(texte,"%x %x %x %x %x\n",sample[count],sample[count+1],sample[count+2],sample[count+3],sample[count+4]);
				
				//Driver_USART7.Send(texte,n);
				//while(Driver_USART7.GetTxCount()!=n);
				x_offset = (int)(distance_f*100.0/6000.0*cos(2*3.1415*angle_f/360.0));
				if (x_offset>240)
					x_offset = 240;
				else if (x_offset<-240)
					x_offset = -240;
				
				y_offset = (int)(distance_f*100.0/6000.0*sin(2*3.1415*angle_f/360.0));
				if (y_offset>136)
					y_offset = 136;
				else if (y_offset<-136)
					y_offset = -136;
				
				
				//GLCD_DrawPixel(240 + x_offset, 136 + y_offset);
				
				n=sprintf(text,"%d %3.4f %6.2f\n",quality,angle_f,distance_f);
				
				
			}
            
			Driver_USART7.Send(&data[count],5);
			while(Driver_USART7.GetTxCount()!=5);
			
		}
        
    }
    else if(!strcmp("set",head)){
        GLCD_DrawString(300,0,"set");
        if(!strcmp("turn",sub1)){
            GLCD_DrawString(300,20,"turn");
            if(atoi(sub2) > 0 && atoi(sub2) <= 100){
                GLCD_DrawString(300,40,sub2);
                pos_arr = ((int)(((((double)(atoi(sub2)))*2967)/100)+62134));
                pos_ccr1 = pos_arr - 59177;	
                TIM13->ARR = pos_arr;
                TIM13->CCR1 = pos_ccr1;
                sprintf(pos_arr_str,"%d",pos_arr); 
                sprintf(pos_ccr1_str,"%d",pos_ccr1);
                GLCD_DrawString(0,24,pos_arr_str); 
                GLCD_DrawString(0,48,pos_ccr1_str);
            }
            else if(!strcmp("pot",sub2)){
                GLCD_DrawString(300,0,"pot");
                ADC3->CR2 |= (1<<30); 					//start ADC conversion
                while((ADC3->SR&(1<<1))==0); 		//block and wait for conversion to finish
                pos_arr = ((int)(((((double)(ADC3->DR))*2967)/4096)+62134));
                pos_ccr1 = pos_arr - 59177;	
                TIM13->ARR = pos_arr;
                TIM13->CCR1 = pos_ccr1;
                sprintf(pos_arr_str,"%d",pos_arr); 
                sprintf(pos_ccr1_str,"%d",pos_ccr1);
                GLCD_DrawString(0,24,pos_arr_str); 
                GLCD_DrawString(0,48,pos_ccr1_str);
                
            }
            else{
                GLCD_DrawString(300,40,sub2);
                Driver_USART7.Send("Invalid input. Please put a value between 0 and 100",51);
                return(-1);
            }
        }
        else if(!strcmp("drive",sub1)){
            GLCD_DrawString(300,20,"drive");
            if(atoi(sub2) >= 0 && atoi(sub2) <= 100){
                GLCD_DrawString(300,40,sub2);
                drive_ccr2 = (int)((((double)(atoi(sub2))/100)*8640));
                TIM12->CCR2 = drive_ccr2; //defines duty cycle
                sprintf(drive_ccr1_str,"%d",drive_ccr2); 
                GLCD_DrawString(0,24,drive_ccr1_str); 
            }
            else{
                GLCD_DrawString(300,40,sub2);
                Driver_USART7.Send("Invalid input. Please put a value between 0 and 100",51);

                return(-1);
            }
        }
        else{
            Driver_USART7.Send("Invalid subcommand.",19);
            return -1;
        }
    }
    else{ 
			//GLCD_ClearScreen();
            GLCD_DrawString(300,0,"print");
			for(i=0; i<=command->length; i++){
				switch (i){
					case 0:
						GLCD_DrawString(0,0,head);
					case 1:
						GLCD_DrawString(0,20,sub1);
					case 2:
						GLCD_DrawString(0,40,sub2);
					case 3:
						GLCD_DrawString(0,60,sub3);
					case 4:
						GLCD_DrawString(0,80,sub4);
				}		
			}
            return(-1);
		}
    
    return(-1);
}

int count(int n){
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
    count((int)((87*mm)/10));
    drive_ccr2 = (int)((((double)(80)/100)*8640));
    TIM12->CCR2 = drive_ccr2; //defines duty cycle
};

int set_wheels(int pos)
{
    int pos_arr;
    int pos_ccr1;
    if(pos>= 8 && pos <= 58) //max turn values for car
    {
        pos_arr = ((int)((((double)(pos))*2967)/100)+62134);
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
        GLCD_DrawString(0,20,"data recieved");
		scan_done=1;
	}
  
  if (event & ARM_USART_EVENT_SEND_COMPLETE) UART6_sent = 1;
}

void myUSART7_callback(uint32_t event)
{
    if (event & ARM_USART_EVENT_SEND_COMPLETE) UART7_sent = 1;
}

void lidar_scan(void){
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
        while(Driver_USART6.GetTxCount()!=2);
        //while(Driver_USART6.GetRxCount()<7); //block until char is received
        while(scan_done!=1);
        //GPIOI->BSRR = (1<<16); 
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
            //if (distance!=0)
			
				//angle_f = ((float)angle)/64.0;
				//distance_f = ((float)distance)/4.0;

				//size=sprintf(data_out,"%d %3.4f %6.2f\n",quality,angle_f,distance_f);
                
                //GLCD_DrawString(0,(n-3)*15,data_out);
				//Driver_USART7.Send(data_out,size);
                Driver_USART7.Send(data_out,data[n]);
				while(Driver_USART7.GetTxCount()!=size);
                
			
        }
    }