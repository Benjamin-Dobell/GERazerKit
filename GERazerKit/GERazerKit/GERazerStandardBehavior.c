#include "GERazerStandardBehavior.h"
#include "GERazerIPC.h"
#include "GERazerStandardMessages.h"

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
