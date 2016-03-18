#include "GERazerStandardMessages.h"
#include "GERazerUtils.h"

//! It looks like Razer were going to support configurable response ports, but then decided not to.
//! Nevertheless, if the key is not included response messages are messed up.
const CFStringRef kGERazerResponseMsgPortNameDefault = CFSTR("MambaRGBConfiguratorReplyPort");

GERazerMessageRef GERazerMessageCreateRetrieveAttachedDevices(void)
{
	GERazerMessageRef message = GERazerMessageCreate(kGERazerMessageIdAttachedProducts);
	CFDictionaryAddValue(GERazerMessageGetData(message), CFSTR("ResponseMsgPortName"), kGERazerResponseMsgPortNameDefault);
	return message;
}

GERazerMessageRef GERazerMessageCreateRetrieveProductAllSettings(SInt32 productId)
{
	GERazerMessageRef message = GERazerMessageCreate(kGERazerMessageIdProductAllSettings);
	CFMutableDictionaryRef messageDictionary = GERazerMessageGetData(message);

	GERazerDictionarySetThenReleaseValue(messageDictionary, CFSTR("ProductID"), CFNumberCreate(kCFAllocatorDefault, kCFNumberSInt32Type, &productId));
	CFDictionaryAddValue(messageDictionary, CFSTR("ResponseMsgPortName"), kGERazerResponseMsgPortNameDefault);

	return message;
}

GERazerMessageRef GERazerMessageCreateActivateProductProfile(SInt32 productId, CFStringRef profileId)
{
	GERazerMessageRef message = GERazerMessageCreate(kGERazerMessageIdProductActivateProfile);
	CFMutableDictionaryRef messageDictionary = GERazerMessageGetData(message);

	GERazerDictionarySetThenReleaseValue(messageDictionary, CFSTR("ProductID"), CFNumberCreate(kCFAllocatorDefault, kCFNumberSInt32Type, &productId));
	GERazerDictionarySetThenReleaseValue(messageDictionary, CFSTR("ProfileID"), CFStringCreateCopy(kCFAllocatorDefault, profileId));
	CFDictionarySetValue(messageDictionary, CFSTR("ResponseMsgPortName"), kGERazerResponseMsgPortNameDefault);

	return message;
}

GERazerMessageRef GERazerMessageCreateSaveAndActivateProductProfile(SInt32 productId, CFDictionaryRef profile)
{
	GERazerMessageRef message = GERazerMessageCreate(kGERazerMessageIdProductSaveAndActivateProfile);
	CFMutableDictionaryRef messageDictionary = GERazerMessageGetData(message);

	GERazerDictionarySetThenReleaseValue(messageDictionary, CFSTR("ProductID"), CFNumberCreate(kCFAllocatorDefault, kCFNumberSInt32Type, &productId));
	GERazerDictionarySetThenReleaseValue(messageDictionary, CFSTR("DeviceSettingsDictionary"), GERazerDictionaryCreateMutableDeepCopy(profile));
	CFDictionarySetValue(messageDictionary, CFSTR("ResponseMsgPortName"), kGERazerResponseMsgPortNameDefault);

	return message;
}

GERazerMessageRef GERazerMessageCreateDeleteProductProfile(SInt32 productId, CFStringRef profileId)
{
	GERazerMessageRef message = GERazerMessageCreate(kGERazerMessageIdProductDeleteProfile);
	CFMutableDictionaryRef messageDictionary = GERazerMessageGetData(message);

	GERazerDictionarySetThenReleaseValue(messageDictionary, CFSTR("ProductID"), CFNumberCreate(kCFAllocatorDefault, kCFNumberSInt32Type, &productId));
	GERazerDictionarySetThenReleaseValue(messageDictionary, CFSTR("ProfileID"), CFStringCreateCopy(kCFAllocatorDefault, profileId));
	CFDictionarySetValue(messageDictionary, CFSTR("ResponseMsgPortName"), kGERazerResponseMsgPortNameDefault);

	return message;
}

GERazerMessageRef GERazerMessageCreateAssignProductDeviceSettings(SInt32 productId, CFStringRef profileId, CFDictionaryRef deviceSettings)
{
	GERazerMessageRef message = GERazerMessageCreate(kGERazerMessageIdProductAssignDeviceSettings);
	CFMutableDictionaryRef messageDictionary = GERazerMessageGetData(message);

	GERazerDictionarySetThenReleaseValue(messageDictionary, CFSTR("ProductID"), CFNumberCreate(kCFAllocatorDefault, kCFNumberSInt32Type, &productId));
	GERazerDictionarySetThenReleaseValue(messageDictionary, CFSTR("ProfileID"), CFStringCreateCopy(kCFAllocatorDefault, profileId));
	CFDictionarySetValue(messageDictionary, CFSTR("ResponseMsgPortName"), kGERazerResponseMsgPortNameDefault);
	GERazerDictionarySetThenReleaseValue(messageDictionary, CFSTR("DeviceSettingsDictionary"), GERazerDictionaryCreateMutableDeepCopy(deviceSettings));

	return message;
}

CFMutableDictionaryRef GERazerDeviceSettingsCreateWithLedFollowingProduct(SInt32 productId, bool followingEnabled)
{
	// Led chroma follow

	char enabled = followingEnabled ? (char) 1 : (char) 0;

	CFMutableDictionaryRef ledChromaFollowDictionary = CFDictionaryCreateMutable(kCFAllocatorDefault, 0, &kCFTypeDictionaryKeyCallBacks, &kCFTypeDictionaryValueCallBacks);
	GERazerDictionarySetThenReleaseValue(ledChromaFollowDictionary, CFSTR("PID"), CFNumberCreate(kCFAllocatorDefault, kCFNumberSInt32Type, &productId));
	GERazerDictionarySetThenReleaseValue(ledChromaFollowDictionary, CFSTR("Enabled"), CFNumberCreate(kCFAllocatorDefault, kCFNumberCharType, &enabled));

	// Device settings dictionary

	CFMutableDictionaryRef deviceSettingsDictionary = CFDictionaryCreateMutable(kCFAllocatorDefault, 0, &kCFTypeDictionaryKeyCallBacks, &kCFTypeDictionaryValueCallBacks);
	CFDictionarySetValue(deviceSettingsDictionary, CFSTR("LEDChromaFollow"), ledChromaFollowDictionary);

	CFRelease(ledChromaFollowDictionary);

	return deviceSettingsDictionary;
}

CFMutableDictionaryRef GERazerDeviceSettingsCreateWithLedEffectList(CFDictionaryRef ledEffectList)
{
	CFMutableDictionaryRef deviceSettingsDictionary = CFDictionaryCreateMutable(kCFAllocatorDefault, 0, &kCFTypeDictionaryKeyCallBacks, &kCFTypeDictionaryValueCallBacks);
	GERazerDictionarySetThenReleaseValue(deviceSettingsDictionary, CFSTR("LEDEffectList"), GERazerDictionaryCreateMutableDeepCopy(ledEffectList));
	return deviceSettingsDictionary;
}

CFMutableDictionaryRef GERazerDeviceSettingsCreateWithEnabledLightingEffect(SInt32 ledId, SInt32 effectId)
{
	static const SInt32 enabled = 1;

	// Led lighting dictionary

	CFMutableDictionaryRef ledLightingDictionary = CFDictionaryCreateMutable(kCFAllocatorDefault, 0, &kCFTypeDictionaryKeyCallBacks, &kCFTypeDictionaryValueCallBacks);
	GERazerDictionarySetThenReleaseValue(ledLightingDictionary, CFSTR("LEDID"), CFNumberCreate(kCFAllocatorDefault, kCFNumberSInt32Type, &ledId));
	GERazerDictionarySetThenReleaseValue(ledLightingDictionary, CFSTR("Effect"), CFNumberCreate(kCFAllocatorDefault, kCFNumberSInt32Type, &effectId));
	GERazerDictionarySetThenReleaseValue(ledLightingDictionary, CFSTR("Enabled"), CFNumberCreate(kCFAllocatorDefault, kCFNumberSInt32Type, &enabled));

	// Lighting array

	CFMutableArrayRef lightingArray = CFArrayCreateMutable(kCFAllocatorDefault, 0, &kCFTypeArrayCallBacks);
	CFArrayAppendValue(lightingArray, ledLightingDictionary);

	CFRelease(ledLightingDictionary);

	// Device settings dictionary

	CFMutableDictionaryRef deviceSettingsDictionary = CFDictionaryCreateMutable(kCFAllocatorDefault, 0, &kCFTypeDictionaryKeyCallBacks, &kCFTypeDictionaryValueCallBacks);
	CFDictionarySetValue(deviceSettingsDictionary, CFSTR("Lighting"), lightingArray);

	CFRelease(lightingArray);

	return deviceSettingsDictionary;
}
