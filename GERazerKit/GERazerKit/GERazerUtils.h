#if !defined(__GERAZERKIT_GERAZERUTILS__)
#define __GERAZERKIT_GERAZERUTILS__ 1

#include <CoreFoundation/CoreFoundation.h>

CFStringRef GERazerStringCreateFromInt(SInt32 value);

//! Recursively merges @dictionary2's values into @mutableDictionary.
//!
//! IMPORTANT: All values (recursively) within @mutableDictionary must be mutable.
//!
//! @dictionary2 will have its key-value pairs "set" on @mutableDictionary with the following two
//! exceptions:
//!
//!  1. If @mutableDictionary and @dictionary2 both contain a CFDictionaryRef value for the same
//!     key, then the final value will be the result of recursively calling this method with
//!     @mutableDictionary's value passed as @mutableDictionary and @dictionary2's value passed as
//!     @dictionary2.
//!
//!  2. If @mutableDictionary and @dictionary2 both contain a CFArrayRef value for the same key,
//!     then the final value will be the result of appending a deep copy of @dictionary2's array to
//!     @mutableDictionary's array.
//!
//! Deep copies will be made with CFPropertyListCreateDeepCopy(), meaning @mutableDictionary and
//! @dictionary2 must both be valid property lists. The copy will be performed with the option
//! kCFPropertyListMutableContainersAndLeaves.
void GERazerDictionaryRecursivelyMergeDictionary(CFMutableDictionaryRef mutableDictionary, CFDictionaryRef dictionary2);

// A convenience method for the common case
static inline CFMutableDictionaryRef GERazerDictionaryCreateMutableDeepCopy(CFDictionaryRef dictionary)
{
	return (CFMutableDictionaryRef) CFPropertyListCreateDeepCopy(kCFAllocatorDefault, dictionary, kCFPropertyListMutableContainersAndLeaves);
}

// A convenience method for the common case
static inline CFMutableArrayRef GERazerArrayCreateMutableDeepCopy(CFArrayRef array)
{
	return (CFMutableArrayRef) CFPropertyListCreateDeepCopy(kCFAllocatorDefault, array, kCFPropertyListMutableContainersAndLeaves);
}

// Macros to facilitate working with temporary owned objects.
//
// These are macros instead of functions so that static analysis sees the CFRelease().
//
// NOTE: It is poor form to use these macros to release a reference stored in a variable!

#define GERazerDictionarySetThenReleaseValue(DICTIONARY, KEY, VALUE) \
{ \
	CFTypeRef temp = (CFTypeRef) (VALUE); \
	CFDictionarySetValue(DICTIONARY, KEY, temp); \
	CFRelease(temp); \
}

#define GERazerArrayAppendThenReleaseValue(ARRAY, VALUE) \
{ \
	CFTypeRef temp = (CFTypeRef) (VALUE); \
	CFArrayAppendValue(ARRAY, VALUE); \
	CFRelease(VALUE); \
}

#define GERazerDictionaryRecursivelyMergeThenReleaseDictionary(MUTABLE_DICTIONARY, DICTIONARY2) \
{ \
	CFDictionaryRef temp = (DICTIONARY2); \
	GERazerDictionaryRecursivelyMergeDictionary(MUTABLE_DICTIONARY, temp); \
	CFRelease(temp); \
}

#endif /* ! __GERAZERKIT_GERAZERUTILS__ */
