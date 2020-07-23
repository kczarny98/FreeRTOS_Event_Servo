#include "watch.h"
#include "FreeRTOS.h"
#include "task.h"
#include "semphr.h"
#include "queue.h"

#define TICKS_IN_SECOND 1000
#define WATCH_QUEUE_SIZE 1

static xQueueHandle xWatchQueue;

struct WatchEvent sWatch_Update(void)
{
	static unsigned char ucSeconds = 0;
	static unsigned char ucMinutes = 0;
	struct WatchEvent sWatchUpdatedValue;
	
	ucSeconds++;
	if (ucSeconds == 60)
	{
		ucSeconds = 0;
		ucMinutes++;
		sWatchUpdatedValue.eTimeUnit = MINUTES;
		sWatchUpdatedValue.TimeValue = ucMinutes;
		return sWatchUpdatedValue;
	}
	else
	{
		sWatchUpdatedValue.eTimeUnit = SECONDS;
		sWatchUpdatedValue.TimeValue = ucSeconds;
		return sWatchUpdatedValue;
	}
}

void Watch_Thread(void *pvParameters)
{
	struct WatchEvent sWatchUpdatedValue;
	
	while(1)
	{
		vTaskDelay(TICKS_IN_SECOND);
		sWatchUpdatedValue = sWatch_Update();
		xQueueSendToBack(xWatchQueue, &sWatchUpdatedValue, portMAX_DELAY);
	}
}

struct WatchEvent sWatch_Read(void)
{
	struct WatchEvent sWatchNewValue;
	xQueueReceive(xWatchQueue, &sWatchNewValue, portMAX_DELAY);
	return sWatchNewValue;
}

void Watch_Init(void)
{
	xWatchQueue = xQueueCreate(WATCH_QUEUE_SIZE, sizeof(struct WatchEvent));
	xTaskCreate(Watch_Thread, NULL, 128, NULL, 1, NULL);
}
