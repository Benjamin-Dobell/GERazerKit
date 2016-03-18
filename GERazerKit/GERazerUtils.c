#include "GERazerUtils.h"

CFStringRef GERazerStringCreateFromInt(SInt32 value)
{
	return CFStringCreateWithFormat(kCFAllocatorDefault, NULL, CFSTR("%d"), value);
}

void GERazerDictionaryRecursivelyMergeDictionary(CFMutableDictionaryRef mutableDictionary, CFDictionaryRef dictionary2)
{
	CFTypeID dictionaryTypeId = CFDictionaryGetTypeID();
	CFTypeID arrayTypeId = CFArrayGetTypeID();

	CFIndex dictionary2Count = CFDictionaryGetCount(dictionary2);
	const void **dictionary2Keys = CFAllocatorAllocate(kCFAllocatorDefault, sizeof(void *) * dictionary2Count, 0);
	const void **dictionary2Values = CFAllocatorAllocate(kCFAllocatorDefault, sizeof(void *) * dictionary2Count, 0);

	CFDictionaryGetKeysAndValues(dictionary2, dictionary2Keys, dictionary2Values);

	for (CFIndex i = 0; i < dictionary2Count; i++)
	{
		const void *key = dictionary2Keys[i];

		const void *value1 = CFDictionaryGetValue(mutableDictionary, key);
		const void *value2 = dictionary2Values[i];

		CFTypeID value1TypeId = value1 ? CFGetTypeID(value1) : CFNullGetTypeID();
		bool typeIdsMatch = value1TypeId == CFGetTypeID(value2);

		if (typeIdsMatch && value1TypeId == dictionaryTypeId)
		{
			GERazerDictionaryRecursivelyMergeDictionary((CFMutableDictionaryRef) value1, value2);
		}
		else if (typeIdsMatch && value1TypeId == arrayTypeId)
		{
			CFArrayRef copiedValue2Array = GERazerArrayCreateMutableDeepCopy(value2);
			CFArrayAppendArray((CFMutableArrayRef) value1, copiedValue2Array, CFRangeMake(0, CFArrayGetCount(copiedValue2Array)));
			CFRelease(copiedValue2Array);
		}
		else
		{
			CFPropertyListRef copiedValue2 = CFPropertyListCreateDeepCopy(kCFAllocatorDefault, value2, kCFPropertyListMutableContainersAndLeaves);
			CFDictionarySetValue(mutableDictionary, key, copiedValue2);
			CFRelease(copiedValue2);
		}
	}

	CFAllocatorDeallocate(kCFAllocatorDefault, dictionary2Keys);
	CFAllocatorDeallocate(kCFAllocatorDefault, dictionary2Values);
}
