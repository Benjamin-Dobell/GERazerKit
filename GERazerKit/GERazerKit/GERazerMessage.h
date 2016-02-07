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

SInt32 GERazerMessageGetId(GERazerMessageRef message);

//! Keys must be CFString, and values property list (plist) compatible CFType.
CFMutableDictionaryRef GERazerMessageGetData(GERazerMessageRef message);

#endif /* ! __GERAZERKIT_GERAZERMESSAGE__ */
