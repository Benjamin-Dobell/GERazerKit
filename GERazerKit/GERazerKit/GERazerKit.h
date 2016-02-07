//
//  GERazerKit.h
//  GERazerKit
//
//  Created by Benjamin Dobell on 7/02/2016.
//  Copyright © 2016 Glass Echidna Pty Ltd. All rights reserved.
//

#if !defined(__GERAZERKIT_GERAZERKIT__)
#define __GERAZERKIT_GERAZERKIT__ 1

#include <GERazerKit/GERazerChroma.h>
#include <GERazerKit/GERazerDeviceManager.h>
#include <GERazerKit/GERazerStandardMessages.h>
#include <GERazerKit/GERazerStandardBehavior.h>

//! Project version number for GERazerKit.
extern double GERazerKitVersionNumber;

//! Project version string for GERazerKit.
extern const unsigned char GERazerKitVersionString[];

//! Should only be called before using other GERazer APIs, or not at all. By default the "main" run loop is used.
void GERazerRunLoopSet(CFRunLoopRef runLoop);

CFRunLoopRef GERazerRunLoopGet(void);

#endif /* ! __GERAZERKIT_GERAZERKIT__ */
