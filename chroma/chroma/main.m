//
//  main.m
//  chroma
//
//  Created by Benjamin Dobell on 8/02/2016.
//  Copyright © 2016 Glass Echidna Pty Ltd. All rights reserved.
//

#import <Foundation/Foundation.h>
#import <GERazerKit/GERazerKit.h>
#import <GERazerKit/GERazerMamba.h>

int main(int argc, const char *argv[])
{
	SInt32 connectStatus = GERazerConnect(NULL);

	if (connectStatus == kGERazerConnectionFailed)
	{
		fprintf(stderr, "Failed to connect to the Razer Device Manager.\n");
		return 1;
	}
	else if (connectStatus == kGERazerConnectionSendOnly)
	{
		fprintf(stderr, "Could only establish 1-way communication with the Razer Device Manager.\n");
		fprintf(stderr, "Please ensure the Synapse Configurator is not running.\n");
		return 1;
	}

	NSArray *attachedDevices = (__bridge_transfer NSArray *) GERazerCopyAttachedDeviceIds();

	if ([attachedDevices count] == 0)
	{
		fprintf(stderr, "Unable to detect any attached devices.\n");
		return 2;
	}

	SInt32 productId = [attachedDevices[0] intValue];

	NSString *profileId = (__bridge_transfer NSString *) GERazerCopyActiveProfileId(productId);

	if (!profileId)
	{
		fprintf(stderr, "Unable to determine the active profile for product id = %d.\n", productId);
		return 2;
	}

	GERazerMessageRef message = GERazerMessageCreateEnableLedEffectRequest(productId, (__bridge CFStringRef) profileId, kLedIdMamba, kGERazerEffectIdWave);
	GERazerSendMessage(message);
	GERazerMessageRelease(message);

	printf("Done");

	return 0;
}
