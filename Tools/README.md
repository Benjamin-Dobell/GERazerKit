# Development Tools

These scripts/tools will facilitate additional development efforts.

## monitor.sh

Execute this script to monitor communication between the Razer Device Engine and software communicating with it e.g. Razer Synapse.

Communication will be logged to stdout.

## patch.sh

This script must be run before you try monitor communication with `./monitor.sh`. It makes some very simple (string replacement) patches to the Razer Device Engine.

## unpatch.sh

Once patched, the Razer Device Engine will only function when `./monitor.sh` is running. This script will return the Razer Device Engine to its original unpatched state.

## bin/monitor

This is a simple command line app that monitors Razer Device Engine communication. Do not execute it directly, instead use `./monitor.sh`.

The source for this binary is `src/monitor.m`.

