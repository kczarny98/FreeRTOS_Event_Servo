#include <LPC21xx.H>
#include "keyboard.h"
#include "FreeRTOS.h"
#include "queue.h"

#define S0_bm (1<<4)
#define S1_bm (1<<6)
#define S2_bm (1<<5)
#define S3_bm (1<<7)

#define KEYBOARD_QUEUE_SIZE 1

static xQueueHandle xKeyboardQueue;

enum KeyboardState eReadButtons(void) 
{ 
	if (!(IO0PIN & S0_bm))
	{
		return BUTTON_0;
	}
	else if (!(IO0PIN & S1_bm))
	{		
		return BUTTON_1;
	}
	else if (!(IO0PIN & S2_bm))
	{		
		return BUTTON_2;
	}
	else if (!(IO0PIN & S3_bm))
	{		
		return BUTTON_3;
	}
	else
	{ 
		return RELEASED;
	}
}

void Keyboard_Thread(void *pvParameters)
{
	enum KeyboardState eButtonPressed;
	
	while(1)
	{
		eButtonPressed = eReadButtons();
		
		if (RELEASED != eButtonPressed)
		{
			xQueueSendToBack(xKeyboardQueue, &eButtonPressed, portMAX_DELAY);
		}
	}
}

enum KeyboardState eKeyboard_Read(void)
{
	enum KeyboardState eButtonPressed;
	xQueueReceive(xKeyboardQueue, &eButtonPressed, portMAX_DELAY);
	return eButtonPressed;
}

void Keyboard_Init(void) 
{
	IO0DIR &= ~(S0_bm | S1_bm | S2_bm | S3_bm);
	xKeyboardQueue = xQueueCreate(KEYBOARD_QUEUE_SIZE, sizeof(enum KeyboardState));
	xTaskCreate(Keyboard_Thread, NULL, 128, NULL, 1, NULL);
}
