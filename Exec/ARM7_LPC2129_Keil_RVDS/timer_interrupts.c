#include <LPC21xx.H>
#include "timer_interrupts.h"
#include "FreeRTOS.h"
#include "semphr.h"

#define mCOUNTER_ENABLE 0x00000001
#define mCOUNTER_RESET  0x00000002

#define mINTERRUPT_ON_MR0 0x00000001
#define mRESET_ON_MR0     0x00000002
#define mMR0_INTERRUPT    0x00000001

#define VIC_TIMER1_CHANNEL_NR 5

#define mIRQ_SLOT_ENABLE 0x00000020

xSemaphoreHandle xTimerISRSemaphore;

__irq void Timer1IRQHandler(void)
{
	BaseType_t xHigherPriorityTaskWoken = pdTRUE;
	T1IR=mMR0_INTERRUPT; 
	xSemaphoreGiveFromISR(xTimerISRSemaphore, &xHigherPriorityTaskWoken);
	VICVectAddr=0x00;
}

void Timer1Interrupts_Init(unsigned int uiPeriod)
{
	
	VICIntEnable |= (0x1 << VIC_TIMER1_CHANNEL_NR);
	VICVectCntl13  = mIRQ_SLOT_ENABLE | VIC_TIMER1_CHANNEL_NR;  
	VICVectAddr13  =(unsigned long)Timer1IRQHandler;
	
	T1MR0 = 15 * uiPeriod;                 	      
	T1MCR |= (mINTERRUPT_ON_MR0 | mRESET_ON_MR0);
	
	T1TCR |=  mCOUNTER_ENABLE;
	
	xTimerISRSemaphore = xSemaphoreCreateBinary();
}
