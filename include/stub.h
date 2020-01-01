#ifndef STUB_H
#define STUB_H

#define ARM_USART_EVENT_RECEIVE_COMPLETE 1
#define ARM_USART_EVENT_SEND_COMPLETE 1
#define ARM_USART_EVENT_SEND_COMPLETE 1

typedef struct {
	void (* Receive)(char *, int);
	int (* GetRxCount)(void);
	void (* Send)(char *, int );
	int (* GetTxCount)(void);
} ARM_DRIVER_USART;

typedef struct {
	int ODR;
	int BSRR;
} GPIO;

typedef struct {
	int ARR;
	int CCR1;
	int CCR2;
} TIM;

typedef struct {
	int CR2;
	int SR;
	int DR;
} ADC;

void UART_init(ARM_DRIVER_USART *uart);
void rx_stub(char *str, int len);
void GLCD_DrawString(int x, int y, char *str);
void GLCD_ClearScreen(void);
void rx_open(FILE *fd);

int UART6_sent;
int UART7_sent;
ARM_DRIVER_USART Driver_USART7; 
ARM_DRIVER_USART Driver_USART6;
GPIO *GPIOI;
TIM *TIM12;
TIM *TIM13;
ADC * ADC3;

#endif
