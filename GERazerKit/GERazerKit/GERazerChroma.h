#if !defined(__GERAZERKIT_GERAZERCHROMA__)
#define __GERAZERKIT_GERAZERCHROMA__ 1

#include <CoreFoundation/CoreFoundation.h>

CF_ENUM(SInt32)
{
	kGERazerEffectIdStatic = 0,
	kGERazerEffectIdBreathing = 2,
	kGERazerEffectIdSpectrumCycling = 4,
	kGERazerEffectIdReactive = 6, // AKA Afterglow
	kGERazerEffectIdCustom = 7,
	kGERazerEffectIdWave = 8,
	kGERazerEffectIdStarlight = 13
};

#endif /* ! __GERAZERKIT_GERAZERCHROMA__ */
