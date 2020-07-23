#include <LPC21xx.H>
#include "led.h"

#define LED0_bm (1<<16)
#define LED1_bm (1<<17)
#define LED2_bm (1<<18)
#define LED3_bm (1<<19)

enum Direction {LEFT, RIGHT};

void Led_Init(void) 
{
	IO1DIR = IO1DIR | (LED0_bm | LED1_bm | LED2_bm | LED3_bm); 
	IO1SET = LED0_bm;
}

void Led_Toggle(unsigned char ucLedIndex) 
{
	switch(ucLedIndex)
	{
		case 0:
			if (IO1PIN & LED0_bm)
			{
				IO1CLR = LED0_bm;
			}
			else
			{
				IO1SET = LED0_bm;
			}
		break;
		case 1:
			if (IO1PIN & LED1_bm)
			{
				IO1CLR = LED1_bm;
			}
			else
			{
				IO1SET = LED1_bm;
			}
		break;
		case 2:
			if (IO1PIN & LED2_bm)
			{
				IO1CLR = LED2_bm;
			}
			else
			{
				IO1SET = LED2_bm;
			}
		break;
		case 3:
			if (IO1PIN & LED3_bm)
			{
				IO1CLR = LED3_bm;
			}
			else
			{
				IO1SET = LED3_bm;
			}
		break;
		default:
		break;
	}
}

void Led_On(unsigned char ucLedIndex) 
{
	IO1CLR = LED0_bm | LED1_bm | LED2_bm | LED3_bm ;
	
	switch(ucLedIndex) 
	{
		case 0:
			IO1SET = LED0_bm;
		break;
		case 1:
			IO1SET = LED1_bm;
		break;
		case 2:
			IO1SET = LED2_bm;
		break;
		case 3:
			IO1SET = LED3_bm;
		break;
		default:
		break;
	}
}

void Led_Step(enum Direction eCurrentDirection) 
{
	static unsigned int Led_Index = 0;
	
	if (eCurrentDirection == LEFT)
	{
		Led_Index = (++Led_Index) % 4;
	}
	else if (eCurrentDirection == RIGHT)
	{
		Led_Index = (--Led_Index) % 4;
	}
	Led_On(Led_Index);
}

void Led_StepLeft(void) 
{
	Led_Step(LEFT);
}

void Led_StepRight(void) 
{
	Led_Step(RIGHT);
}
