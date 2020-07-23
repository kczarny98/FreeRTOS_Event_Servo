#ifndef WATCH_H
#define WATCH_H

enum TimeUnit {SECONDS, MINUTES};

struct WatchEvent
{
	enum TimeUnit eTimeUnit;
	unsigned char TimeValue;
};

void Watch_Init(void);
struct WatchEvent sWatch_Read(void);

#endif
