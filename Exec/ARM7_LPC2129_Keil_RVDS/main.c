#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "keyboard.h"
#include "uart.h"
#include "servo.h"
#include "string.h"
#include "command_decoder.h"

#define EVENT_QUEUE_SIZE 		16
#define EVENT_STRING_LENGTH	20

xQueueHandle xEventQueue;

void KeyboardEvent_Thread(void *pvParameters)
{
	
	while(1)
	{
		switch (eKeyboard_Read())
		{
			case BUTTON_0:
				xQueueSendToBack(xEventQueue, "button 0x0000", portMAX_DELAY);
			break;
			
			case BUTTON_1:
				xQueueSendToBack(xEventQueue, "button 0x0001", portMAX_DELAY);
			break;
			
			case BUTTON_2:
				xQueueSendToBack(xEventQueue, "button 0x0002", portMAX_DELAY);
			break;
			
			case BUTTON_3:
				xQueueSendToBack(xEventQueue, "button 0x0003", portMAX_DELAY);
			break;
			
			case RELEASED:
			default:
			break;
		}
		
		vTaskDelay(100);
	}
}

void UartRxEvent_Thread(void *pvParameters)
{
	char acUartEventBuffer[EVENT_STRING_LENGTH] = "";
	
	while(1)
	{
		UartRx_GetString(acUartEventBuffer);
		xQueueSendToBack(xEventQueue, acUartEventBuffer, portMAX_DELAY);
	}
}

void Executor_Thread(void *pvParameters)
{
	struct ServoStatus sServoStatus;
	char acEventBuffer[EVENT_STRING_LENGTH] = "";
	char acUartTxBuffer[UART_TX_BUFFER_SIZE] = "";
	
	while(1)
	{
		xQueueReceive(xEventQueue, acEventBuffer, portMAX_DELAY);
		DecodeMsg(acEventBuffer);
		if (asToken[0].eType == KEYWORD) 
		{
			switch (asToken[0].uValue.eKeyword) 
			{
				case ID:
					UartTx_SendString("id my_EVM\n");
				break;
				
				case CL:
					Servo_Callib();
					UartTx_SendString("ok\n");
				break;
				
				case GT:
					Servo_GoTo(asToken[1].uValue.uiNumber);
					UartTx_SendString("ok\n");
				break;
				
				case BT:
					switch(asToken[1].uValue.uiNumber)
					{
						case 0: 
							Servo_Callib(); 
						break;
						
						case 1: 
							Servo_GoTo(12); 
						break;
						
						case 2: 
							Servo_GoTo(24); 
						break;
						
						case 3: 
							Servo_GoTo(36);
							Servo_Wait(400);
						break;
						
						default: break;
					}
				break;
					
				case ST:
					sServoStatus = Servo_State();
					switch(sServoStatus.eState)
					{
						case _CALLIB: 			CopyString("state callib ", acUartTxBuffer); 			break;
						case _IDLE:					CopyString("state idle ", acUartTxBuffer); 				break;
						case _IN_PROGRESS: 	CopyString("state in_progress ", acUartTxBuffer); break;
						case _WAITING:			CopyString("state waiting ", acUartTxBuffer); 		break;
						default: 																															break;
					}
					AppendUIntToString(sServoStatus.uiPosition, acUartTxBuffer);
					AppendString("\n", acUartTxBuffer);
					UartTx_SendString(acUartTxBuffer);
				break;
				
				default:
					UartTx_SendString("unknown command\n");
				break;
			}	
		}
		else 
		{
			UartTx_SendString("unknown command\n");
		}
	}
}

int main(void)
{
	UART_InitWithInt(9600);
	Keyboard_Init();
	Servo_Init();
	
	xEventQueue = xQueueCreate(EVENT_QUEUE_SIZE, (sizeof(char) * EVENT_STRING_LENGTH));
	xTaskCreate(KeyboardEvent_Thread, NULL, 128, NULL, 1, NULL);
	xTaskCreate(UartRxEvent_Thread, NULL, 128, NULL, 1, NULL);
	xTaskCreate(Executor_Thread, NULL, 128, NULL, 1, NULL);
	vTaskStartScheduler();
	while(1);
}
