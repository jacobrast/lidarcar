default: cli.c stub.c 
	gcc -o test cli.c stub.c -Iinclude
