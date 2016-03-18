#if !defined(__GERAZERKIT_GERAZERMESSAGE__)
#define __GERAZERKIT_GERAZERMESSAGE__ 1

#include <CoreFoundation/CoreFoundation.h>

struct GERazerMessage;
typedef struct GERazerMessage *GERazerMessageRef;

GERazerMessageRef GERazerMessageCreate(SInt32 messageId);

GERazerMessageRef GERazerMessageCreateWithDictionary(SInt32 messageId, CFDictionaryRef dictionary);

GERazerMessageRef GERazerMessageCreateCopy(GERazerMessageRef source);

void GERazerMessageRetain(GERazerMessageRef message);

void GERazerMessageRelease(GERazerMessageRef message);

SInt32 GERazerMessageGetId(const GERazerMessageRef message);

//! Keys must be CFString, and values property list (plist) compatible CFType.
CFMutableDictionaryRef GERazerMessageGetData(const GERazerMessageRef message);

CFHashCode GERazerMessageHash(GERazerMessageRef message);

bool GERazerMessageEqual(GERazerMessageRef message1, GERazerMessageRef message2);

// Core foundation data structure callbacks

const void *GERazerMessageRetainCallback(CFAllocatorRef allocator, const void *value);
void GERazerMessageReleaseCallback(CFAllocatorRef allocator, const void *value);
Boolean	GERazerMessageEqualCallback(const void *value1, const void *value2);
CFHashCode GERazerMessageHashCallback(const void *value);

extern CFArrayCallBacks kGERazerMessageArrayCallbacks;

extern const CFIndex kGERazerTerminate;

//! The first varg must be CFStringRef with all subsequent vargs being a CFStringRef or CFIndex and
//! you *must* terminate the varg list with a kGERazerTerminate
//! e.g.
//! GERazerMessageDataDictionaryGetValue(profile, CFSTR("AllDevSettings"), CFSTR("Profiles"), 0, kGERazerTerminate);
CFPropertyListRef GERazerMessageDataDictionaryGetValue(CFDictionaryRef dictionary, ...);

//! The first varg must be CFIndex with all subsequent vargs being a CFStringRef or CFIndex and you
//! *must* terminate the varg list with a kGERazerTerminate
//! e.g.
//! GERazerMessageDataArrayGetValue(profiles, 0, CFSTR("IsDeviceWired"), kGERazerTerminate);
CFPropertyListRef GERazerMessageDataArrayGetValue(CFArrayRef array, ...);

//! vargs must be CFStringRef or CFIndex (for looking into nested arrays) and you *must* terminate
//! the varg list with a kGERazerTerminate
//! e.g.
//! GERazerMessageDataGetValue(message, CFSTR("AllDevSettings"), CFSTR("Profiles"), 0, kGERazerTerminate);
CFPropertyListRef GERazerMessageDataGetValue(GERazerMessageRef message, ...);

#endif /* ! __GERAZERKIT_GERAZERMESSAGE__ */
