#!/bin/sh
BASEDIR=$(dirname $0)

RZ_DEVICE_ENGINE="/Library/Application Support/Razer/RzDeviceEngine.app/Contents/MacOS/RzDeviceEngine"
ORIG_PORT=RazerDeviceEngineMsgPort
NEW_PORT=Razer_InterceptedMsgPort
ORIG_RESPONSE_PORT=EmilyConfiguratorReplyPort
NEW_RESPONSE_PORT=Emily_InterceptedReplyPort
TEMP_FILE=/tmp/intercept_razer_port

grep=grep

if [ -n "$($grep --version | grep '2.5.1')" ]; then
	grep=ggrep
	if [ -z "$($grep --version 2>/dev/null)" ]; then
		echo "Your system is running grep 2.5.1, which contains a critical bug."
		echo "Please install a newer grep from Homebrew."
	fi
fi

port_offset=$($grep --byte-offset --only-matching -a -e $ORIG_PORT "$RZ_DEVICE_ENGINE" | cut -d ':' -f 1)
response_port_offset=$($grep --byte-offset --only-matching -a -e $ORIG_RESPONSE_PORT "$RZ_DEVICE_ENGINE" | cut -d ':' -f 1)

if [  -n "$port_offset" ] || [ -n "$response_port_offset" ]; then
	echo $NEW_PORT > "$TEMP_FILE"
	echo "$port_offset" | xargs -I % dd if="$TEMP_FILE" of="$RZ_DEVICE_ENGINE" obs=1 seek=% conv=notrunc

	echo $NEW_RESPONSE_PORT > "$TEMP_FILE"
	echo "$response_port_offset" | xargs -I % dd if="$TEMP_FILE" of="$RZ_DEVICE_ENGINE" obs=1 seek=% conv=notrunc

	echo ""
	echo "RzDeviceEngine successfully patched."
	rm "$TEMP_FILE"

	echo ""
	echo "When you're done you can unpatch RzDeviceEngine with:"
	echo "	$BASEDIR/unpatch.sh"
	echo ""
	echo "You should now run:"
	echo "	$BASEDIR/monitor.sh"
else
	echo "No patch regions identified in RzDeviceEngine."
	echo "The binary is probably already patched."
fi

