#include "GERazerChroma.h"

const CFStringRef kGERazerEffectNameStatic = CFSTR("Static");
const CFStringRef kGERazerEffectNameBreathing = CFSTR("Breathing");
const CFStringRef kGERazerEffectNameSpectrumCycling = CFSTR("Spectrum Cycling");
const CFStringRef kGERazerEffectNameReactive = CFSTR("Afterglow");
const CFStringRef kGERazerEffectNameWave = CFSTR("Wave");

CFMutableDictionaryRef GERazerColorDictionaryCreate(float r, float g, float b)
{
	CFNumberRef red = CFNumberCreate(kCFAllocatorDefault, kCFNumberFloatType, &r);
	CFNumberRef green = CFNumberCreate(kCFAllocatorDefault, kCFNumberFloatType, &g);
	CFNumberRef blue = CFNumberCreate(kCFAllocatorDefault, kCFNumberFloatType, &b);

	CFMutableDictionaryRef dictionary = CFDictionaryCreateMutable(kCFAllocatorDefault, 0, &kCFTypeDictionaryKeyCallBacks, &kCFTypeDictionaryValueCallBacks);
	CFDictionaryAddValue(dictionary, CFSTR("Red"), red);
	CFDictionaryAddValue(dictionary, CFSTR("Green"), green);
	CFDictionaryAddValue(dictionary, CFSTR("Blue"), blue);

	CFRelease(red);
	CFRelease(green);
	CFRelease(blue);

	return dictionary;
}

CFMutableDictionaryRef GERazerEffectCreateStatic(float r, float g, float b)
{
	static const SInt32 effectId = kGERazerEffectIdStatic;

	CFMutableDictionaryRef colorDictionary = GERazerColorDictionaryCreate(r, g, b);

	CFMutableArrayRef colorsArray = CFArrayCreateMutable(kCFAllocatorDefault, 0, &kCFTypeArrayCallBacks);
	CFArrayAppendValue(colorsArray, colorDictionary);

	CFRelease(colorDictionary);

	CFNumberRef boxedEffectId = CFNumberCreate(kCFAllocatorDefault, kCFNumberSInt32Type, &effectId);

	CFMutableDictionaryRef dictionary = CFDictionaryCreateMutable(kCFAllocatorDefault, 0, &kCFTypeDictionaryKeyCallBacks, &kCFTypeDictionaryValueCallBacks);
	CFDictionaryAddValue(dictionary, CFSTR("Colors"), colorsArray);
	CFDictionaryAddValue(dictionary, CFSTR("Effect"), boxedEffectId);

	CFRelease(colorsArray);
	CFRelease(boxedEffectId);

	return dictionary;
}

CFMutableDictionaryRef GERazerEffectCreateBreathing(float r1, float g1, float b1, float r2, float g2, float b2)
{
	static const SInt32 effectId = kGERazerEffectIdBreathing;

	CFMutableDictionaryRef color1Dictionary = GERazerColorDictionaryCreate(r1, g1, b1);
	CFMutableDictionaryRef color2Dictionary = GERazerColorDictionaryCreate(r2, g2, b2);

	CFMutableArrayRef colorsArray = CFArrayCreateMutable(kCFAllocatorDefault, 0, &kCFTypeArrayCallBacks);
	CFArrayAppendValue(colorsArray, color1Dictionary);
	CFArrayAppendValue(colorsArray, color2Dictionary);

	CFRelease(color1Dictionary);
	CFRelease(color2Dictionary);

	CFNumberRef boxedEffectId = CFNumberCreate(kCFAllocatorDefault, kCFNumberSInt32Type, &effectId);

	CFMutableDictionaryRef dictionary = CFDictionaryCreateMutable(kCFAllocatorDefault, 0, &kCFTypeDictionaryKeyCallBacks, &kCFTypeDictionaryValueCallBacks);
	CFDictionaryAddValue(dictionary, CFSTR("Colors"), colorsArray);
	CFDictionaryAddValue(dictionary, CFSTR("Effect"), boxedEffectId);

	CFRelease(colorsArray);
	CFRelease(boxedEffectId);

	return dictionary;
}

CFMutableDictionaryRef GERazerEffectCreateRandomBreathing(void)
{
	static const SInt32 effectId = kGERazerEffectIdBreathing;
	static const char random = 1;

	CFNumberRef boxedRandom = CFNumberCreate(kCFAllocatorDefault, kCFNumberCharType, &random);
	CFNumberRef boxedEffectId = CFNumberCreate(kCFAllocatorDefault, kCFNumberSInt32Type, &effectId);

	CFMutableDictionaryRef dictionary = CFDictionaryCreateMutable(kCFAllocatorDefault, 0, &kCFTypeDictionaryKeyCallBacks, &kCFTypeDictionaryValueCallBacks);
	CFDictionaryAddValue(dictionary, CFSTR("IsRandomColor"), boxedRandom);
	CFDictionaryAddValue(dictionary, CFSTR("Effect"), boxedEffectId);

	CFRelease(boxedRandom);
	CFRelease(boxedEffectId);

	return dictionary;
}

CFMutableDictionaryRef GERazerEffectCreateSpectrumCycling(void)
{
	static const SInt32 effectId = kGERazerEffectIdSpectrumCycling;

	CFNumberRef boxedEffectId = CFNumberCreate(kCFAllocatorDefault, kCFNumberSInt32Type, &effectId);

	CFMutableDictionaryRef dictionary = CFDictionaryCreateMutable(kCFAllocatorDefault, 0, &kCFTypeDictionaryKeyCallBacks, &kCFTypeDictionaryValueCallBacks);
	CFDictionaryAddValue(dictionary, CFSTR("Effect"), boxedEffectId);

	CFRelease(boxedEffectId);

	return dictionary;
}

CFMutableDictionaryRef GERazerEffectCreateReactive(float r, float g, float b, SInt32 afterglowDuration)
{
	static const SInt32 effectId = kGERazerEffectIdReactive;

	CFMutableDictionaryRef colorDictionary = GERazerColorDictionaryCreate(r, g, b);

	CFMutableArrayRef colorsArray = CFArrayCreateMutable(kCFAllocatorDefault, 0, &kCFTypeArrayCallBacks);
	CFArrayAppendValue(colorsArray, colorDictionary);

	CFRelease(colorDictionary);

	CFNumberRef boxedDuration = CFNumberCreate(kCFAllocatorDefault, kCFNumberSInt32Type, &afterglowDuration);
	CFNumberRef boxedEffectId = CFNumberCreate(kCFAllocatorDefault, kCFNumberSInt32Type, &effectId);

	CFMutableDictionaryRef dictionary = CFDictionaryCreateMutable(kCFAllocatorDefault, 0, &kCFTypeDictionaryKeyCallBacks, &kCFTypeDictionaryValueCallBacks);
	CFDictionaryAddValue(dictionary, CFSTR("Colors"), colorsArray);
	CFDictionaryAddValue(dictionary, CFSTR("Duration"), boxedDuration);
	CFDictionaryAddValue(dictionary, CFSTR("Effect"), boxedEffectId);

	CFRelease(colorsArray);
	CFRelease(boxedDuration);
	CFRelease(boxedEffectId);

	return dictionary;
}

CFMutableDictionaryRef GERazerEffectCreateWave(SInt32 direction)
{
	static const SInt32 effectId = kGERazerEffectIdWave;

	CFNumberRef boxedDirection = CFNumberCreate(kCFAllocatorDefault, kCFNumberSInt32Type, &direction);
	CFNumberRef boxedEffectId = CFNumberCreate(kCFAllocatorDefault, kCFNumberSInt32Type, &effectId);

	CFMutableDictionaryRef dictionary = CFDictionaryCreateMutable(kCFAllocatorDefault, 0, &kCFTypeDictionaryKeyCallBacks, &kCFTypeDictionaryValueCallBacks);
	CFDictionaryAddValue(dictionary, CFSTR("Direction"), boxedDirection);
	CFDictionaryAddValue(dictionary, CFSTR("Effect"), boxedEffectId);

	CFRelease(boxedDirection);
	CFRelease(boxedEffectId);

	return dictionary;
}
