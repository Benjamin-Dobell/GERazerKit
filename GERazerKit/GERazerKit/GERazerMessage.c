#include <libkern/OSAtomic.h>
#include "GERazerMessage.h"
#include "GERazerUtils.h"

struct GERazerMessage
{
	UInt32 _retainCount;
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
			message->_retainCount = 1;
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
	UInt32 rc;
	do {
		rc = message->_retainCount;
	} while (!OSAtomicCompareAndSwap32Barrier(rc, rc + 1, (int32_t *) &message->_retainCount));
}

void GERazerMessageRelease(GERazerMessageRef message)
{
	bool deallocate = false;

	UInt32 rc;
	do {
		rc = message->_retainCount;

		if (rc == 1 && OSAtomicCompareAndSwap32Barrier(1, 0, (int32_t *) &message->_retainCount))
		{
			deallocate = true;
			break;
		}
	} while (!OSAtomicCompareAndSwap32Barrier(rc, rc - 1, (int32_t *) &message->_retainCount));

	if (deallocate)
	{
		CFRelease(message->_data);
		CFAllocatorDeallocate(CFAllocatorGetDefault(), message);
	}
}

SInt32 GERazerMessageGetId(const GERazerMessageRef message)
{
	return message->_id;
}

CFMutableDictionaryRef GERazerMessageGetData(const GERazerMessageRef message)
{
	return message->_data;
}

CFHashCode GERazerMessageHash(GERazerMessageRef message)
{
	return 31 * CFHash(message->_data) + message->_id;
}

bool GERazerMessageEqual(GERazerMessageRef message1, GERazerMessageRef message2)
{
	return message1->_id == message2->_id && CFEqual(message1->_data, message2->_data);
}

const void *GERazerMessageRetainCallback(CFAllocatorRef allocator, const void *value)
{
	GERazerMessageRef message = (GERazerMessageRef) value;
	GERazerMessageRetain(message);
	return message;
}

void GERazerMessageReleaseCallback(CFAllocatorRef allocator, const void *value)
{
	GERazerMessageRef message = (GERazerMessageRef) value;
	GERazerMessageRelease(message);
}

Boolean	GERazerMessageEqualCallback(const void *value1, const void *value2)
{
	return GERazerMessageEqual((GERazerMessageRef) value1, (GERazerMessageRef) value2);
}

CFHashCode GERazerMessageHashCallback(const void *value)
{
	return GERazerMessageHash((GERazerMessageRef) value);
}

CFArrayCallBacks kGERazerMessageArrayCallbacks = {
	.retain = &GERazerMessageRetainCallback,
	.release = &GERazerMessageReleaseCallback,
	.equal = &GERazerMessageEqualCallback,
	.version = 0
};

#if __LLP64__
const CFIndex kGERazerTerminate = LLONG_MIN;
#else
const CFIndex kGERazerTerminate = LONG_MIN;
#endif

CFPropertyListRef GERazerMessageDataArrayGetValue(CFArrayRef array, va_list argp);

CFPropertyListRef GERazerMessageDataDictionaryGetValue(CFDictionaryRef dictionary, va_list argp)
{
	CFStringRef key = va_arg(argp, CFStringRef);

	if ((CFIndex) key == kGERazerTerminate)
	{
		return dictionary;
	}
	else
	{
		CFPropertyListRef value = CFDictionaryGetValue(dictionary, key);

		if (value)
		{
			CFTypeID typeID = CFGetTypeID(value);

			if (typeID == CFDictionaryGetTypeID())
			{
				return GERazerMessageDataDictionaryGetValue(value, argp);
			}
			else if (typeID == CFArrayGetTypeID())
			{
				return GERazerMessageDataArrayGetValue(value, argp);
			}
			else if (va_arg(argp, CFIndex) == kGERazerTerminate)
			{
				return value;
			}
		}
	}

	return NULL;
}

CFPropertyListRef GERazerMessageDataArrayGetValue(CFArrayRef array, va_list argp)
{
	CFIndex index = va_arg(argp, CFIndex);

	if (index == kGERazerTerminate)
	{
		return array;
	}
	else
	{
		if (index < CFArrayGetCount(array))
		{
			CFPropertyListRef value = CFArrayGetValueAtIndex(array, index);

			if (value)
			{
				CFTypeID typeID = CFGetTypeID(value);

				if (typeID == CFDictionaryGetTypeID())
				{
					return GERazerMessageDataDictionaryGetValue(value, argp);
				}
				else if (typeID == CFArrayGetTypeID())
				{
					return GERazerMessageDataArrayGetValue(value, argp);
				}
				else if (va_arg(argp, CFIndex) == kGERazerTerminate)
				{
					return value;
				}
			}
		}
	}

	return NULL;
}

CFPropertyListRef GERazerMessageGetDataValue(GERazerMessageRef message, ...)
{
	CFPropertyListRef result = NULL;

	va_list argp;
	va_start(argp, message);

	result = GERazerMessageDataDictionaryGetValue(GERazerMessageGetData(message), argp);

	va_end(argp);

	return result;
}
