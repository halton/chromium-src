#import <Foundation/Foundation.h>
#import "SUDiskImageUnarchiver.h"
#if 0
typedef void(^CompletionBlock)(NSError *);
typedef void(^ProgressBlock)(double);

@interface AutoUpdateMAC : NSObject
    CompletionBlock completed;
    ProgressBlock progress;

-(void) onCompleted:(NSError *err);
-(void) onProgress:(double)progress;
@end

@implementation AutoUpdateMAC

-(void) onCompleted:(NSError *err) {
    if (err) {
        NSLog(@"%@", [err localizedDescription]);
    } else {
        NSLog(@"notify: Completed");
    }
}
-(void) onProgress:(double)progress {
    NSLog(@"onProgress: %f", progress);
}

@end
#endif

void ExtractFiles()
{
}

static NSRunLoop *loop = nil;
int main()
{
    NSLog(@"Start");
    NSString * res = [[NSBundle mainBundle] resourcePath];
    NSString * exec = [[NSBundle mainBundle] executablePath];
    NSCondition *cond = [[NSCondition alloc] init];
    NSLog(@"resource path: %@", res);
    NSLog(@"executable path: %@", exec);
    SUDiskImageUnarchiver * unarchiver = [[SUDiskImageUnarchiver alloc] initWithArchivePath:@"/Users/liuwei/work2/autoupdate/Redcore.dmg"
                                                                 decryptionPassword:nil];
    if (!unarchiver) {
        NSLog(@"failed to init SUDiskImageUnarchiver");
        return -1;
    }
    NSLog(@"unarchiving ...");
    [unarchiver unarchiveWithCompletionBlock:^(NSError *err){
        if (err) {
            NSLog(@"%@", [err localizedDescription]);
        } else {
            NSLog(@" >> notify: Completed");
            //CFRunLoopStop([loop getCFRunLoop]);
            NSTask *task = [[NSTask alloc] init];
            [task setLaunchPath:@"/bin/sh"];
            [task setArguments:[NSArray arrayWithObjects:@"autoupdate.sh", nil]];
            [task setStandardOutput:[NSPipe pipe]];
            [task setStandardInput:[NSPipe pipe]];

            [task launch];
#if 0
            [task waitUntilExit];
            int status = [task terminationStatus];
            if (status==0)
                NSLog(@" >> Install succeeded");
            else
                NSLog(@" >> Install script failed");
            [task release];
#endif
            NSLog(@" >> exit");
            exit(0);
        }
    } progressBlock:^(double progress){
        NSLog(@" >> onProgress: %f", progress);
    }];
    //NSRunLoop *loop = [NSRunLoop currentRunLoop];
    //[loop run];
    dispatch_main();
    NSLog(@" finished, exit");
    return 0;
}
