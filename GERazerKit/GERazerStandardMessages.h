#if !defined(__GERAZERKIT_GERAZERSTANDARDMESSAGES__)
#define __GERAZERKIT_GERAZERSTANDARDMESSAGES__ 1

#include <GERazerKit/GERazerMessage.h>

CF_ENUM(SInt32)
{
	kGERazerMessageIdProductAllSettings = 4,
	kGERazerMessageIdProductSaveAndActivateProfile = 5,
	kGERazerMessageIdProductAssignDeviceSettings = 6,
	kGERazerMessageIdProductDeleteProfile = 9,
	kGERazerMessageIdProductActivateProfile = 10,
	kGERazerMessageIdProductFirmwareVersion = 14,
	kGERazerMessageIdProductBatteryPercentage = 15,
	kGERazerMessageIdProductChargingStatus = 16,
	kGERazerMessageIdAttachedProducts = 56,
	kGERazerMessageIdProductReturnDictionary = 58, // Response to kGERazerMessageIdProductSaveAndActivateProfile and kGERazerMessageIdProductAssignDeviceSettings
	kGERazerMessageIdProductBasicInfo = 68,
	kGERazerMessageIdProductConnected = 69,
	kGERazerMessageIdProductSurfaceCalibration = 74,
	kGERazerMessageIdProductSerialNumber = 82,
	kGERazerMessageIdProductRgbLayers = 94
};

extern const CFStringRef kGERazerResponseMsgPortNameDefault;

GERazerMessageRef GERazerMessageCreateRetrieveAttachedDevices(void);

GERazerMessageRef GERazerMessageCreateRetrieveProductAllSettings(SInt32 productId);

GERazerMessageRef GERazerMessageCreateActivateProductProfile(SInt32 productId, CFStringRef profileId);

GERazerMessageRef GERazerMessageCreateSaveAndActivateProductProfile(SInt32 productId, CFDictionaryRef profile);

GERazerMessageRef GERazerMessageCreateDeleteProductProfile(SInt32 productId, CFStringRef profileId);

// Note: It seems like a Device Manager bug, but the specified a profileId will always be made
//       active - in effect making this message no different than GERazerMessageCreateSaveAndActivateProductProfile().
GERazerMessageRef GERazerMessageCreateAssignProductDeviceSettings(SInt32 productId, CFStringRef profileId, CFDictionaryRef deviceSettings);

CFMutableDictionaryRef GERazerDeviceSettingsCreateWithLedFollowingProduct(SInt32 productId, bool followingEnabled);

CFMutableDictionaryRef GERazerDeviceSettingsCreateWithLedEffectList(CFDictionaryRef ledEffectList);

CFMutableDictionaryRef GERazerDeviceSettingsCreateWithEnabledLightingEffect(SInt32 ledId, SInt32 effectId);

#endif /* ! __GERAZERKIT_GERAZERSTANDARDMESSAGES__ */
