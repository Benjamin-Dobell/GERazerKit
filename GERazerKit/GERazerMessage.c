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
		message->_data = GERazerDictionaryCreateMutableDeepCopy(dictionary);

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

CFPropertyListRef GERazerMessageDataArrayVGetValue(CFArrayRef array, va_list argp);

CFPropertyListRef GERazerMessageDataDictionaryVGetValue(CFDictionaryRef dictionary, va_list argp)
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
				return GERazerMessageDataDictionaryVGetValue(value, argp);
			}
			else if (typeID == CFArrayGetTypeID())
			{
				return GERazerMessageDataArrayVGetValue(value, argp);
			}
			else if (va_arg(argp, CFIndex) == kGERazerTerminate)
			{
				return value;
			}
		}
	}

	return NULL;
}

CFPropertyListRef GERazerMessageDataArrayVGetValue(CFArrayRef array, va_list argp)
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
					return GERazerMessageDataDictionaryVGetValue(value, argp);
				}
				else if (typeID == CFArrayGetTypeID())
				{
					return GERazerMessageDataArrayVGetValue(value, argp);
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

CFPropertyListRef GERazerMessageDataDictionaryGetValue(CFDictionaryRef dictionary, ...)
{
	CFPropertyListRef result = NULL;

	va_list argp;
	va_start(argp, dictionary);

	result = GERazerMessageDataDictionaryVGetValue(dictionary, argp);

	va_end(argp);

	return result;
}

CFPropertyListRef GERazerMessageDataArrayGetValue(CFArrayRef array, ...)
{
	CFPropertyListRef result = NULL;

	va_list argp;
	va_start(argp, array);

	result = GERazerMessageDataArrayVGetValue(array, argp);

	va_end(argp);

	return result;
}

CFPropertyListRef GERazerMessageDataGetValue(GERazerMessageRef message, ...)
{
	CFPropertyListRef result = NULL;

	va_list argp;
	va_start(argp, message);

	result = GERazerMessageDataDictionaryVGetValue(GERazerMessageGetData(message), argp);

	va_end(argp);

	return result;
}
