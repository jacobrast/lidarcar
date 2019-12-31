#include <stdio.h>
#include "stub.h"
#include "cli.h"

char *new_line = "\n\r";
int dist = 0;

void rx_stub(char *str, int len);
int get_rx_count(void);
void tx_stub(char *str, int len);
int get_tx_count(void);

void UART_init(ARM_DRIVER_USART *uart){
	uart->Receive = &rx_stub;
	uart->GetRxCount = &get_rx_count;
	uart->Send = &tx_stub;
	uart->GetTxCount = &get_tx_count;
}

static int rx_count;
static int tx_count;

void GLCD_DrawString(int x, int y, char *str)
{
	printf("%s", str);
}

void GLCD_ClearScreen(void){};

void rx_stub(char *str, int len)
{
}

int get_rx_count(void)
{
	return 0;
}

void tx_stub(char *str, int len)
{
}

int get_tx_count(void)
{
	return 0;
}

int main(void)
{
	UART_init(&Driver_USART7);

	return 0;
}
