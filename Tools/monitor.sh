#!/bin/sh
BASEDIR=$(dirname $0)

RZ_DEVICE_ENGINE="/Library/Application Support/Razer/RzDeviceEngine.app/Contents/MacOS/RzDeviceEngine"
PATCHED_PORT=Razer_InterceptedMsgPort
PATCHED_RESPONSE_PORT=Emily_InterceptedReplyPort

grep=grep

if [ -n "$($grep --version | grep '2.5.1')" ]; then
        grep=ggrep
        if [ -z "$($grep --version 2>/dev/null)" ]; then
                echo "Your system is running grep 2.5.1, which contains a critical bug."
                echo "Please install a newer grep from Homebrew."
        fi
fi

port_offset=$($grep --byte-offset --only-matching -a -e $PATCHED_PORT "$RZ_DEVICE_ENGINE" | cut -d ':' -f 1)
response_port_offset=$($grep --byte-offset --only-matching -a -e $PATCHED_RESPONSE_PORT "$RZ_DEVICE_ENGINE" | cut -d ':' -f 1)

if [  -n "$port_offset" ] || [ -n "$response_port_offset" ]; then
        echo "Killing RzDeviceEngine. It will restart automatically."
        pid=$(ps aux | grep -m 1 '[R]zDeviceEngine' | tr -s ' ' | cut -d ' ' -f 2)
        kill -9 $pid 2>/dev/null; "$BASEDIR/bin/monitor"
else
        echo "The RzDeviceEngine must first be patched. Run:"
	echo "	$BASEDIR/patch.sh"
fi

