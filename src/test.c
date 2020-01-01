#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "stub.h"
#include "cli.h"
#include "list.h"
#include "dict.h"

void parse_command_test1(void)
{
	List command;

	cli_init();
	list_init(&command, NULL);

	list_ins_next(&command, list_tail(&command), "clear");
	parse_command(command);
}

void get_command_test1(void)
{
	ARM_DRIVER_USART UART7;
	UART_init(&UART7);
	char *file = "test_file.txt";
	char expected_string[1024];

	FILE *fd = fopen(file, "r");
	rx_open(fd);

	List  *command;
	ListElmt *element;

	get_command(&command, &UART7);

	element = list_head(command);
	while(!list_is_tail(element)){
		printf("%s\n", list_data(element));
		element = list_next(element);
	}
	printf("%s\n", list_data(element));

	remove(file);
	
}

void gets_test1(void)
{
	ARM_DRIVER_USART UART7;
	UART_init(&UART7);
	char expected_string[1024];

	FILE *fd = fopen("test_file.txt", "r");
	rx_open(fd);

	UART_gets(expected_string, &UART7);

	fclose(fd);
	printf("%s", expected_string);
}

void rx_test1(void)
{
	char s[1024];
	char *test_string = "this is a test string...";
	FILE *fd = fopen("test_file.txt", "w");
	fwrite(test_string, 1, strlen(test_string), fd);
	fclose(fd);

	rx_open(fd);
	rx_stub(s, 4);

	printf("%s", s);
}

void dict_test(void)
{
	CHTbl dict;
	entry test_entry, *entry2;

	dict_init(&dict, 4);
	test_entry.key = "John";
	test_entry.value = "Smith";

	dict_insert(&dict, &test_entry);
	dict_lookup(&dict, "John", &entry2);

	printf("%s\n", entry2->value);
}

int func(void)
{
	printf("Hello world!\n");
	return 0;
}

void dict_funct_test(void)
{
	CHTbl dict;
	entry test_entry, *entry2;

	dict_init(&dict, 4);
	test_entry.key = "John";
	test_entry.value = &func;

	dict_insert(&dict, &test_entry);
	dict_lookup(&dict, "John", &entry2);

	((int (*)(void))(entry2->value))();
}

int main(void)
{
	rx_test1();
	gets_test1();
	get_command_test1();
	
	dict_test();
	dict_funct_test();
	parse_command_test1();

	return 0;
}
