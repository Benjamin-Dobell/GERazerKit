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

typedef void (*GERazerDisconnectedCallback)(void);

//! GERazerSendMessage() will automatically connect as necessary, however GERazerConnect()
//! can be called (one or more times) to register a callback if a disconnection occurs.
SInt32 GERazerConnect(GERazerDisconnectedCallback callback);

void GERazerDisconnect(void);

void GERazerRemoveDisconnectedCallback(GERazerDisconnectedCallback callback);

//! Warning: Sending multiple messages with the same message ID in quick succession (< 1 seconds
//! between messages) seems to result in the Razer Device Manager properly processing the most
//! recent message only. It is suggested you only send one message with the data of each message
//! merged using GERazerDictionaryRecursivelyMergeDictionary();
SInt32 GERazerSendMessage(GERazerMessageRef message);

//! Can only be called on the run loop returned from GERazerReceiveRunLoopGet().
//! @message will point to a copy of the received message (the caller has ownership),
//! or NULL if no message is received.
SInt32 GERazerReceiveMessage(SInt32 messageId, GERazerMessageRef *message, CFTimeInterval receiveTimeout);

//! Important: Can only be called on the run loop returned from GERazerReceiveRunLoopGet().
//!
//! Received messages (if any) will be appended to @receivedMessages.
//!
//! This method will return when one or more messages for each message ID in @messageIds has been
//! received.
//!
//! If a message for each message ID is not received in @receiveTimeout seconds then the method
//! will timeout. Messages that were received before timing out will still be available in
//! @receivedMessages.
SInt32 GERazerReceiveMessages(SInt32 *messageIds, CFIndex messageIdCount, CFMutableArrayRef receivedMessages, CFTimeInterval receiveTimeout);

typedef void (*GERazerMessageReceivedCallback)(GERazerMessageRef message);

void GERazerAddMessageReceivedCallback(SInt32 messageId, GERazerMessageReceivedCallback callback);

void GERazerRemoveMessageReceivedCallback(SInt32 messageId, GERazerMessageReceivedCallback callback);

#endif /* ! __GERAZERKIT_GERAZERIPC__ */
