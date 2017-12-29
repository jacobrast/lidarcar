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

typedef struct command{
		char head[20];
		char sub1[20];
		char sub2[20];
		char sub3[20];
		char sub4[20];
		int length;
}command_t;

int get_command(command_t *command);
int parse_command(command_t *command);
void add_command(command_t *command);
