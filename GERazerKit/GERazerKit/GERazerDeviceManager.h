#if !defined(__GERAZERKIT_GERAZERDEVICEMANAGER__)
#define __GERAZERKIT_GERAZERDEVICEMANAGER__ 1

#include <CoreFoundation/CoreFoundation.h>
#include <GERazerKit/GERazerMessage.h>

CF_ENUM(SInt32)
{
	kGERazerDeviceManagerConnectionSuccess = 0,
	kGERazerDeviceManagerConnectionFailed = 1, // The Razer Device Manager is presumably not running
	kGERazerDeviceManagerConnectionSendOnly = 2 // We can only send and not receive because Razer Synapse is running.
};

CF_ENUM(SInt32)
{
	kGERazerDeviceManagerSendSuccess = 0,
	kGERazerDeviceManagerSendConnectionInvalid = 1,
	kGERazerDeviceManagerSendSerializationFailed = 2,
	kGERazerDeviceManagerSendTimeout = 3
};

typedef void (*GERazerDeviceManagerDisconnectedCallback)();

// GERazerDeviceSendMessage will automatically connect as necessary, however GERazerDeviceManagerConnect
// can be called (one or more times) to register a callback if a disconnection occurs.
SInt32 GERazerDeviceManagerConnect(GERazerDeviceManagerDisconnectedCallback callback);

void GERazerDeviceManagerDisconnect();

void GERazerDeviceManagerRemoveDisconnectedCallback(GERazerDeviceManagerDisconnectedCallback callback);

typedef void (*GERazerDeviceManagerReplyReceivedCallback)(GERazerMessageRef message);

SInt32 GERazerDeviceSendMessage(GERazerMessageRef message);

void GERazerDeviceReceiveMessage(SInt32 messageId, GERazerDeviceManagerReplyReceivedCallback callback);

#endif /* ! __GERAZERKIT_GERAZERDEVICEMANAGER__ */
