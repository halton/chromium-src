// Copyright 2018 The Redcore (Beijing) Technology Co.,Ltd. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef YSP_LOGIN_MANAGER_BRIDGE_H_
#define YSP_LOGIN_MANAGER_BRIDGE_H_
#include "components/ysp_login/ysp_login_manager.h"

class YSPLoginManagerObserverBridge : public YSPLoginManagerObserver {
    public:
        void OnConfigDataUpdated(const std::string& type,
                const std::string& data) override; 
        void OnLoginRequestFailure(const std::string& error) override;
        void OnLoginResponseParseFailure(const std::string& error) override;
        void OnLoginFailure(base::string16 message) override;
        void OnLoginSuccess(const base::string16& name,
                const std::string& head_image_url) override;
        void OnLogout() override;

    protected:
}

@interface NSObject(YSPLoginManagerBridge)
- (void)UpdateConfigData:(NSString *)type
                     data:(NSString *)data;
- (void)loginRequestFailure:(NSString *)error;
- (void)loginResponseParseFailure:(NSString *)error;
- (void)loginFailure:(NSString *)message;
- (void)loginSuccess:(NSString *)name
                 url:(NSString *)url;
- (void)logout;
@end


#endif //YSP_LOGIN_MANAGER_BRIDGE_H_
