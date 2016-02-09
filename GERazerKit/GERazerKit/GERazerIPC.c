#include "GERazerIPC.h"

CFMessagePortRef sendPort = NULL;
CFMessagePortRef receivePort = NULL;

CFRunLoopRef receiveRunLoop = NULL;
CFRunLoopSourceRef receiveRunLoopSource = NULL;

bool receiveIsBlocking = false;
CFMutableArrayRef blockingReceiveMessages = false;
SInt32 *blockingReceiveMessageIds = NULL;
CFIndex blockingReceiveMessageIdCount = 0;
bool *blockingReceiveReceivedMessageIdMap = NULL;

CFRunLoopRef GERazerReceiveRunLoopGet(void)
{
	return receiveRunLoop;
}

CFMutableArrayRef GERazerDeviceManagerDisconnectedCallbacks()
{
	static CFMutableArrayRef disconnnectedCallbacks = NULL;

	if (!disconnnectedCallbacks)
	{
		disconnnectedCallbacks = CFArrayCreateMutable(kCFAllocatorDefault, 0, NULL);
	}

	return disconnnectedCallbacks;
}

CFMutableDictionaryRef GERazerDeviceManagerMessageReceivedCallbacks()
{
	static CFMutableDictionaryRef messageReceivedCallbacks = NULL;

	if (!messageReceivedCallbacks)
	{
		messageReceivedCallbacks = CFDictionaryCreateMutable(kCFAllocatorDefault, 0, NULL, NULL);
	}

	return messageReceivedCallbacks;
}

CFMutableArrayRef GERazerDeviceManagerGetMessageReceivedCallbacks(SInt32 messageId)
{
	CFMutableDictionaryRef callbacks = GERazerDeviceManagerMessageReceivedCallbacks();
	return (CFMutableArrayRef) CFDictionaryGetValue(callbacks, (void *) (CFIndex) messageId);
}

void GERazerDeviceManagerPortInvalidated(CFMessagePortRef ms, void *info)
{
	CFRelease(sendPort);
	sendPort = NULL;

	if (receivePort)
	{
		CFMessagePortInvalidate(receivePort);
		CFRunLoopRemoveSource(GERazerReceiveRunLoopGet(), receiveRunLoopSource, kCFRunLoopCommonModes);
		CFRelease(receiveRunLoopSource);
		CFRelease(receivePort);
		receivePort = NULL;
	}

	receiveRunLoop = NULL;

	CFArrayRef callbacks = GERazerDeviceManagerDisconnectedCallbacks();
	CFIndex count = CFArrayGetCount(callbacks);

	for (CFIndex i = 0; i < count; i++)
	{
		GERazerDisconnectedCallback callback = CFArrayGetValueAtIndex(callbacks, i);
		callback();
	}
}

CFIndex GERazerIndexForBlockingMessageId(SInt32 messageId)
{
	for (int i = 0; i < blockingReceiveMessageIdCount; i++)
	{
		if (blockingReceiveMessageIds[i] == messageId)
		{
			return i;
		}
	}

	return kCFNotFound;
}

void GERazerHandleExpectedBlockingMessage(GERazerMessageRef message)
{
	CFArrayAppendValue(blockingReceiveMessages, message);

	CFIndex index = GERazerIndexForBlockingMessageId(GERazerMessageGetId(message));
	blockingReceiveReceivedMessageIdMap[index] = true;

	bool allReceived = true;

	for (int i = 0; i < blockingReceiveMessageIdCount; i++)
	{
		if (!blockingReceiveReceivedMessageIdMap[i])
		{
			allReceived = false;
			break;
		}
	}

	if (allReceived)
	{
		CFRunLoopStop(GERazerReceiveRunLoopGet());
	}
}

CFDataRef GERazerDeviceManagerMessageReceived(CFMessagePortRef local, SInt32 msgid, CFDataRef data, void *info)
{
	bool expectedBlockingMessage = receiveIsBlocking && GERazerIndexForBlockingMessageId(msgid) != kCFNotFound;
	CFMutableArrayRef callbacks = GERazerDeviceManagerGetMessageReceivedCallbacks(msgid);

	if (callbacks || expectedBlockingMessage)
	{

		GERazerMessageRef message = NULL;
		CFDictionaryRef dictionary = CFPropertyListCreateWithData(kCFAllocatorDefault, data, 0, NULL, NULL);

		if (dictionary)
		{
			CFDataRef rzdata = CFDictionaryGetValue(dictionary, CFSTR("rzdata"));

			if (rzdata)
			{
				CFDictionaryRef rzdataDictionary = CFPropertyListCreateWithData(kCFAllocatorDefault, rzdata, 0, NULL, NULL);
				message = GERazerMessageCreateWithDictionary(msgid, rzdataDictionary);
				CFRelease(rzdataDictionary);
			}

			CFRelease(dictionary);
		}

		if (message)
		{
			CFIndex callbackCount = callbacks ? CFArrayGetCount(callbacks) : 0;

			for (int i = 0; i < callbackCount; i++)
			{
				GERazerMessageReceivedCallback callback = CFArrayGetValueAtIndex(callbacks, i);
				callback(message);
			}

			if (expectedBlockingMessage)
			{
				GERazerHandleExpectedBlockingMessage(message);
			}

			GERazerMessageRelease(message);
		}
		else
		{
			printf("Received invalid message with an expected message ID of %d.\n", msgid);
		}
	}

	return NULL;
}

SInt32 GERazerConnect(GERazerDisconnectedCallback callback)
{
	if (callback)
	{
		CFMutableArrayRef callbacks = GERazerDeviceManagerDisconnectedCallbacks();

		if (CFArrayGetFirstIndexOfValue(callbacks, CFRangeMake(0, CFArrayGetCount(callbacks)), callback) == kCFNotFound)
		{
			CFArrayAppendValue(callbacks, callback);
		}
	}

	if (!sendPort)
	{
		sendPort = CFMessagePortCreateRemote(kCFAllocatorDefault, CFSTR("RazerDeviceEngineMsgPort"));

		if (!sendPort)
		{
			return kGERazerConnectionFailed;
		}

		CFMessagePortSetInvalidationCallBack(sendPort, GERazerDeviceManagerPortInvalidated);
	}

	if (!receivePort)
	{
		CFMessagePortContext context;
		context.version = 0;
		context.info = NULL;
		context.retain = NULL;
		context.release = NULL;
		context.copyDescription = NULL;

		Boolean shouldFreeInfo = FALSE;

		receivePort = CFMessagePortCreateLocal(NULL, CFSTR("EmilyConfiguratorReplyPort"), &GERazerDeviceManagerMessageReceived, &context, &shouldFreeInfo);

		if (receivePort)
		{
			receiveRunLoop = CFRunLoopGetCurrent();
			receiveRunLoopSource = CFMessagePortCreateRunLoopSource(NULL, receivePort, 0);
			CFRunLoopAddSource(receiveRunLoop, receiveRunLoopSource, kCFRunLoopCommonModes);
		}
		else
		{
			return kGERazerConnectionSendOnly;
		}
	}

	return kGERazerConnectionSuccess;
}

void GERazerDisconnect()
{
	if (sendPort)
	{
		CFMessagePortInvalidate(sendPort);
	}
}

void GERazerRemoveDisconnectedCallback(GERazerDisconnectedCallback callback)
{
	CFMutableArrayRef callbacks = GERazerDeviceManagerDisconnectedCallbacks();
	CFIndex index = CFArrayGetFirstIndexOfValue(callbacks, CFRangeMake(0, CFArrayGetCount(callbacks)), callback);

	if (index != kCFNotFound)
	{
		CFArrayRemoveValueAtIndex(callbacks, index);
	}
}

SInt32 GERazerSendMessage(GERazerMessageRef message)
{
	static const CFIndex rzcontflagZero = 0;
	static const void *rzkeys[] = { CFSTR("rzcontflag"), CFSTR("rzdata") };

	if (!sendPort)
	{
		GERazerConnect(NULL);
	}

	if (sendPort)
	{
		CFDataRef rzdata = CFPropertyListCreateData(kCFAllocatorDefault, GERazerMessageGetData(message), kCFPropertyListBinaryFormat_v1_0, 0, NULL);

		if (!rzdata)
		{
			return kGERazerTransferSerializationFailed;
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
			return kGERazerTransferSerializationFailed;
		}

		SInt32 result = CFMessagePortSendRequest(sendPort, GERazerMessageGetId(message), data, 60.0, 60.0, NULL, NULL);

		CFRelease(data);

		if (result != kCFMessagePortSuccess)
		{
			if (result == kCFMessagePortSendTimeout)
			{
				return kGERazerTransferTimedOut;
			}
			else
			{
				return kGERazerTransferConnectionInvalid;
			}
		}
	}
	else
	{
		return kGERazerTransferConnectionInvalid;
	}

	return kGERazerTransferSuccess;
}

SInt32 GERazerReceiveMessage(SInt32 messageId, GERazerMessageRef *message, CFTimeInterval receiveTimeout)
{
	SInt32 messageIds = { messageId };
	CFMutableArrayRef messages = CFArrayCreateMutable(kCFAllocatorDefault, 0, &kGERazerMessageArrayCallbacks);

	SInt32 status = GERazerReceiveMessages(&messageIds, 1, messages, receiveTimeout);

	if (CFArrayGetCount(messages) > 0)
	{
		*message = (GERazerMessageRef) CFArrayGetValueAtIndex(messages, 0);
		GERazerMessageRetain(*message);
	}
	else
	{
		*message = NULL;
	}

	CFRelease(messages);

	return status;
}

SInt32 GERazerReceiveMessages(SInt32 *messageIds, CFIndex messageIdCount, CFMutableArrayRef receivedMessages, CFTimeInterval receiveTimeout)
{
	if (CFRunLoopGetCurrent() != GERazerReceiveRunLoopGet())
	{
		return kGERazerTransferWrongRunLoop;
	}

	if (!receivePort)
	{
		return kGERazerTransferConnectionInvalid;
	}

	receiveIsBlocking = true;
	blockingReceiveMessages = receivedMessages;
	blockingReceiveMessageIds = messageIds;
	blockingReceiveMessageIdCount = messageIdCount;
	blockingReceiveReceivedMessageIdMap = CFAllocatorAllocate(kCFAllocatorDefault, messageIdCount * sizeof(bool), 0);

	CFRunLoopRunResult result = CFRunLoopRunInMode(kCFRunLoopDefaultMode, receiveTimeout, false);

	receiveIsBlocking = false;
	CFAllocatorDeallocate(kCFAllocatorDefault, blockingReceiveReceivedMessageIdMap);

	return result == kCFRunLoopRunTimedOut ? kGERazerTransferTimedOut : kGERazerTransferSuccess;
}

void GERazerAddMessageReceivedCallback(SInt32 messageId, GERazerMessageReceivedCallback callback)
{
	CFMutableArrayRef messageCallbacks = GERazerDeviceManagerGetMessageReceivedCallbacks(messageId);

	if (!messageCallbacks)
	{
		messageCallbacks = CFArrayCreateMutable(kCFAllocatorDefault, 0, NULL);
		CFDictionaryAddValue(GERazerDeviceManagerMessageReceivedCallbacks(), (void *) (CFIndex) messageId, messageCallbacks);
	}

	CFArrayAppendValue(messageCallbacks, callback);
}

void GERazerRemoveMessageReceivedCallback(SInt32 messageId, GERazerMessageReceivedCallback callback)
{
	CFMutableArrayRef messageCallbacks = GERazerDeviceManagerGetMessageReceivedCallbacks(messageId);

	if (messageCallbacks)
	{
		CFIndex index = CFArrayGetFirstIndexOfValue(messageCallbacks, CFRangeMake(0, CFArrayGetCount(messageCallbacks)), callback);

		if (index != kCFNotFound)
		{
			CFArrayRemoveValueAtIndex(messageCallbacks, index);
		}
	}
}
