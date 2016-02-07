#include "GERazerKit.h"

CFRunLoopRef sharedRunLoop = NULL;

void GERazerRunLoopSet(CFRunLoopRef runLoop)
{
	sharedRunLoop = runLoop;
}

CFRunLoopRef GERazerRunLoopGet(void)
{
	if (sharedRunLoop)
	{
		return sharedRunLoop;
	}
	else
	{
		return CFRunLoopGetMain();
	}
}
