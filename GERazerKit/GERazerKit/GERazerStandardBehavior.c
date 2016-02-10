#include "GERazerStandardBehavior.h"
#include "GERazerIPC.h"
#include "GERazerStandardMessages.h"
#include "GERazerKit.h"

CFTimeInterval kGERazerStandardBehaviourReceiveTimeout = 1.0;

CFArrayRef GERazerCopyAttachedDeviceIds(void)
{
	GERazerMessageRef message = GERazerMessageCreateAttachedDevicesRequest();
	GERazerSendMessage(message);
	GERazerMessageRelease(message);

	GERazerMessageRef responseMessage;
	GERazerReceiveMessage(kGERazerMessageIdAttachedDevices, &responseMessage, kGERazerStandardBehaviourReceiveTimeout);

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
	GERazerMessageRef message = GERazerMessageCreateAllSettingsRequest(productId);
	GERazerSendMessage(message);
	GERazerMessageRelease(message);

	GERazerMessageRef responseMessage;
	GERazerReceiveMessage(kGERazerMessageIdDeviceAllSettings, &responseMessage, kGERazerStandardBehaviourReceiveTimeout);

	CFStringRef profileId = NULL;

	if (responseMessage)
	{
		profileId = GERazerMessageGetDataValue(responseMessage, CFSTR("AllDevSettings"), CFSTR("DeviceSettings"), CFSTR("ActiveProfile"), kGERazerTerminate);

		if (profileId)
		{
			profileId = CFStringCreateCopy(kCFAllocatorDefault, profileId);
		}

		GERazerMessageRelease(responseMessage);
	}

	return profileId;
}

SInt32 GERazerGetLedFollowingProductId(SInt32 productId)
{
	GERazerMessageRef message = GERazerMessageCreateAllSettingsRequest(productId);
	GERazerSendMessage(message);
	GERazerMessageRelease(message);

	GERazerMessageRef responseMessage;
	GERazerReceiveMessage(kGERazerMessageIdDeviceAllSettings, &responseMessage, kGERazerStandardBehaviourReceiveTimeout);

	SInt32 followingProductId = kGERazerProductIdNone;

	if (responseMessage)
	{
		CFNumberRef boxedFollowingProductId = GERazerMessageGetDataValue(responseMessage, CFSTR("AllDevSettings"), CFSTR("Profiles"), 0, CFSTR("LEDChromaFollow"), CFSTR("PID"), kGERazerTerminate);

		if (boxedFollowingProductId)
		{
			CFNumberGetValue(boxedFollowingProductId, kCFNumberSInt32Type, &followingProductId);
		}

		GERazerMessageRelease(responseMessage);
	}

	return followingProductId;
}
