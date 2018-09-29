#import <Foundation/Foundation.h>
#import "ysp_sysinfo.h"

@interface NSTextCheckingResult (PartialAvailability)
- (NSRange)rangeAtIndex:(NSUInteger)idx;
@end

std::string GetHardwareUUID()
{
  NSTask *task;
  task = [[NSTask alloc] init];
  [task setLaunchPath: @"/usr/sbin/system_profiler"];
  NSArray *arguments;
  arguments = [NSArray arrayWithObjects: @"SPHardwareDataType", nil];
  [task setArguments: arguments];
  NSPipe *pipe;
  pipe = [NSPipe pipe];
  [task setStandardOutput: pipe];

  NSFileHandle *file;
  file = [pipe fileHandleForReading];

  [task launch];

  NSData *data;
  data = [file readDataToEndOfFile];

  NSString *string;
  string = [[NSString alloc] initWithData: data encoding: NSUTF8StringEncoding];
  NSError *error;
  NSRegularExpression *regex = [NSRegularExpression regularExpressionWithPattern:@"Hardware UUID: ([-\\w]+)\\b"
                                options:0 error:&error];
  NSTextCheckingResult *match = [regex firstMatchInString:string options:0 range:NSMakeRange(0, [string length]) ];
  NSRange range = [match rangeAtIndex:1];
  if (!NSEqualRanges(range, NSMakeRange(NSNotFound, 0))) {
    NSString *uuid = [string substringWithRange:range];
    return std::string([uuid UTF8String]);
  } else {
    NSLog(@"Not found!");
    return std::string();
  }
}

#if YSP_MAC_UT==1
#import <iostream>

int main (int argc, const char * argv[])
{
  std::string uuid = GetHardwareUUID();
  std::cout << "UUID: " << uuid << std::endl;
  return 0;
}
#endif

