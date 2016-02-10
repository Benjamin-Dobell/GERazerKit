#include "GERazerStandardMessages.h"
#include "GERazerUtils.h"

//! It looks like Razer were going to support configurable response ports, but then decided not to.
//! Nevertheless, if the key is not included response messages are messed up.
const CFStringRef kGERazerResponseMsgPortNameDefault = CFSTR("MambaRGBConfiguratorReplyPort");

GERazerMessageRef GERazerMessageCreateAttachedDevicesRequest(void)
{
	GERazerMessageRef message = GERazerMessageCreate(kGERazerMessageIdAttachedDevices);
	CFDictionaryAddValue(GERazerMessageGetData(message), CFSTR("ResponseMsgPortName"), kGERazerResponseMsgPortNameDefault);
	return message;
}

GERazerMessageRef GERazerMessageCreateAllSettingsRequest(SInt32 productId)
{
	CFNumberRef boxedProductId = CFNumberCreate(kCFAllocatorDefault, kCFNumberSInt32Type, &productId);

	GERazerMessageRef message = GERazerMessageCreate(kGERazerMessageIdDeviceAllSettings);
	CFDictionaryAddValue(GERazerMessageGetData(message), CFSTR("ProductID"), boxedProductId);
	CFDictionaryAddValue(GERazerMessageGetData(message), CFSTR("ResponseMsgPortName"), kGERazerResponseMsgPortNameDefault);

	CFRelease(boxedProductId);

	return message;
}

GERazerMessageRef GERazerMessageCreateAssignDeviceSettingsRequest(SInt32 productId, CFStringRef profileId, CFDictionaryRef deviceSettings)
{
	CFNumberRef boxedProductId = CFNumberCreate(kCFAllocatorDefault, kCFNumberSInt32Type, &productId);

	GERazerMessageRef message = GERazerMessageCreate(kGERazerMessageIdAssignDeviceSettings);
	CFMutableDictionaryRef messageDictionary = GERazerMessageGetData(message);

	CFDictionarySetValue(messageDictionary, CFSTR("ProductID"), boxedProductId);
	CFDictionarySetValue(messageDictionary, CFSTR("ProfileID"), profileId);
	CFDictionarySetValue(messageDictionary, CFSTR("ResponseMsgPortName"), kGERazerResponseMsgPortNameDefault);
	GERazerDictionarySetThenReleaseValue(messageDictionary, CFSTR("DeviceSettingsDictionary"), CFPropertyListCreateDeepCopy(kCFAllocatorDefault, deviceSettings, kCFPropertyListMutableContainersAndLeaves));

	CFRelease(boxedProductId);

	return message;
}

CFMutableDictionaryRef GERazerDeviceSettingsCreateWithLedFollowingProduct(SInt32 productId, bool followingEnabled)
{
	// Led chroma follow

	char enabled = followingEnabled ? (char) 1 : (char) 0;

	CFNumberRef boxedProductId = CFNumberCreate(kCFAllocatorDefault, kCFNumberSInt32Type, &productId);
	CFNumberRef boxedEnabled = CFNumberCreate(kCFAllocatorDefault, kCFNumberCharType, &enabled);

	CFMutableDictionaryRef ledChromaFollowDictionary = CFDictionaryCreateMutable(kCFAllocatorDefault, 0, &kCFTypeDictionaryKeyCallBacks, &kCFTypeDictionaryValueCallBacks);
	CFDictionarySetValue(ledChromaFollowDictionary, CFSTR("PID"), boxedProductId);
	CFDictionarySetValue(ledChromaFollowDictionary, CFSTR("Enabled"), boxedEnabled);

	CFRelease(boxedProductId);
	CFRelease(boxedEnabled);

	// Device settings dictionary

	CFMutableDictionaryRef deviceSettingsDictionary = CFDictionaryCreateMutable(kCFAllocatorDefault, 0, &kCFTypeDictionaryKeyCallBacks, &kCFTypeDictionaryValueCallBacks);
	CFDictionarySetValue(deviceSettingsDictionary, CFSTR("LEDChromaFollow"), ledChromaFollowDictionary);

	CFRelease(ledChromaFollowDictionary);

	return deviceSettingsDictionary;
}

CFMutableDictionaryRef GERazerDeviceSettingsCreateWithLedEffectList(CFDictionaryRef ledEffectList)
{
	CFMutableDictionaryRef deviceSettingsDictionary = CFDictionaryCreateMutable(kCFAllocatorDefault, 0, &kCFTypeDictionaryKeyCallBacks, &kCFTypeDictionaryValueCallBacks);
	GERazerDictionarySetThenReleaseValue(deviceSettingsDictionary, CFSTR("LEDEffectList"), CFPropertyListCreateDeepCopy(kCFAllocatorDefault, ledEffectList, kCFPropertyListMutableContainersAndLeaves));
	return deviceSettingsDictionary;
}

CFMutableDictionaryRef GERazerDeviceSettingsCreateWithEnabledLightingEffect(SInt32 ledId, SInt32 effectId)
{
	static const SInt32 enabled = 1;

	// Led lighting dictionary

	CFNumberRef boxedLedId = CFNumberCreate(kCFAllocatorDefault, kCFNumberSInt32Type, &ledId);
	CFNumberRef boxedEffectId = CFNumberCreate(kCFAllocatorDefault, kCFNumberSInt32Type, &effectId);
	CFNumberRef boxedEnabled = CFNumberCreate(kCFAllocatorDefault, kCFNumberSInt32Type, &enabled);

	CFMutableDictionaryRef ledLightingDictionary = CFDictionaryCreateMutable(kCFAllocatorDefault, 0, &kCFTypeDictionaryKeyCallBacks, &kCFTypeDictionaryValueCallBacks);
	CFDictionarySetValue(ledLightingDictionary, CFSTR("LEDID"), boxedLedId);
	CFDictionarySetValue(ledLightingDictionary, CFSTR("Effect"), boxedEffectId);
	CFDictionarySetValue(ledLightingDictionary, CFSTR("Enabled"), boxedEnabled);

	CFRelease(boxedLedId);
	CFRelease(boxedEffectId);
	CFRelease(boxedEnabled);

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
