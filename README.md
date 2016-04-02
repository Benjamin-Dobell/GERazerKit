# GERazerKit

GERazerKit is an unofficial SDK to configure Razer hardware on OS X.

This SDK was initially developed to enable Razer Chroma devices to be used as a build light. 

## Supported Functionality

GERazerKit basically provides a C API for configuring any options that are accessible through Razer Synapse on OS X.

GERazerKit does support Chroma lighting configuration, but unfortunately this SDK does *not* presently provide equivalent functionality to the Razer Chroma SDK available for Windows.

## Supported Devices

Theoretically GERazerKit should support all devices that Razer Synapse supports.

However, at present GERazerKit only provides named constants for the Mamba (Wireless/Chroma) mouse, and various helper methods may only be applicable to Chroma mice (rather than keyboards). 

## Requirements

This SDK communicates with the official Razer Device Manager, rather than communicating directly with devices. This means custom drivers are *not* required. However, the official Razer Synapse package (from Razer) should be installed.

## Razer Synapse Compatibility

Both Razer Synapse and GERazerKit work with "profiles". Profiles created in Razer Synapse can be modified by GERazerKit and vice versa.

Unfortunately, due to the way the Razer Device Manager communicates, only one application at a time can be connected to the Razer Device Manager. As such GERazerKit cannot operate fully when Razer Synapse UI is open.

However, multiple applications developed with GERazerKit can run at the same time, as long as each application treats the Razer Device Manager as a shared resource i.e. only connect to the Razer Device Manager when communication is required, then disconnect immediately afterwards.

## Getting Started

We're a bit light on documentation at the moment, however there is a demo project (for the Mamba Chroma mouse) included which shows of all the basics. There is also some documentation included in various header files.

