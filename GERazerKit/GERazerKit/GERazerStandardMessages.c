#include "GERazerStandardMessages.h"

GERazerMessageRef GERazerMessageListAttachedDevicesCreate()
{
	GERazerMessageRef message = GERazerMessageCreate(kGERazerMessageIdListAttachedDevices);
	CFDictionaryAddValue(GERazerMessageGetData(message), CFSTR("ResponseMsgPortName"), CFSTR("IgnoredButRequired"));
	return message;
}
