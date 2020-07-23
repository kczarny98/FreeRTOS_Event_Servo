#include <LPC21xx.H>
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "led.h"
#include "servo.h"

#define DETECTOR_bm 			(1<<10)
#define COMMAND_QUEUE_LEN 8
#define STATUS_QUEUE_LEN 	1

enum CommandType {CMD_GOTO, CMD_CALLIB, CMD_WAIT, CMD_SPEED};
enum DetectorState {ACTIVE, INACTIVE};
enum ServoState {CALLIB, IDLE, IN_PROGRESS};

struct Command
{
	enum CommandType eType;
	unsigned int uiArgument;
};

struct Servo
{
	enum ServoState eState;
	unsigned int uiCurrentPosition;
	unsigned int uiDesiredPosition;
	unsigned int uiOffset;
	unsigned int uiTickDelayBetweenSteps;
};

static xQueueHandle xCmdToSendQueue;
static xQueueHandle xStatusQueue;

enum DetectorState eDetector_Read(void) 
{
	if ((IO0PIN & DETECTOR_bm) == 0) 
	{
		return ACTIVE;
	}
	else 
	{
		return INACTIVE;
	}
}

void Detector_Init(void) 
{
	IO0DIR = IO0DIR & (~DETECTOR_bm);
}

void Servo_StateMachine(void *pvParameters) 
{
	static struct Servo sServo = {IDLE, 0, 0, 0, 50};
	struct ServoStatus sServoStatus; 
	struct Command sCmdToProcess = {CMD_CALLIB, 0};
	
	while(1)
	{
		sServoStatus.eState = (enum ServoStateExtended)sServo.eState;
		sServoStatus.uiPosition = sServo.uiCurrentPosition;
		xQueueOverwrite(xStatusQueue, &sServoStatus);
		
		switch (sServo.eState) 
		{
			case CALLIB:
				if (eDetector_Read() == ACTIVE) 
				{
					sServo.eState = IDLE;
					sServo.uiCurrentPosition = sServo.uiOffset;
					sServo.uiDesiredPosition = 0;
				}
				else 
				{
					Led_StepLeft();
					sServo.eState = CALLIB;
				}
			break;
			case IDLE:
				xQueueReceive(xCmdToSendQueue, &sCmdToProcess, portMAX_DELAY);
				switch(sCmdToProcess.eType)
				{
					case CMD_CALLIB:
						sServo.eState = CALLIB;
					break;
					
					case CMD_GOTO:
						sServo.uiDesiredPosition = sCmdToProcess.uiArgument;
						sServo.eState = IN_PROGRESS;
					break;
					
					case CMD_SPEED:
						sServo.uiTickDelayBetweenSteps = sCmdToProcess.uiArgument;
						sServo.eState = IDLE;
					break;
					
					case CMD_WAIT:
						sServoStatus.eState = _WAITING;
						sServoStatus.uiPosition = sServo.uiCurrentPosition;
						xQueueOverwrite(xStatusQueue, &sServoStatus);
						vTaskDelay(sCmdToProcess.uiArgument);
						sServo.eState = IDLE;
					break;
					
					default:
						sServo.eState = IDLE;
					break;
				}
			break;
			case IN_PROGRESS:
				if (sServo.uiCurrentPosition < sServo.uiDesiredPosition) 
				{
					Led_StepRight();
					sServo.uiCurrentPosition++;
					sServo.eState = IN_PROGRESS;
				}
				else if (sServo.uiCurrentPosition > sServo.uiDesiredPosition) 
				{
					Led_StepLeft();
					sServo.uiCurrentPosition--;
					sServo.eState = IN_PROGRESS;
				}
				else 
				{
					sServo.eState = IDLE;
				}
			break;
		}
		
		vTaskDelay(sServo.uiTickDelayBetweenSteps);
	}
}

void Servo_Callib(void) 
{
	struct Command sCallibCmd = {CMD_CALLIB, 0};
	xQueueSendToBack(xCmdToSendQueue, &sCallibCmd, portMAX_DELAY);
}

void Servo_GoTo(unsigned int uiPosition) 
{
	struct Command sGotoCmd = {CMD_GOTO, 0};
	sGotoCmd.uiArgument = uiPosition;
	xQueueSendToBack(xCmdToSendQueue, &sGotoCmd, portMAX_DELAY);
}

void Servo_Wait(unsigned int uiWaitTime)
{
	struct Command sWaitCmd = {CMD_WAIT, 0};
	sWaitCmd.uiArgument = uiWaitTime;
	xQueueSendToBack(xCmdToSendQueue, &sWaitCmd, portMAX_DELAY);
}

void Servo_Speed(unsigned int uiTickDelay)
{
	struct Command sSpeedCmd = {CMD_SPEED, 0};
	sSpeedCmd.uiArgument = uiTickDelay;
	xQueueSendToBack(xCmdToSendQueue, &sSpeedCmd, portMAX_DELAY);
}

struct ServoStatus Servo_State(void)
{
	struct ServoStatus sServoStatus;
	xQueuePeek(xStatusQueue, &sServoStatus, portMAX_DELAY);
	return sServoStatus;
}

void Servo_Init() 
{
	struct Command sCallibCmd = {CMD_CALLIB, 0};

	Led_Init();
	Detector_Init();
	
	xCmdToSendQueue = xQueueCreate(COMMAND_QUEUE_LEN, sizeof(struct Command));
	xStatusQueue = xQueueCreate(STATUS_QUEUE_LEN, sizeof(struct ServoStatus));
	xQueueSendToBack(xCmdToSendQueue, &sCallibCmd, portMAX_DELAY);
	xTaskCreate(Servo_StateMachine, NULL, 128, NULL, 2, NULL);
}
