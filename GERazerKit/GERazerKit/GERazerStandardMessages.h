#if !defined(__GERAZERKIT_GERAZERSTANDARDMESSAGES__)
#define __GERAZERKIT_GERAZERSTANDARDMESSAGES__ 1

#include <GERazerKit/GERazerMessage.h>

CF_ENUM(SInt32)
{
	kGERazerMessageIdDeviceAllSettings = 4,
	kGERazerMessageIdAssignDeviceSettings = 6,
	kGERazerMessageIdDeviceFirmwareVersion = 14,
	kGERazerMessageIdDeviceBatteryPercentage = 15,
	kGERazerMessageIdDeviceChargingStatus = 16,
	kGERazerMessageIdAttachedDevices = 56,
	kGERazerMessageIdDeviceBasicInfo = 68,
	kGERazerMessageIdDeviceConnected = 69,
	kGERazerMessageIdDeviceSurfaceCalibration = 74,
	kGERazerMessageIdDeviceSerialNumber = 82
};

extern const CFStringRef kGERazerResponseMsgPortNameDefault;

GERazerMessageRef GERazerMessageCreateAttachedDevicesRequest(void);

GERazerMessageRef GERazerMessageCreateAllSettingsRequest(SInt32 productId);

GERazerMessageRef GERazerMessageCreateEnableLedEffectRequest(SInt32 productId, CFStringRef profileId, SInt32 ledId, SInt32 effectId);

#endif /* ! __GERAZERKIT_GERAZERSTANDARDMESSAGES__ */
