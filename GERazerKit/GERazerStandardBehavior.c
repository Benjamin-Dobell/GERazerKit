#include "GERazerStandardBehavior.h"
#include "GERazerIPC.h"
#include "GERazerStandardMessages.h"
#include "GERazerUtils.h"

CFTimeInterval kGERazerStandardBehaviourReceiveTimeout = 1.0;

CFIndex GERazerProfilesGetIndexForProfileId(CFArrayRef profiles, CFStringRef profileId)
{
	CFIndex profileCount = CFArrayGetCount(profiles);

	for (CFIndex i = 0; i < profileCount; i++)
	{
		CFStringRef id = GERazerMessageDataArrayGetValue(profiles, i, CFSTR("ProfileID"), kGERazerTerminate);

		if (CFEqual(profileId, id))
		{
			return i;
		}
	}

	return kCFNotFound;
}

CFArrayRef GERazerCopyAttachedProductIds(void)
{
	GERazerMessageRef message = GERazerMessageCreateRetrieveAttachedDevices();
	GERazerSendMessage(message);
	GERazerMessageRelease(message);

	GERazerMessageRef responseMessage;
	GERazerReceiveMessage(kGERazerMessageIdAttachedProducts, &responseMessage, kGERazerStandardBehaviourReceiveTimeout);

	CFArrayRef productIds = NULL;

	if (responseMessage)
	{
		productIds = CFDictionaryGetValue(GERazerMessageGetData(responseMessage), CFSTR("AttachedDevices"));

		if (productIds)
		{
			productIds = CFArrayCreateCopy(kCFAllocatorDefault, productIds);
		}

		GERazerMessageRelease(responseMessage);
	}

	return productIds;
}

CFStringRef GERazerCopyActiveProfileId(SInt32 productId)
{
	GERazerMessageRef message = GERazerMessageCreateRetrieveProductAllSettings(productId);
	GERazerSendMessage(message);
	GERazerMessageRelease(message);

	GERazerMessageRef responseMessage;
	GERazerReceiveMessage(kGERazerMessageIdProductAllSettings, &responseMessage, kGERazerStandardBehaviourReceiveTimeout);

	CFStringRef profileId = NULL;

	if (responseMessage)
	{
		profileId = GERazerMessageDataGetValue(responseMessage, CFSTR("AllDevSettings"), CFSTR("DeviceSettings"), CFSTR("ActiveProfile"), kGERazerTerminate);

		if (profileId)
		{
			profileId = CFStringCreateCopy(kCFAllocatorDefault, profileId);
		}

		GERazerMessageRelease(responseMessage);
	}

	return profileId;
}

CFDictionaryRef GERazerCopyActiveProfile(SInt32 productId)
{
	GERazerMessageRef message = GERazerMessageCreateRetrieveProductAllSettings(productId);
	GERazerSendMessage(message);
	GERazerMessageRelease(message);

	GERazerMessageRef responseMessage;
	GERazerReceiveMessage(kGERazerMessageIdProductAllSettings, &responseMessage, kGERazerStandardBehaviourReceiveTimeout);

	CFMutableDictionaryRef activeProfile = NULL;

	if (responseMessage)
	{
		CFStringRef profileId = GERazerMessageDataGetValue(responseMessage, CFSTR("AllDevSettings"), CFSTR("DeviceSettings"), CFSTR("ActiveProfile"), kGERazerTerminate);
		CFArrayRef profiles = GERazerMessageDataGetValue(responseMessage, CFSTR("AllDevSettings"), CFSTR("Profiles"), kGERazerTerminate);

		if (profileId && profiles)
		{
			CFIndex profileIndex = GERazerProfilesGetIndexForProfileId(profiles, profileId);

			if (profileIndex != kCFNotFound)
			{
				activeProfile = GERazerDictionaryCreateMutableDeepCopy(CFArrayGetValueAtIndex(profiles, profileIndex));
			}
		}

		GERazerMessageRelease(responseMessage);
	}

	return activeProfile;
}

CFMutableArrayRef GERazerCopyProductProfiles(SInt32 productId)
{
	GERazerMessageRef message = GERazerMessageCreateRetrieveProductAllSettings(productId);
	GERazerSendMessage(message);
	GERazerMessageRelease(message);

	GERazerMessageRef responseMessage;
	GERazerReceiveMessage(kGERazerMessageIdProductAllSettings, &responseMessage, kGERazerStandardBehaviourReceiveTimeout);

	CFMutableArrayRef profiles = NULL;

	if (responseMessage)
	{
		CFArrayRef productProfiles = GERazerMessageDataGetValue(responseMessage, CFSTR("AllDevSettings"), CFSTR("Profiles"), kGERazerTerminate);
		profiles = GERazerArrayCreateMutableDeepCopy(productProfiles);

		GERazerMessageRelease(responseMessage);
	}

	return profiles;
}

bool GERazerSaveProductProfile(SInt32 productId, CFDictionaryRef profile)
{
	CFStringRef originalActiveProfileId = GERazerCopyActiveProfileId(productId);

	GERazerMessageRef message = GERazerMessageCreateSaveAndActivateProductProfile(productId, profile);
	SInt32 result = GERazerSendMessage(message);
	GERazerMessageRelease(message);

	bool success = result == kGERazerTransferSuccess;

	if (originalActiveProfileId)
	{
		if (success && !CFEqual(CFDictionaryGetValue(profile, CFSTR("ProfileID")), originalActiveProfileId))
		{
			success = GERazerActivateProductProfile(productId, originalActiveProfileId);
		}

		CFRelease(originalActiveProfileId);
	}

	return success;
}

bool GERazerActivateProductProfile(SInt32 productId, CFStringRef profileId)
{
	GERazerMessageRef message = GERazerMessageCreateActivateProductProfile(productId, profileId);
	GERazerSendMessage(message);
	GERazerMessageRelease(message);

	return true;
}

bool GERazerDeleteProductProfile(SInt32 productId, CFStringRef profileId)
{
	GERazerMessageRef message = GERazerMessageCreateDeleteProductProfile(productId, profileId);
	SInt32 result = GERazerSendMessage(message);
	GERazerMessageRelease(message);

	return result == kGERazerTransferSuccess;
}

SInt32 GERazerGetLedFollowingProductId(SInt32 productId)
{
	GERazerMessageRef message = GERazerMessageCreateRetrieveProductAllSettings(productId);
	GERazerSendMessage(message);
	GERazerMessageRelease(message);

	GERazerMessageRef responseMessage;
	GERazerReceiveMessage(kGERazerMessageIdProductAllSettings, &responseMessage, kGERazerStandardBehaviourReceiveTimeout);

	SInt32 followingProductId = kGERazerProductIdNone;

	if (responseMessage)
	{
		CFNumberRef boxedFollowingProductId = GERazerMessageDataGetValue(responseMessage, CFSTR("AllDevSettings"), CFSTR("Profiles"), 0, CFSTR("LEDChromaFollow"), CFSTR("PID"), kGERazerTerminate);

		if (boxedFollowingProductId)
		{
			CFNumberGetValue(boxedFollowingProductId, kCFNumberSInt32Type, &followingProductId);
		}

		GERazerMessageRelease(responseMessage);
	}

	return followingProductId;
}

bool GERazerSetProductDeviceSettings(SInt32 productId, CFStringRef profileId, CFDictionaryRef deviceSettings)
{
	GERazerMessageRef message = GERazerMessageCreateAssignProductDeviceSettings(productId, profileId, deviceSettings);
	GERazerSendMessage(message);
	GERazerMessageRelease(message);

	GERazerMessageRef responseMessage;
	GERazerReceiveMessage(kGERazerMessageIdProductReturnDictionary, &responseMessage, 1.0);

	if (responseMessage)
	{
		GERazerMessageRelease(responseMessage);
	}
	else
	{
		return false;
	}

	return true;
}
