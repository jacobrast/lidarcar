#include "stub.h"
#include "list.h"

#define num_sample 500
#define timing_var 1
#define pause 40
#define duration_58 (int)((685.0)*3.14)
#define duration_45 870 
//(int)((815.0)*3.14)
#define duration_8 (int)((1334.0)*3.14)
#define duration_40 (int)((1380.0)*3.14)
#define duration_12 900 
//(int)((1585.0)*3.14)
#define duration_35 (int)((2090.0)*3.14)

int get_command(List **command, ARM_DRIVER_USART *driver);
void add_command(List *command);

//Temp for testing...
int UART_gets(char *str, ARM_DRIVER_USART *driver);

int cli_init(void);
int parse_command(List command);
int parse_subcommand(List *command, char ***argv);
