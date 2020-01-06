#include <stdio.h>
#include <math.h>
#include <stdint.h>
#include <string.h>

#include "Driver_USART.h"               // ::CMSIS Driver:USART
#include "Board_GLCD.h"                 // ::Board Support:Graphic LCD
#include "stm32f7xx_hal.h"              // Keil::Device:STM32Cube HAL:Common

#include "cli.h"
#include "setup.h"
#include "list.h"
#include "dict.h"
#include "command.h"

extern ARM_DRIVER_USART Driver_USART7;
extern ARM_DRIVER_USART Driver_USART6;

CHTbl command_dict;

int count(int n);
int go_forward(int time);
int set_wheels(int pos);
void block(int ms);
void lidar_scan(void);

int UART_gets(char *str, ARM_DRIVER_USART *driver);

int get_command(List **command, ARM_DRIVER_USART *driver)
{
	char string[1024];
	char *token, *data;
	
	UART_gets(string, driver);

	*command = (List*)malloc(sizeof(List));
	list_init(*command, free);

	token = strtok(string, " ");

	while(token != NULL){
		data = (char *)malloc(strlen(token));
		strcpy(data, token);
		list_ins_next(*command, list_tail(*command), data);
		token = strtok(NULL, " ");
	}

	return 0;
}

int UART_gets(char *str, ARM_DRIVER_USART *driver)
{
	int count = 0;

	for(;;){
		driver->Receive(str, 1);
		while(driver->GetRxCount() != 1);
		count ++;

		if(*str == '\n' || *str == EOF){
			*str = '\0';
			return count;
		}

		str ++;
	}
}

int cli_init(void)
{
	int i = 0;

	dict_init(&command_dict, 64);
	List command;
	entry *item;

	while(command_list[i].key != NULL){
		dict_insert(&command_dict, &command_list[i]);
		i++;
	}

	return 0;
}

int parse_command(List command)
{
	entry *item;
	char *key;

	key = list_data(list_head(&command));	

	if (dict_lookup(&command_dict, key, &item) != 0){
		return -1;
	}

	return (*(int (*)(List *))item->value)(&command);
}

int parse_subcommand(List *command, char ***argv)
{
	// Step 1: Get size of subcommand
	// Step 2: Allocate memory
	// Step 3: Write into memory
	// Step 4: Return pointer to memory
	int i;
	ListElmt *arg;

	*argv = malloc(list_size(command) * sizeof(int));
	arg = list_head(command);

	for(i = 0; i < list_size(command); i++){
		*argv[i] = list_data(arg);
		arg = list_next(arg);
	}

	return 0;
}
