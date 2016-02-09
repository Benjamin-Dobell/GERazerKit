#if !defined(__GERAZERKIT_GERAZERIPC__)
#define __GERAZERKIT_GERAZERIPC__ 1

#include <CoreFoundation/CoreFoundation.h>
#include <GERazerKit/GERazerMessage.h>

CF_ENUM(SInt32)
{
	kGERazerConnectionSuccess = 0,
	kGERazerConnectionFailed = 1, // The Razer Device Manager is presumably not running
	kGERazerConnectionSendOnly = 2 // We can only send and not receive because Razer Synapse is running.
};

CF_ENUM(SInt32)
{
	kGERazerTransferSuccess = 0,
	kGERazerTransferConnectionInvalid = 1,
	kGERazerTransferSerializationFailed = 2, // Send only
	kGERazerTransferTimedOut = 3,
	kGERazerTransferWrongRunLoop = 4 // Blocking receive only
};

//! The run loop for the thread on which GERazerConnect() first returned kGERazerConnectionSuccess.
CFRunLoopRef GERazerReceiveRunLoopGet(void);

typedef void (*GERazerDisconnectedCallback)();

//! GERazerSendMessage() will automatically connect as necessary, however GERazerConnect()
//! can be called (one or more times) to register a callback if a disconnection occurs.
SInt32 GERazerConnect(GERazerDisconnectedCallback callback);

void GERazerDisconnect();

void GERazerRemoveDisconnectedCallback(GERazerDisconnectedCallback callback);

SInt32 GERazerSendMessage(GERazerMessageRef message);

//! Can only be called on the run loop returned from GERazerReceiveRunLoopGet().
//! The caller has ownership of @message.
SInt32 GERazerReceiveMessage(SInt32 messageId, GERazerMessageRef *message, CFTimeInterval receiveTimeout);

//! Can only be called on the run loop returned from GERazerReceiveRunLoopGet().
//! Will timeout unless one (or more) messages are received for each messageId.
SInt32 GERazerReceiveMessages(SInt32 *messageIds, CFIndex messageIdCount, CFMutableArrayRef receivedMessages, CFTimeInterval receiveTimeout);

typedef void (*GERazerMessageReceivedCallback)(GERazerMessageRef message);

void GERazerAddMessageReceivedCallback(SInt32 messageId, GERazerMessageReceivedCallback callback);

void GERazerRemoveMessageReceivedCallback(SInt32 messageId, GERazerMessageReceivedCallback callback);

#endif /* ! __GERAZERKIT_GERAZERIPC__ */
