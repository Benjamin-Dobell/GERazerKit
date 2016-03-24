#if !defined(__GERAZERKIT_GERAZERSTANDARDBEHAVIOR__)
#define __GERAZERKIT_GERAZERSTANDARDBEHAVIOR__ 1

#include <CoreFoundation/CoreFoundation.h>

CF_ENUM(SInt32)
{
	kGERazerBehaviorFailed = -2147483648
};

CF_ENUM(SInt32)
{
	kGERazerProductIdNone = -1
};

// Helpers

CFIndex GERazerProfilesGetIndexForProfileId(CFArrayRef profiles, CFStringRef profileId);

// Standard IPC

CFArrayRef GERazerCopyAttachedProductIds(void);

CFStringRef GERazerCopyActiveProfileId(SInt32 productId);

CFDictionaryRef GERazerCopyActiveProfile(SInt32 productId);

CFMutableArrayRef GERazerCopyProductProfiles(SInt32 productId);

SInt32 GERazerGetBatteryPercentage(SInt32 productId);

bool GERazerSaveProductProfile(SInt32 productId, CFDictionaryRef profile);

bool GERazerActivateProductProfile(SInt32 productId, CFStringRef profileId);

bool GERazerDeleteProductProfile(SInt32 productId, CFStringRef profileId);

SInt32 GERazerGetLedFollowingProductId(SInt32 productId, CFStringRef profileId);

bool GERazerSetProductDeviceSettings(SInt32 productId, CFStringRef profileId, CFDictionaryRef deviceSettings);

#endif /* ! __GERAZERKIT_GERAZERSTANDARDBEHAVIOR__ */
