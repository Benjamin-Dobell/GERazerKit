#include "GERazerDeviceManager.h"
#include "GERazerKit.h"

CFMessagePortRef deviceManagerSendPort = NULL;
CFMessagePortRef deviceManagerReceivePort = NULL;

CFRunLoopSourceRef deviceManagerRunLoopSource = NULL;

CFMutableArrayRef deviceManagerDisconnnectedCallbacks = NULL;
CFMutableDictionaryRef deviceManagerReplyReceivedCallbacks = NULL;

CFMutableArrayRef GERazerDeviceManagerDisconnectedCallbacks()
{
	if (!deviceManagerDisconnnectedCallbacks)
	{
		deviceManagerDisconnnectedCallbacks = CFArrayCreateMutable(kCFAllocatorDefault, 0, NULL);
	}

	return deviceManagerDisconnnectedCallbacks;
}

CFMutableDictionaryRef GERazerDeviceManagerReplyReceivedCallbacks()
{
	if (!deviceManagerReplyReceivedCallbacks)
	{
		deviceManagerReplyReceivedCallbacks = CFDictionaryCreateMutable(kCFAllocatorDefault, 0, NULL, NULL);
	}

	return deviceManagerReplyReceivedCallbacks;
}

CFMutableArrayRef GERazerDeviceManagerGetMessageReplyReceivedCallbacks(SInt32 messageId)
{
	CFMutableDictionaryRef callbacks = GERazerDeviceManagerReplyReceivedCallbacks();
	return (CFMutableArrayRef) CFDictionaryGetValue(callbacks, (void *) (CFIndex) messageId);
}

void GERazerDeviceManagerAddReplyReceivedCallback(SInt32 messageId, GERazerDeviceManagerReplyReceivedCallback callback)
{
	CFMutableArrayRef messageCallbacks = GERazerDeviceManagerGetMessageReplyReceivedCallbacks(messageId);

	if (!messageCallbacks)
	{
		messageCallbacks = CFArrayCreateMutable(kCFAllocatorDefault, 0, NULL);
		CFDictionaryAddValue(GERazerDeviceManagerReplyReceivedCallbacks(), (void *) (CFIndex) messageId, messageCallbacks);
	}

	CFArrayAppendValue(messageCallbacks, callback);
}

void GERazerDeviceManagerPortInvalidated(CFMessagePortRef ms, void *info)
{
	CFRelease(deviceManagerSendPort);
	deviceManagerSendPort = NULL;

	CFArrayRef callbacks = GERazerDeviceManagerDisconnectedCallbacks();
	CFIndex count = CFArrayGetCount(callbacks);

	for (CFIndex i = 0; i < count; i++)
	{
		GERazerDeviceManagerDisconnectedCallback callback = CFArrayGetValueAtIndex(callbacks, i);
		callback();
	}

	if (deviceManagerReceivePort)
	{
		CFMessagePortInvalidate(deviceManagerReceivePort);
		CFRunLoopRemoveSource(GERazerRunLoopGet(), deviceManagerRunLoopSource, kCFRunLoopCommonModes);
		CFRelease(deviceManagerRunLoopSource);
		CFRelease(deviceManagerReceivePort);
		deviceManagerReceivePort = NULL;
	}
}

CFDataRef GERazerDeviceManagerReplyReceived(CFMessagePortRef local, SInt32 msgid, CFDataRef data, void *info)
{
	CFMutableArrayRef callbacks = GERazerDeviceManagerGetMessageReplyReceivedCallbacks(msgid);

	if (callbacks)
	{
		CFIndex count = CFArrayGetCount(callbacks);

		if (count > 0)
		{
			GERazerMessageRef message = NULL;
			CFDictionaryRef dictionary = CFPropertyListCreateWithData(kCFAllocatorDefault, data, 0, NULL, NULL);

			if (dictionary)
			{
				CFDataRef rzdata = CFDictionaryGetValue(dictionary, CFSTR("rzdata"));

				if (rzdata)
				{
					CFDictionaryRef rzdataDictionary = CFPropertyListCreateWithData(kCFAllocatorDefault, data, 0, NULL, NULL);
					message = GERazerMessageCreateWithDictionary(msgid, rzdataDictionary);
					CFRelease(rzdataDictionary);
				}

				CFRelease(dictionary);
			}

			if (message)
			{
				for (int i = 0; i < count; i++)
				{
					GERazerDeviceManagerReplyReceivedCallback callback = CFArrayGetValueAtIndex(callbacks, i);
					callback(message);
				}

				CFArrayRemoveAllValues(callbacks);
				GERazerMessageRelease(message);
			}
			else
			{
				printf("Received invalid message with ID %d, which has callbacks registered.", msgid);
			}
		}
	}

	return NULL;
}

SInt32 GERazerDeviceManagerConnect(GERazerDeviceManagerDisconnectedCallback callback)
{
	if (callback)
	{
		CFMutableArrayRef callbacks = GERazerDeviceManagerDisconnectedCallbacks();

		if (CFArrayGetFirstIndexOfValue(callbacks, CFRangeMake(0, CFArrayGetCount(callbacks)), callback) == kCFNotFound)
		{
			CFArrayAppendValue(callbacks, callback);
		}
	}

	if (!deviceManagerSendPort)
	{
		deviceManagerSendPort = CFMessagePortCreateRemote(kCFAllocatorDefault, CFSTR("RazerDeviceEngineMsgPort"));

		if (!deviceManagerSendPort)
		{
			return kGERazerDeviceManagerConnectionFailed;
		}

		CFMessagePortSetInvalidationCallBack(deviceManagerSendPort, GERazerDeviceManagerPortInvalidated);
	}

	if (!deviceManagerReceivePort)
	{
		CFMessagePortContext context;
		context.version = 0;
		context.info = NULL;
		context.retain = NULL;
		context.release = NULL;
		context.copyDescription = NULL;

		Boolean shouldFreeInfo = FALSE;

		deviceManagerReceivePort = CFMessagePortCreateLocal(NULL, CFSTR("EmilyConfiguratorReplyPort"), &GERazerDeviceManagerReplyReceived, &context, &shouldFreeInfo);

		if (deviceManagerReceivePort)
		{
			deviceManagerRunLoopSource = CFMessagePortCreateRunLoopSource(NULL, deviceManagerReceivePort, 0);
			CFRunLoopAddSource(GERazerRunLoopGet(), deviceManagerRunLoopSource, kCFRunLoopCommonModes);
		}
		else
		{
			return kGERazerDeviceManagerConnectionSendOnly;
		}
	}

	return kGERazerDeviceManagerConnectionSuccess;
}

void GERazerDeviceManagerDisconnect()
{
	if (deviceManagerSendPort)
	{
		CFMessagePortInvalidate(deviceManagerSendPort);
	}
}

void GERazerDeviceManagerRemoveDisconnectedCallback(GERazerDeviceManagerDisconnectedCallback callback)
{
	CFMutableArrayRef callbacks = GERazerDeviceManagerDisconnectedCallbacks();
	CFIndex index = CFArrayGetFirstIndexOfValue(callbacks, CFRangeMake(0, CFArrayGetCount(callbacks)), callback);

	if (index != kCFNotFound)
	{
		CFArrayRemoveValueAtIndex(callbacks, index);
	}
}

SInt32 GERazerDeviceSendMessage(GERazerMessageRef message)
{
	static const CFIndex rzcontflagZero = 0;
	static const void *rzkeys[] = { CFSTR("rzcontflag"), CFSTR("rzdata") };

	if (!deviceManagerSendPort)
	{
		GERazerDeviceManagerConnect(NULL);
	}

	if (deviceManagerSendPort)
	{
		CFDataRef rzdata = CFPropertyListCreateData(kCFAllocatorDefault, GERazerMessageGetData(message), kCFPropertyListBinaryFormat_v1_0, 0, NULL);

		if (!rzdata)
		{
			return kGERazerDeviceManagerSendSerializationFailed;
		}

		CFNumberRef rzcontflag = CFNumberCreate(kCFAllocatorDefault, kCFNumberCFIndexType, &rzcontflagZero);
		const void *rzvalues[] = {rzcontflag, rzdata};
		CFDictionaryRef dictionary = CFDictionaryCreate(kCFAllocatorDefault, rzkeys, rzvalues, 2, NULL, NULL);

		CFDataRef data = CFPropertyListCreateData(kCFAllocatorDefault, dictionary, kCFPropertyListBinaryFormat_v1_0, 0, NULL);

		CFRelease(dictionary);
		CFRelease(rzcontflag);
		CFRelease(rzdata);

		if (!data)
		{
			return kGERazerDeviceManagerSendSerializationFailed;
		}

		SInt32 result = CFMessagePortSendRequest(deviceManagerSendPort, GERazerMessageGetId(message), data, 60.0, 60.0, NULL, NULL);

		CFRelease(data);

		if (result != kCFMessagePortSuccess)
		{
			if (result == kCFMessagePortSendTimeout)
			{
				return kGERazerDeviceManagerSendTimeout;
			}
			else
			{
				return kGERazerDeviceManagerSendConnectionInvalid;
			}
		}
	}
	else
	{
		return kGERazerDeviceManagerSendConnectionInvalid;
	}

	return kGERazerDeviceManagerSendSuccess;
}

void GERazerDeviceReceiveMessage(SInt32 messageId, GERazerDeviceManagerReplyReceivedCallback callback)
{
	GERazerDeviceManagerAddReplyReceivedCallback(messageId, callback);
}
