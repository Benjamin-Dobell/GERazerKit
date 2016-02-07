#include "GERazerMessage.h"

struct GERazerMessage
{
	SInt32 _id;
	CFMutableDictionaryRef _data;
};

GERazerMessageRef GERazerMessageCreate(SInt32 messageId)
{
	GERazerMessageRef message = CFAllocatorAllocate(CFAllocatorGetDefault(), sizeof(struct GERazerMessage), 0);

	if (message)
	{
		message->_data = CFDictionaryCreateMutable(kCFAllocatorDefault, 0, &kCFCopyStringDictionaryKeyCallBacks, &kCFTypeDictionaryValueCallBacks);

		if (message->_data)
		{
			message->_id = messageId;
		}
		else
		{
			CFAllocatorDeallocate(CFAllocatorGetDefault(), message);
			message = NULL;
		}
	}

	return message;
}

GERazerMessageRef GERazerMessageCreateWithDictionary(SInt32 messageId, CFDictionaryRef dictionary)
{
	GERazerMessageRef message = CFAllocatorAllocate(CFAllocatorGetDefault(), sizeof(struct GERazerMessage), 0);

	if (message)
	{
		message->_data = (CFMutableDictionaryRef)CFPropertyListCreateDeepCopy(kCFAllocatorDefault, dictionary, kCFPropertyListMutableContainersAndLeaves);

		if (message->_data)
		{
			message->_id = messageId;
		}
		else
		{
			CFAllocatorDeallocate(CFAllocatorGetDefault(), message);
			message = NULL;
		}
	}

	return message;
}

GERazerMessageRef GERazerMessageCreateCopy(GERazerMessageRef source)
{
	return GERazerMessageCreateWithDictionary(source->_id, source->_data);
}

void GERazerMessageRetain(GERazerMessageRef message)
{
	CFRetain(message->_data);
}

void GERazerMessageRelease(GERazerMessageRef message)
{
	bool deallocate = CFGetRetainCount(message->_data) == 1;

	CFRelease(message->_data);

	if (deallocate)
	{
		CFAllocatorDeallocate(CFAllocatorGetDefault(), message);
	}
}

SInt32 GERazerMessageGetId(GERazerMessageRef message)
{
	return message->_id;
}

CFMutableDictionaryRef GERazerMessageGetData(GERazerMessageRef message)
{
	return message->_data;
}
