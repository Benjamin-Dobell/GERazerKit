#import <Foundation/Foundation.h>

CFMessagePortRef inboundPort = NULL;
CFMessagePortRef outboundPort = NULL;

CFMessagePortRef inboundResponsePort = NULL;

// Note: We only support one, we should probably have multiple response ports of our own and maintain a mapping.
NSString *outboundResponsePortName = nil;
CFMessagePortRef outboundResponsePort = NULL;

NSString *const ResponsePortName = @"Emily_InterceptedReplyPort";

void outboundResponsePortInvalidated(CFMessagePortRef ms, void *info)
{
  CFRelease(outboundResponsePort);
  outboundResponsePort = NULL;
  outboundResponsePortName = nil;

  NSLog(@"Response port connection lost.");
}

void connectResponsePort(NSString *name)
{
  if (![name isEqualTo:outboundResponsePortName])
  {
    if (outboundResponsePort)
    {
      CFMessagePortInvalidate(outboundResponsePort);
      CFRelease(outboundResponsePort);
      outboundResponsePort = NULL;
      outboundResponsePortName = nil;
    }

    outboundResponsePort = CFMessagePortCreateRemote(NULL, (__bridge CFStringRef)name);

    if (outboundResponsePort)
    {
      CFMessagePortSetInvalidationCallBack(outboundResponsePort, &outboundResponsePortInvalidated);
      outboundResponsePortName = name;

      NSLog(@"Connected to response port %@", name);

    }
  }
}

void logMessage(SInt32 msgid, NSData *data, BOOL response)
{
  if (response)
  {
    NSLog(@"<<<<<<<<<< Outbound Response Message (msgid: %d) <<<<<<<<<<", msgid);
  }
  else
  {
    NSLog(@">>>>>>>>>> Inbound Message (msgid: %d) >>>>>>>>>>", msgid);
  }

  NSDictionary *dictionary = [NSPropertyListSerialization propertyListWithData:data options:0 format:NULL error:nil];
  printf("\n%s\n\n", [[dictionary description] UTF8String]);

  NSData *rzData = dictionary[@"rzdata"];

  NSDictionary *rzDictionary = [NSPropertyListSerialization propertyListWithData:rzData options:0 format:NULL error:nil];
  printf("rzdata = %s\n\n", [[rzDictionary description] UTF8String]);
}

NSData *tweakedData(NSData *data)
{
  NSDictionary *dictionary = [NSPropertyListSerialization propertyListWithData:data options:0 format:NULL error:nil];
  NSDictionary *rzDictionary = [NSPropertyListSerialization propertyListWithData:dictionary[@"rzdata"] options:0 format:NULL error:nil];

  if (rzDictionary[@"ResponseMsgPortName"])
  {
    NSMutableDictionary *tweakedRzDictionary = (__bridge_transfer NSMutableDictionary *)CFPropertyListCreateDeepCopy(kCFAllocatorDefault, (__bridge CFDictionaryRef)rzDictionary, kCFPropertyListMutableContainers);

    NSMutableDictionary *tweakedDictionary = [dictionary mutableCopy];
    tweakedDictionary[@"rzdata"] = [NSPropertyListSerialization dataWithPropertyList:tweakedRzDictionary format:NSPropertyListBinaryFormat_v1_0 options:0 error:NULL];

    dictionary = tweakedDictionary;
  }

  return [NSPropertyListSerialization dataWithPropertyList:dictionary format:NSPropertyListBinaryFormat_v1_0 options:0 error:NULL];;
}

void forwardMessage(SInt32 msgid, CFDataRef data)
{
  SInt32 result = CFMessagePortSendRequest(outboundPort, msgid, (__bridge CFDataRef)tweakedData((__bridge NSData *)data), 60.0, 60.0, NULL, NULL);

  if (result != kCFMessagePortSuccess)
  {
    NSLog(@"ERROR: Failed to forward message.");
  }
}

void forwardResponseMessage(SInt32 msgid, CFDataRef data)
{
  connectResponsePort(@"EmilyConfiguratorReplyPort");
  
  if (outboundResponsePort)
  {
    SInt32 result = CFMessagePortSendRequest(outboundResponsePort, msgid, data, 60.0, 60.0, NULL, NULL);

    if (result != kCFMessagePortSuccess)
    {
      NSLog(@"ERROR: Failed to forward message.");
    }
  }
  else
  {
    NSLog(@"Could not forward response. No response port open.");
  }
}

CFDataRef inboundCallback(CFMessagePortRef local, SInt32 msgid, CFDataRef data, void *info)
{
  logMessage(msgid, (__bridge NSData *)data, NO);
  forwardMessage(msgid, data);
  return NULL;
}

CFDataRef responseCallback(CFMessagePortRef local, SInt32 msgid, CFDataRef data, void *info)
{
  logMessage(msgid, (__bridge NSData *)data, YES);
  forwardResponseMessage(msgid, data);
  return NULL;
}

BOOL listen()
{
  CFMessagePortContext context;
  context.version = 0;
  context.info = NULL;
  context.retain = NULL;
  context.release = NULL;
  context.copyDescription = NULL;

  Boolean shouldFreeInfo = NO;

  inboundPort = CFMessagePortCreateLocal(NULL, (__bridge CFStringRef)@"RazerDeviceEngineMsgPort", &inboundCallback, &context, &shouldFreeInfo);

  if (inboundPort)
  {
    CFRunLoopSourceRef runLoopSource = CFMessagePortCreateRunLoopSource(NULL, inboundPort, 0);
    CFRunLoopAddSource([[NSRunLoop currentRunLoop] getCFRunLoop], runLoopSource, kCFRunLoopCommonModes);

    NSLog(@"Succesfully hijacked the Razer Device Engine port.");

    inboundResponsePort = CFMessagePortCreateLocal(NULL, (__bridge CFStringRef)ResponsePortName, &responseCallback, &context, &shouldFreeInfo);

    if (inboundResponsePort)
    {
      CFRunLoopSourceRef responseRunLoopSource = CFMessagePortCreateRunLoopSource(NULL, inboundResponsePort, 0);
      CFRunLoopAddSource([[NSRunLoop currentRunLoop] getCFRunLoop], responseRunLoopSource, kCFRunLoopCommonModes);
    }
    else
    {
      NSLog(@"ERROR: Failed to setup an inbound response port.");
      return NO;
    }
  }
  else
  {
    NSLog(@"ERROR: Failed to hijack the Razer Device Engine port.");
    NSLog(@"Please ensure RzDeviceEngine has been patched.");
    return NO;
  }

  return YES;
}

BOOL connect()
{
  if (!outboundPort)
  {
    outboundPort = CFMessagePortCreateRemote(NULL, (__bridge CFStringRef)@"Razer_InterceptedMsgPort");

    if (outboundPort)
    {
      NSLog(@"Succesfully connected to the Razer Device Engine.");
    }
    else
    {
      NSLog(@"Could not connect to the Razer Device Engine.");
      NSLog(@"Please ensure RzDeviceEngine is running.");
      return NO;
    }
  }

  return YES;
}

@interface Connector : NSObject

@property(nonatomic, strong) NSTimer *timer;

@end

@implementation Connector

- (instancetype)init
{
  self = [super init];

  if (self)
  {
    NSTimer *timer = [NSTimer scheduledTimerWithTimeInterval:0.5 target:self selector:@selector(attemptConnection) userInfo:nil repeats:YES];
    [self setTimer:timer]; // Technically a race condition, but not going to be problematic during notmal execution... and I'm lazy.
  }

  return self;
}

- (void)attemptConnection
{
  if (connect())
  {
    [[self timer] invalidate];
  }
}

@end

int main(int argc, const char **argv)
{

  @autoreleasepool {
    if (listen())
    {
      [[Connector alloc] init];
      [[NSRunLoop currentRunLoop] run];
    }
  }

	return 0;
}
