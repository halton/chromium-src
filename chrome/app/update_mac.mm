#include <unistd.h>
#import <Foundation/Foundation.h>
#include "chrome/common/chrome_version.h"

void check_update(bool relaunch)
{
    NSLog(@"[YSP] check_update enter");
    // check update 
    NSString * bundlePath = [[NSBundle mainBundle] bundlePath];
    //NSLog(@"[YSP] bundle path: %@", bundlePath);
    const char *bundlePathCString = [bundlePath UTF8String];
    NSString *updateInfoFile = [bundlePath stringByAppendingString:@"/.need_update"];
    NSFileManager *manager = [NSFileManager defaultManager];
    if (![manager fileExistsAtPath:updateInfoFile])
        return;

    NSLog(@"[YSP] Found update(%@), checking path...", updateInfoFile );
    // get the path of update file
#if 0 //TODO: workaround for the issue that fileExistsAtPath always return false
    NSString *updatePath = [NSString stringWithContentsOfFile:updateInfoFile 
                                                      encoding:NSUTF8StringEncoding
                                                         error:nil];
#else
    NSString *updatePath = [bundlePath stringByAppendingString:@"/Contents.update"];
#endif

    NSLog(@"[YSP] updatePath: %@", updatePath);
    if (![manager fileExistsAtPath:updatePath]) {
        NSLog(@"[YSP] Warning: update path doesn't exist, update info file may be corrupt");
        [manager removeItemAtPath:updateInfoFile error:nil];
        return;
    }
    NSLog(@"[YSP] Found update path, start updating...");
    const char *rel_path_cstring = "/Contents/Versions/" CHROME_VERSION_STRING
                                    "/" PRODUCT_FULLNAME_STRING
                                    " Framework.framework/Resources/autoupdate.sh";
    NSString *rel_path = [NSString stringWithCString:rel_path_cstring encoding:NSUTF8StringEncoding];
    const char *installerPath = [[bundlePath stringByAppendingString:rel_path] UTF8String];
    NSLog(@"[YSP] installer Path: %s", installerPath);
    const char *updatePathCString = [updatePath UTF8String];

    pid_t pid = fork();
    if (pid == 0) {
        // child process
        int ret = execl("/bin/sh", "sh", 
                        installerPath,
                        bundlePathCString,
                        updatePathCString,
                        relaunch ? "yes" : "no",
                        NULL);
        printf(" >> [YSP] ERROR: execl returns %d\n", ret);
    } else if (pid > 0) {
        exit(0);
    }

    return;
}
