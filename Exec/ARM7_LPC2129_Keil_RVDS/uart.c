#include <LPC21XX.H>
#include "FreeRTOS.h"
#include "queue.h"
#include "semphr.h"
#include "uart.h"
#include "string.h"

#define NULL 0

#define mPIN_FUNCTION_RXD_UART0 									 (1<<2)
#define mPIN_FUNCTION_TXD_UART0 									 (1<<0)

#define mDIVISOR_LATCH_ACCES_BIT                   (1<<7)
#define m8BIT_UART_WORD_LENGTH                     3

#define mRX_DATA_AVALIABLE_INTERRUPT_ENABLE        (1<<0)
#define mTHRE_INTERRUPT_ENABLE                     (1<<1)

#define mINTERRUPT_PENDING_IDETIFICATION_BITFIELD  0xF
#define mTHRE_INTERRUPT_PENDING                    (1<<1)
#define mRX_DATA_AVALIABLE_INTERRUPT_PENDING       (1<<2)

#define mFIFO_BUFFER_ENABLE 											 (1<<0)

#define VIC_UART0_CHANNEL_NR  6
#define VIC_UART1_CHANNEL_NR  7

#define mIRQ_SLOT_ENABLE                           (1<<5)

#define TERMINATOR '\r'

static xQueueHandle xUartRxQueue;
static xQueueHandle xUartTxQueue;

void UartTx_SendString(char *pcUartTx_Buffer) 
{
	unsigned char ucCharCtr = 0;
	unsigned char ucCharTmp = NULL;
	
	for (ucCharCtr = 0; pcUartTx_Buffer[ucCharCtr] != NULL; ucCharCtr++)
	{
		xQueueSendToBack(xUartTxQueue, &pcUartTx_Buffer[ucCharCtr], portMAX_DELAY);
	}
	xQueueSendToBack(xUartTxQueue, &ucCharTmp, portMAX_DELAY);
	xQueueReceive(xUartTxQueue, &ucCharTmp, portMAX_DELAY);
	U0THR = ucCharTmp;
}

void UartRx_GetString(char *pcUartRx_Buffer)
{
	char cCharReceived = ' ';
	unsigned char ucCharCtr = 0;
	
	xQueueReceive(xUartRxQueue, &cCharReceived, portMAX_DELAY);
	
	while(cCharReceived != TERMINATOR)
	{
		pcUartRx_Buffer[ucCharCtr++] = cCharReceived;
		xQueueReceive(xUartRxQueue, &cCharReceived, portMAX_DELAY);
	}
	pcUartRx_Buffer[ucCharCtr++] = NULL;
}

__irq void UART0_Interrupt(void) 
{   
   unsigned int uiCopyOfU0IIR = U0IIR; 
	 BaseType_t xHigherPriorityTaskWoken = pdTRUE;
	 char cReceivedChar, cTransmittedChar;
	
   if((uiCopyOfU0IIR & mINTERRUPT_PENDING_IDETIFICATION_BITFIELD) == mRX_DATA_AVALIABLE_INTERRUPT_PENDING) 
   {
		 cReceivedChar = U0RBR;
		 xQueueSendToBackFromISR(xUartRxQueue, &cReceivedChar, &xHigherPriorityTaskWoken);
   } 
   
   if((uiCopyOfU0IIR & mINTERRUPT_PENDING_IDETIFICATION_BITFIELD) == mTHRE_INTERRUPT_PENDING)              
   {
     if (xQueueReceiveFromISR(xUartTxQueue, &cTransmittedChar, &xHigherPriorityTaskWoken) == pdPASS)
		 {
			 if (cTransmittedChar != NULL) 
			 {
					U0THR = cTransmittedChar;
			 }
			 else
			 {
				 U0THR = TERMINATOR;
			 }
		 }
   }

   VICVectAddr = 0; 
}

void UART_InitWithInt(unsigned int uiBaudRate)
{	
	unsigned long ulDivisor, ulWantedClock;
	ulWantedClock=uiBaudRate*16;
	ulDivisor=15000000/ulWantedClock;
	
	// UART
	PINSEL0 = PINSEL0 | 0x55;
	U0LCR  |= m8BIT_UART_WORD_LENGTH | mDIVISOR_LATCH_ACCES_BIT;
	U0DLL = ( unsigned char ) ( ulDivisor & ( unsigned long ) 0xff );
	ulDivisor >>= 8;
	U0DLM = ( unsigned char ) ( ulDivisor & ( unsigned long ) 0xff );
	U0LCR  &= (~mDIVISOR_LATCH_ACCES_BIT);
	U0IER  |= mRX_DATA_AVALIABLE_INTERRUPT_ENABLE | mTHRE_INTERRUPT_ENABLE ;
	//U0FCR  |= mFIFO_BUFFER_ENABLE;

	// INT
	VICVectAddr1  = (unsigned long) UART0_Interrupt;
	VICVectCntl1  = mIRQ_SLOT_ENABLE | VIC_UART0_CHANNEL_NR;
	VICIntEnable |= (0x1 << VIC_UART0_CHANNEL_NR);
	
	xUartRxQueue = xQueueCreate(UART_RX_BUFFER_SIZE, sizeof(char));
	xUartTxQueue = xQueueCreate(UART_TX_BUFFER_SIZE, sizeof(char));
}
