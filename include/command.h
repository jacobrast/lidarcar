#ifndef COMMAND_H
#define COMMAND_H
#include "dict.h"

int clear_screen(List *argv);
int turn_cmd(List *argv);
int scan_cmd(List *argv);
int set_cmd(List *argv);
int drive_cmd(List *argv);
int print_cmd(List *argv);

extern entry command_list[8];
#endif
