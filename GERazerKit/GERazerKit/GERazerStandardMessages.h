#if !defined(__GERAZERKIT_GERAZERSTANDARDMESSAGES__)
#define __GERAZERKIT_GERAZERSTANDARDMESSAGES__ 1

#include <GERazerKit/GERazerMessage.h>

CF_ENUM(SInt32)
{
	kGERazerMessageIdDeviceAllSettings = 4,
	kGERazerMessageIdDeviceSetPowerSavingOptions = 6,
	kGERazerMessageIdDeviceFirmwareVersion = 14,
	kGERazerMessageIdDeviceBatteryPercentage = 15,
	kGERazerMessageIdDeviceChargingStatus = 16,
	kGERazerMessageIdListAttachedDevices = 56,
	kGERazerMessageIdDeviceInfo = 68,
	kGERazerMessageIdDeviceSurfaceCalibration = 74,
	kGERazerMessageIdDeviceSerialNumber = 82
};

GERazerMessageRef GERazerMessageListAttachedDevicesCreate();

#endif /* ! __GERAZERKIT_GERAZERSTANDARDMESSAGES__ */
