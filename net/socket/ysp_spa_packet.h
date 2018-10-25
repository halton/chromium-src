// Copyright 2018 The Redcore (Beijing) Technology Co.,Ltd. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef NET_SOCKET_YSP_SPA_PACKET_H_
#define NET_SOCKET_YSP_SPA_PACKET_H_

#include <string>

#include "base/bind.h"
#include "base/macros.h"
#include "base/memory/weak_ptr.h"
#include "base/time/time.h"
#include "net/base/completion_callback.h"
#include "net/socket/client_socket_factory.h"

class YSPRedcoreSpaPacket {
 public:
  YSPRedcoreSpaPacket();
  ~YSPRedcoreSpaPacket();

  static YSPRedcoreSpaPacket* GetInstance();

  int InitValues(const std::string& device_id,
                 const std::string& username,
                 const std::string& access_cmd,
                 const std::string& server_ip,
                 const std::string& key,
                 const std::string& hmac_key,
                 int time_diff);
  int SendUdpPacket(net::CompletionCallback callback);

 private:
  int CreateRandAndTimeStamp(int time_diff);
  bool EncryptString(const std::string& plain_text, std::string* cipher_text);
  bool DecryptString(const std::string& cipher_text, std::string* plain_text);
  bool HMACEncryptString(const std::string& plain_text,
                         std::string* cipher_text);

  // encoded_msg:
  // rand_value:base64(device_id):base64(username):timestamp:version:message_type:base64(access_cmd):nat_access:server_auth:client_timeout
  std::string rand_value_;      //随机数
  std::string b64_device_id_;   // base64编码后的device_id
  std::string b64_username_;    // base64编码后的username
  time_t time_stamp_;           //时间戳
  std::string version_;         //浏览器版本
  std::string b64_access_cmd_;  // base64编码后的access_cmd
  std::string server_ip_;
  std::string block_key_;              // base64解码后的AES加密KEY
  std::string hmac_key_;               // base64解码后的HMAC KEY
  std::string encoded_string_;         //加密前的验证串
  std::string sha256_encoded_string_;  //加密前的验证串进行SHA256加密
  std::string encrypted_string_;       // AES-CBC加密串
  std::string
      hmac_sha256_encrypted_string_;  // AES-CBC加密串再进行HMAC-SHA256加密
  net::CompletionCallback completion_callback_;

  DISALLOW_COPY_AND_ASSIGN(YSPRedcoreSpaPacket);
};

#endif  // NET_SOCKET_YSP_SPA_PACKET_H_
