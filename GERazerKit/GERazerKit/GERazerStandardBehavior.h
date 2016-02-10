#if !defined(__GERAZERKIT_GERAZERSTANDARDBEHAVIOR__)
#define __GERAZERKIT_GERAZERSTANDARDBEHAVIOR__ 1

#include <CoreFoundation/CoreFoundation.h>

CFArrayRef GERazerCopyAttachedDeviceIds(void);

CFStringRef GERazerCopyActiveProfileId(SInt32 productId);

SInt32 GERazerGetLedFollowingProductId(SInt32 productId);

#endif /* ! __GERAZERKIT_GERAZERSTANDARDBEHAVIOR__ */
