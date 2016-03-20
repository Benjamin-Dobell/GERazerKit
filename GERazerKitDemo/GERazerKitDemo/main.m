//
//  main.m
//  chroma
//
//  Created by Benjamin Dobell on 8/02/2016.
//  Copyright © 2016 Glass Echidna Pty Ltd. All rights reserved.
//

#import <Foundation/Foundation.h>
#import <GERazerKit/GERazerKit.h>

CFStringRef profileName = CFSTR("GERazerKitDemo");

CFMutableDictionaryRef getFirstProfileWithName(CFArrayRef profiles, CFStringRef name)
{
	CFIndex profileCount = CFArrayGetCount(profiles);

	for (CFIndex i = 0; i < profileCount; i++)
	{
		CFStringRef profileName = GERazerMessageDataArrayGetValue(profiles, i, CFSTR("ProfileName"), kGERazerTerminate);

		if (CFEqual(profileName, name))
		{
			return (CFMutableDictionaryRef) CFArrayGetValueAtIndex(profiles, i);
		}
	}

	return NULL;
}

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

	NSArray *attachedDevices = (__bridge_transfer NSArray *) GERazerCopyAttachedProductIds();

	if ([attachedDevices count] == 0)
	{
		fprintf(stderr, "Unable to detect any attached devices.\n");
		return 2;
	}

	SInt32 productId = [attachedDevices[0] intValue];
	NSString *activeProfileId = (__bridge_transfer NSString *) GERazerCopyActiveProfileId(productId);

	if (!activeProfileId)
	{
		fprintf(stderr, "Unable to determine the active profile for product id = %d.\n", productId);
		return 2;
	}

	CFArrayRef profiles = GERazerCopyProductProfiles(productId);

	CFStringRef profileId = NULL;
	CFMutableDictionaryRef profile = getFirstProfileWithName(profiles, profileName);

	if (profile)
	{
		profileId = CFDictionaryGetValue(profile, CFSTR("ProfileID"));
		CFRetain(profileId);
	}
	else if (CFArrayGetCount(profiles) > 0)
	{
		CFUUIDRef uuid = CFUUIDCreate(kCFAllocatorDefault);
		profileId = CFUUIDCreateString(kCFAllocatorDefault, uuid);
		CFRelease(uuid);

		CFIndex activeProfileIndex = GERazerProfilesGetIndexForProfileId(profiles, (__bridge CFStringRef) activeProfileId);
		CFDictionaryRef templateProfile = CFArrayGetValueAtIndex(profiles, activeProfileIndex);

		profile = GERazerDictionaryCreateMutableDeepCopy(templateProfile);
		CFDictionarySetValue(profile, CFSTR("ProfileName"), profileName);
		CFDictionarySetValue(profile, CFSTR("ProfileID"), profileId);

		GERazerSaveProductProfile(productId, profile);

		CFRelease(profile);
	}

	CFRelease(profiles);

	if (!profileId)
	{
		fprintf(stderr, "Uh-oh! This demo isn't clever enough to create a profile from scratch.\nAt least one profile must exist so we can clone it.\n");
		return 2;
	}

	if (!CFEqual((__bridge CFStringRef) activeProfileId, profileId))
	{
		GERazerActivateProductProfile(productId, profileId);
	}

	SInt32 followingProductId = GERazerGetLedFollowingProductId(productId);

	// Configure effects

	CFMutableDictionaryRef dockEffects = CFDictionaryCreateMutable(kCFAllocatorDefault, 0, &kCFTypeDictionaryKeyCallBacks, &kCFTypeDictionaryValueCallBacks);
	GERazerDictionarySetThenReleaseValue(dockEffects, kGERazerEffectNameSpectrumCycling, GERazerEffectCreateSpectrumCycling());
	GERazerDictionarySetThenReleaseValue(dockEffects, kGERazerEffectNameBreathing, GERazerEffectCreateBreathing(0.0, 1.0, 0.0, 1.0, 1.0, 1.0));
	GERazerDictionarySetThenReleaseValue(dockEffects, kGERazerEffectNameStatic, GERazerEffectCreateStatic(1.0, 1.0, 0.0));

	CFMutableDictionaryRef mouseEffects = CFDictionaryCreateMutable(kCFAllocatorDefault, 0, &kCFTypeDictionaryKeyCallBacks, &kCFTypeDictionaryValueCallBacks);
	GERazerDictionarySetThenReleaseValue(mouseEffects, kGERazerEffectNameSpectrumCycling, GERazerEffectCreateSpectrumCycling());
	GERazerDictionarySetThenReleaseValue(mouseEffects, kGERazerEffectNameBreathing, GERazerEffectCreateBreathing(0.0, 1.0, 0.0, 1.0, 1.0, 1.0));
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

	GERazerDictionaryRecursivelyMergeThenReleaseDictionary(deviceSettings, GERazerDeviceSettingsCreateWithEnabledLightingEffect(kGERazerLedIdMambaDock, kGERazerEffectIdBreathing, kGERazerLightingBrightnessNormal));
	GERazerDictionaryRecursivelyMergeThenReleaseDictionary(deviceSettings, GERazerDeviceSettingsCreateWithEnabledLightingEffect(kGERazerLedIdMambaMouse, kGERazerEffectIdReactive, kGERazerLightingBrightnessNormal));

	if (followingProductId != kGERazerProductIdNone)
	{
		GERazerDictionaryRecursivelyMergeThenReleaseDictionary(deviceSettings, GERazerDeviceSettingsCreateWithLedFollowingProduct(followingProductId, false));
	}

	GERazerSetProductDeviceSettings(productId, profileId, deviceSettings);

	CFRelease(deviceSettings);
	CFRelease(profileId);

	printf("Done\n");

	return 0;
}
