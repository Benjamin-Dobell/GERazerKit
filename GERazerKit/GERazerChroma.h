#if !defined(__GERAZERKIT_GERAZERCHROMA__)
#define __GERAZERKIT_GERAZERCHROMA__ 1

#include <CoreFoundation/CoreFoundation.h>

CF_ENUM(SInt32)
{
	kGERazerEffectIdStatic = 0,
	/* kGERazerEffectIdBlink = 1, */
	kGERazerEffectIdBreathing = 2,
	/* kGERazerEffectIdSoftwareControl = 3, */
	kGERazerEffectIdSpectrumCycling = 4,
	kGERazerEffectIdOff = 5,
	kGERazerEffectIdReactive = 6, // AKA Afterglow
	kGERazerEffectIdCustom = 7,
	kGERazerEffectIdWave = 8,
	kGERazerEffectIdStarlight = 13
};

CF_ENUM(SInt32)
{
	kGERazerReactiveAfterglowDurationShort = 1,
	kGERazerReactiveAfterglowDurationMedium = 2,
	kGERazerReactiveAfterglowDurationLong = 3
};

CF_ENUM(SInt32)
{
	kGERazerWaveDirectionBackToFront = 1,
	kGERazerWaveDirectionFrontToBack = 2
};

CF_ENUM(SInt32)
{
	kGERazerLightingBrightnessDim = 33,
	kGERazerLightingBrightnessNormal = 66,
	kGERazerLightingBrightnessBright = 100
};

extern const CFStringRef kGERazerEffectNameStatic;
extern const CFStringRef kGERazerEffectNameBreathing;
extern const CFStringRef kGERazerEffectNameSpectrumCycling;
extern const CFStringRef kGERazerEffectNameReactive;
extern const CFStringRef kGERazerEffectNameWave;

CFMutableDictionaryRef GERazerColorDictionaryCreate(float r, float g, float b);

CFMutableDictionaryRef GERazerEffectCreateStatic(float r, float g, float b);

CFMutableDictionaryRef GERazerEffectCreateBreathing(float r1, float g1, float b1, float r2, float g2, float b2);

CFMutableDictionaryRef GERazerEffectCreateRandomBreathing(void);

CFMutableDictionaryRef GERazerEffectCreateSpectrumCycling(void);

CFMutableDictionaryRef GERazerEffectCreateReactive(float r, float g, float b, SInt32 afterglowDuration);

CFMutableDictionaryRef GERazerEffectCreateWave(SInt32 direction);

#endif /* ! __GERAZERKIT_GERAZERCHROMA__ */
