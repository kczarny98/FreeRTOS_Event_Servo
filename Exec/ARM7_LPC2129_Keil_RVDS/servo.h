#ifndef SERVO_H
#define SERVO_H

enum ServoStateExtended {_CALLIB, _IDLE, _IN_PROGRESS, _WAITING};

struct ServoStatus
{
	enum ServoStateExtended eState;
	unsigned int uiPosition;
};

void Servo_Init(void);
void Servo_Callib(void);
void Servo_GoTo(unsigned int uiPosition);
void Servo_Wait(unsigned int uiWaitTime);
void Servo_Speed(unsigned int uiTickDelay);
struct ServoStatus Servo_State(void);

#endif
