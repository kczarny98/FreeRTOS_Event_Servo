#define UART_RX_BUFFER_SIZE 20
#define UART_TX_BUFFER_SIZE 20

void UART_InitWithInt(unsigned int uiBaudRate);
void UartTx_SendString(char *pcUartTx_Buffer);
void UartRx_GetString(char *pcUartRx_Buffer);
