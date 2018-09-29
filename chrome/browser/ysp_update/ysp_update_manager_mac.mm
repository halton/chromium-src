//
//  ysp_update_manager_mac.m
//  all.ninja
//
//  Created by 刘 崴 on 18/4/4.
//
//
#import <Foundation/Foundation.h>
#include <string>
#include "chrome/browser/ysp_update/ysp_update_manager.h"
#include "chrome/browser/ysp_update/mac/SUDiskImageUnarchiver.h"
#include "base/mac/foundation_util.h"
#include "base/mac/mac_util.h"
#include "base/strings/sys_string_conversions.h"

//#include "chrome/browser/ysp_update/ysp_update_manager.h"
void PrepareUpdate(std::string path)
{
  NSString *archivePath = base::SysUTF8ToNSString(path);
  SUDiskImageUnarchiver * unarchiver = [[SUDiskImageUnarchiver alloc] initWithArchivePath:archivePath
                                                                       decryptionPassword:nil];
  if (!unarchiver) {
    NSLog(@"[YSP] failed to init SUDiskImageUnarchiver");
    return;
  }
  NSLog(@"unarchiving ...");
  [unarchiver unarchiveWithCompletionBlock:^(NSError *err){
    if (err) {
      NSLog(@"%@", [err localizedDescription]);
    } else {
      NSLog(@"[YSP] notify: Unarchiving completed");
      // copy Redcore.app/Contents to /Application/Recore.app/Contents.update
      NSString *bundlePath = [[NSBundle mainBundle] bundlePath];
      NSString *fromPath = [[archivePath stringByDeletingPathExtension] stringByAppendingString:@"/Redcore.app/Contents"];
      NSString *toPath = [bundlePath stringByAppendingString:@"/Contents.update"];
      NSFileManager *manager = [NSFileManager defaultManager];

      if ([manager fileExistsAtPath:toPath])
        [manager removeItemAtPath:toPath error:nil];

      if (![manager copyItemAtPath:fromPath toPath:toPath error:nil]) {
        NSLog(@"[YSP] failed to copy from %@ to %@", fromPath, toPath);
        return;
      }
      NSString *needUpdate = [bundlePath stringByAppendingString:@"/.need_update"];
      if (![[NSFileManager defaultManager] createFileAtPath:needUpdate contents:nil attributes:nil]) {
        NSLog(@"[YSP] failed to create %@", needUpdate);
        return;
      }

      [toPath writeToFile:needUpdate atomically:YES encoding:NSUTF8StringEncoding error:nil];
    }
  } progressBlock:^(double progress){
    NSLog(@"[YSP] onProgress: %f", progress);
  }];

}
