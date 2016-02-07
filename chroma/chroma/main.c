//
//  main.c
//  chroma
//
//  Created by Benjamin Dobell on 8/02/2016.
//  Copyright © 2016 Glass Echidna Pty Ltd. All rights reserved.
//

#include <GERazerKit/GERazerKit.h>

void disconnected()
{
	printf("Oh!");
}

void messageReceived(GERazerMessageRef message)
{
	printf("Yay, a message.");
}

int main(int argc, const char *argv[])
{
	GERazerDeviceManagerConnect(&disconnected);
	GERazerDeviceReceiveMessage(kGERazerMessageIdListAttachedDevices, &messageReceived);

	GERazerMessageRef message = GERazerMessageListAttachedDevicesCreate();
	GERazerDeviceSendMessage(message);
	GERazerMessageRelease(message);

	CFRunLoopRun();

	return 0;
}
