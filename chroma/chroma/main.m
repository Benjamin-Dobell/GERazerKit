//
//  main.m
//  chroma
//
//  Created by Benjamin Dobell on 8/02/2016.
//  Copyright © 2016 Glass Echidna Pty Ltd. All rights reserved.
//

#import <Foundation/Foundation.h>
#import <GERazerKit/GERazerKit.h>

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

	SInt32 followingProductId = GERazerGetLedFollowingProductId(productId);

	// Configure effects

	CFMutableDictionaryRef dockEffects = CFDictionaryCreateMutable(kCFAllocatorDefault, 0, &kCFTypeDictionaryKeyCallBacks, &kCFTypeDictionaryValueCallBacks);
	GERazerDictionarySetThenReleaseValue(dockEffects, kGERazerEffectNameSpectrumCycling, GERazerEffectCreateSpectrumCycling());
	GERazerDictionarySetThenReleaseValue(dockEffects, kGERazerEffectNameBreathing, GERazerEffectCreateBreathing(0.0, 1.0, 0.0, 1.0, 1.0, 1.0));
	GERazerDictionarySetThenReleaseValue(dockEffects, kGERazerEffectNameStatic, GERazerEffectCreateStatic(1.0, 1.0, 0.0));

	CFMutableDictionaryRef mouseEffects = CFDictionaryCreateMutable(kCFAllocatorDefault, 0, &kCFTypeDictionaryKeyCallBacks, &kCFTypeDictionaryValueCallBacks);
	GERazerDictionarySetThenReleaseValue(mouseEffects, kGERazerEffectNameSpectrumCycling, GERazerEffectCreateSpectrumCycling());
	GERazerDictionarySetThenReleaseValue(mouseEffects, kGERazerEffectNameBreathing, GERazerEffectCreateSpectrumCycling());
	GERazerDictionarySetThenReleaseValue(mouseEffects, kGERazerEffectNameWave, GERazerEffectCreateWave(kGERazerWaveDirectionBackToFront));
	GERazerDictionarySetThenReleaseValue(mouseEffects, kGERazerEffectNameStatic, GERazerEffectCreateStatic(1.0, 1.0, 0.0));
	GERazerDictionarySetThenReleaseValue(mouseEffects, kGERazerEffectNameReactive, GERazerEffectCreateReactive(0.0, 1.0, 0.0, kGERazerReactiveAfterglowDurationShort));

	CFStringRef dockIdString = GERazerStringCreateFromInt(kGERazerLedIdMambaDock);
	CFStringRef mouseIdString = GERazerStringCreateFromInt(kGERazerLedIdMambaMouse);

	CFMutableDictionaryRef effectList = CFDictionaryCreateMutable(kCFAllocatorDefault, 0, &kCFTypeDictionaryKeyCallBacks, &kCFTypeDictionaryValueCallBacks);
	CFDictionarySetValue(effectList, dockIdString, dockEffects);
	CFDictionarySetValue(effectList, mouseIdString, mouseEffects);

	CFRelease(dockEffects);
	CFRelease(mouseEffects);

	CFRelease(dockIdString);
	CFRelease(mouseIdString);

	CFMutableDictionaryRef deviceSettings = GERazerDeviceSettingsCreateWithLedEffectList(effectList);

	CFRelease(effectList);

	GERazerDictionaryRecursivelyMergeThenReleaseDictionary(deviceSettings, GERazerDeviceSettingsCreateWithEnabledLightingEffect(kGERazerLedIdMambaDock, kGERazerEffectIdBreathing));
	GERazerDictionaryRecursivelyMergeThenReleaseDictionary(deviceSettings, GERazerDeviceSettingsCreateWithEnabledLightingEffect(kGERazerLedIdMambaMouse, kGERazerEffectIdReactive));

	if (followingProductId != kGERazerProductIdNone)
	{
		GERazerDictionaryRecursivelyMergeThenReleaseDictionary(deviceSettings, GERazerDeviceSettingsCreateWithLedFollowingProduct(followingProductId, false));
	}

	GERazerMessageRef deviceSettingsMessage = GERazerMessageCreateAssignDeviceSettingsRequest(productId, (__bridge CFStringRef) profileId, deviceSettings);

	CFRelease(deviceSettings);

	GERazerSendMessage(deviceSettingsMessage);
	GERazerMessageRelease(deviceSettingsMessage);

	GERazerMessageRef receivedMessage;
	GERazerReceiveMessage(kGERazerMessageIdReturnDictionary, &receivedMessage, 1.0);

	if (receivedMessage)
	{
		GERazerMessageRelease(receivedMessage);
	}

	printf("Done\n");

	return 0;
}
