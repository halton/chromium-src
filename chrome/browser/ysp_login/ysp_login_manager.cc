// Copyright 2018 The Redcore (Beijing) Technology Co.,Ltd. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "chrome/browser/ysp_login/ysp_login_manager.h"

#include <openssl/aes.h>
#include <locale>
#include <utility>

#include "base/base64.h"
#include "base/files/file_path.h"
#include "base/files/file_util.h"
#include "base/guid.h"
#include "base/json/json_file_value_serializer.h"
#include "base/json/json_reader.h"
#include "base/json/json_writer.h"
#include "base/native_library.h"
#include "base/path_service.h"
#include "base/strings/string_number_conversions.h"
#include "base/strings/string_util.h"
#include "base/strings/utf_string_conversions.h"
#include "base/values.h"
#include "base/win/windows_version.h"
#include "chrome/browser/browser_process.h"
#include "chrome/browser/themes/theme_properties.h"
#include "chrome/browser/ysp_login/ysp_us_report_fetcher.h"
#include "chrome/common/channel_info.h"
#include "chrome/common/chrome_paths.h"
#include "chrome/common/pref_names.h"
#include "chrome/grit/generated_resources.h"
#include "components/metrics/metrics_pref_names.h"
#include "components/prefs/pref_service.h"
#include "components/version_info/version_info.h"
#include "content/public/browser/browser_thread.h"
#include "crypto/ysp_crypto_encryption.h"
#include "crypto/ysp_crypto_header.h"
#include "net/base/mime_util.h"
#include "net/socket/transport_client_socket_pool.h"
#include "net/url_request/url_request_http_job.h"
#include "ui/base/l10n/l10n_util.h"

#if defined(OS_MACOSX)
#include "base/sys_info.h"
#include "chrome/browser/ysp_login/mac/ysp_sysinfo.h"
#endif

#if defined(OS_WIN)
#include <Iphlpapi.h>
#include <comdef.h>
#include <urlmon.h>
#include <wininet.h>
#include "base/win/registry.h"
#include "setupapi.h"
#endif  // defined(OS_WIN)

#ifdef IE_REDCORE
#include <codecvt>
#include "chrome/browser/ui/ysp_download_activex_infobar_delegate.h"
#endif

#ifdef IE_REDCORE
#pragma comment(lib, "iphlpapi.lib")
#pragma comment(lib, "netapi32")
#pragma comment(lib, "setupapi")
#endif  // IE_REDCORE

const SkColor kAvatarBackground[] = {0xffe13737, 0xfffc7936, 0xffffad36,
                                     0xff71ad2c, 0xff2dc6e0, 0xff2590ee,
                                     0xff5169fa, 0xff8158ec, 0xffb757df};
const int kAvatarBackgroundSize = sizeof(kAvatarBackground) / sizeof(SkColor);

UrlTrusted::UrlTrusted() {}

UrlTrusted::~UrlTrusted() {}

UrlTrusted::UrlTrusted(const UrlTrusted& other) = default;

namespace {
// YSP+ { Fetcher resource
const char kLoginUploadPath[] = "/client/v3/login";  // 登录接口
const char kGetUserPath[] = "/client/v3/user/";  // 获取企业和用户信息
const char kGetApplicationPath[] =
    "/client/v3/strategy/application/";  // 获取用户应用
const char kGetConfigurationStrategyPath[] =
    "/client/v3/strategy/configuration/";  // 获取用户功能配置
const char kGetSsoTokenPath[] = "/client/v3/sso/token";  // 获取SSO的token
const char kGetSWAPath[] = "/client/v3/swa/";            // 获取SWA配置
const char kGetConfigurationPCPath[] =
    "/client/v3/configuration/pc/";                        // 获取PC设置
const char kAutoGetConfigPath[] = "/client/v3/version/";   // 获取版本信息
const char kGetAuthTokenPath[] = "/client/v3/auth/token";  // 更新access_token
const char kSdpDevicePath[] = "/client/v3/sdp/device/";  // 获取/删除用户设备
const char kModifyPasswordPath[] =
    "/client/v3/device/password";  // 修改密码接口
const char kUploadAvatar[] = "/client/v3/user/avatar";  // 修改密码接口
const char kApplictionStatusPath[] =
    "/client/v3/strategy/application/market/";  // 启用/关闭应用市场应用
const char kGatewayApplictionPath[] =
    "/client/v3/gateway/application/domain/";  // 获取需要敲门的应用
// const char kUploadContentType[] = "multipart/form-data";
const char kMultipartBoundary[] = "------xdcehKrkohmfeHiyreFnWifghoDl------";
// YSP+ } /*Fetcher resource*/

YSPLoginManager* g_instance = nullptr;
// YSP+ { Resource Replace
static std::string ParsePath(const std::string& path_url) {
#if defined(OS_WIN)
  std::string file_path;
  std::string path_url_parse;

  std::string u16_url = path_url;
  int offset = u16_url.find('/', 0);
  file_path.assign(u16_url, offset + 2, (u16_url.length() - offset + 2));

  for (int i = file_path.find('/'); i != -1; i = file_path.find('/'))
    file_path.replace(i, 1, "\\");

  int colon_offset = file_path.find(':');
  if (file_path.find(':') != std::string::npos) {
    path_url_parse.append(file_path, 0, colon_offset);
    path_url_parse.append(file_path, colon_offset + 1,
                          file_path.length() - colon_offset - 1);
  } else {
    path_url_parse.assign(file_path);
  }

  return path_url_parse;
#else
  return path_url;
#endif
}

// YSP+ } /*Resource Replace*/
base::FilePath FilePathOfLoginCache(const std::string& company_id) {
  base::FilePath path;
  if (company_id.empty())
    return path;

  base::PathService::Get(chrome::DIR_USER_DATA, &path);
  path = path.Append(FILE_PATH_LITERAL("Default"));
  path = path.Append(FILE_PATH_LITERAL("Cache"));
  path = path.AppendASCII(company_id + ".dat");
  return path;
}

// YSP+ { Resource Replace
base::FilePath FileDirectoryPathOfReplaceCache(const std::string& path_url) {
  base::FilePath path;
  if (path_url.empty()) {
    return path;
  }
  std::string tmp_path = ParsePath(path_url);

  base::PathService::Get(chrome::DIR_USER_DATA, &path);
  path = path.AppendASCII("Default");
  path = path.AppendASCII("Resource");
  path = path.AppendASCII(tmp_path.c_str());
  path = path.DirName();

  return path;
}

base::FilePath FilePathOfReplaceCache(const std::string& path_url) {
  base::FilePath path;
  if (path_url.empty()) {
    return path;
  }
  std::string tmp_path = ParsePath(path_url);

  base::PathService::Get(chrome::DIR_USER_DATA, &path);
  path = path.AppendASCII("Default");
  path = path.AppendASCII("Resource");
  path = path.AppendASCII(tmp_path.c_str());

  return path;
}
// YSP+ } /*Resource Replace*/
// std::string GetOsVersionStr() {
//  static std::string strVersion;

//  if (strVersion.empty())
//  {
//    OSVERSIONINFO osVer;
//    osVer.dwOSVersionInfoSize = sizeof(osVer);
//    GetVersionEx(&osVer);

//    std::ostringstream ostrStream;

//    ostrStream << osVer.dwMajorVersion << "."
//      << osVer.dwMinorVersion << "."
//      << osVer.dwBuildNumber << "."
//      << osVer.szCSDVersion;

//    strVersion = ostrStream.str();
//  }

//  return strVersion;
//}
// YSP+ { system version
std::string GetOsVersionStr() {
  std::string system_version = "";
#if defined(OS_WIN)
  base::win::Version version = base::win::GetVersion();
  if (version == base::win::VERSION_PRE_XP)
    system_version = "Not supported";
  else if (version == base::win::VERSION_XP)
    system_version = "Windows XP";
  else if (version == base::win::VERSION_SERVER_2003)
    system_version = "Windows XP";
  else if (version == base::win::VERSION_VISTA)
    system_version = "Winsows vista";
  else if (version == base::win::VERSION_WIN7)
    system_version = "Windows 7";
  else if (version == base::win::VERSION_WIN8)
    system_version = "Windows 8";
  else if (version == base::win::VERSION_WIN8_1)
    system_version = "Windows 8.1";
  else if (version == base::win::VERSION_WIN10)
    system_version = "Windows 10";
  else if (version == base::win::VERSION_WIN10_TH2)
    system_version = "Windows 10";
  else if (version == base::win::VERSION_WIN_LAST)
    system_version = "unknown";
#elif defined(OS_MACOSX)
  system_version = "OS X " + base::SysInfo::OperatingSystemVersion();
#else
  DLOG(WARNING) << "Unknown operating system";
#endif
  return system_version;
}

#if defined(OS_WIN)
// YSP+ } /*system version*/
void FormatString(std::string& return_value, const char* format, ...) {
  char buffer_array[512] = {0};
  va_list args;

  va_start(args, format);
  int size =
      _vsnprintf(buffer_array, sizeof(buffer_array) / sizeof(buffer_array[0]),
                 format, args);

  if ((unsigned int)size < strlen(buffer_array) && size != -1) {
    return_value = buffer_array;
  } else {
    int i = 1;
    char* tmp_buffer = NULL;

    do {
      if (tmp_buffer) {
        delete[] tmp_buffer;
        tmp_buffer = NULL;
      }

      if (++i >= 100)
        break;

      tmp_buffer = new char[512 * i];
      memset(tmp_buffer, 0, 512 * i * sizeof(char));

      size = _vsnprintf(tmp_buffer, 512 * i, format, args);
    } while (size >= 512 * i || size == -1);

    if (tmp_buffer) {
      return_value = tmp_buffer;
      delete[] tmp_buffer;
      tmp_buffer = NULL;
    }
  }

  va_end(args);
}

BOOL IsPhysicalAdapter(const char* adapter_name, DWORD& media_sub_type) {
  BOOL return_value = FALSE;
  UUID guid;
  unsigned char guid_string[] = "4D36E972-E325-11CE-BFC1-08002BE10318";
  ::UuidFromStringA(guid_string, &guid);
  HDEVINFO device_info_handle =
      ::SetupDiGetClassDevsA(&guid, NULL, NULL, DIGCF_PRESENT);
  if (device_info_handle == INVALID_HANDLE_VALUE)
    return return_value;

  unsigned char net_card_key[] =
      "System\\CurrentControlSet\\Control\\Network\\{4D36E972-E325-11CE-BFC1-"
      "08002BE10318}";
  char data_buffer[MAX_PATH + 1] = {0};
  DWORD data_len = MAX_PATH;
  DWORD type = REG_SZ;
  HKEY local_net = NULL;
  std::string pnp_instance_id;

  sprintf_s(data_buffer, "%s\\%s\\Connection", net_card_key, adapter_name);
  if (::RegOpenKeyExA(HKEY_LOCAL_MACHINE, data_buffer, 0, KEY_READ,
                      &local_net) != ERROR_SUCCESS) {
    SetupDiDestroyDeviceInfoList(device_info_handle);
    return return_value;
  }

  if (::RegQueryValueExA(local_net, "PnpInstanceID", 0, &type,
                         (BYTE*)data_buffer, &data_len)) {
    SetupDiDestroyDeviceInfoList(device_info_handle);
    ::RegCloseKey(local_net);
    return return_value;
  }

  pnp_instance_id = data_buffer;
  type = REG_DWORD;
  data_len = sizeof(DWORD);
  ::RegQueryValueExA(local_net, "MediaSubType", 0, &type,
                     (BYTE*)&media_sub_type, &data_len);

  char device_id[MAX_PATH] = {0};
  ULONG bus_number = (ULONG)-1;
  SP_DEVINFO_DATA device_info_data;
  device_info_data.cbSize = sizeof(SP_DEVINFO_DATA);
  for (int i = 0;
       ::SetupDiEnumDeviceInfo(device_info_handle, i, &device_info_data); ++i) {
    ZeroMemory(device_id, sizeof(device_id));
    bus_number = (ULONG)-1;

    ::SetupDiGetDeviceInstanceIdA(device_info_handle, &device_info_data,
                                  device_id, MAX_PATH, NULL);

    if (!::SetupDiGetDeviceRegistryPropertyA(
            device_info_handle, &device_info_data, SPDRP_BUSNUMBER, NULL,
            (PBYTE)&bus_number, sizeof(ULONG), NULL))
      continue;

    if (pnp_instance_id.compare(device_id) == 0 && bus_number != (ULONG)-1) {
      return_value = TRUE;
      break;
    }
  }

  ::RegCloseKey(local_net);
  SetupDiDestroyDeviceInfoList(device_info_handle);
  return return_value;
}

// std::string GetResolution() {
//   std::string strScreen = "";
//   FormatString(strScreen, "%dx%d", GetSystemMetrics(SM_CXSCREEN),
//   GetSystemMetrics(SM_CYSCREEN)); return strScreen;
// }
#endif  // OS_WIN
std::string GetLocalMacAdd() {
  static std::string mac_address_string;

  if (!mac_address_string.empty())
    return mac_address_string;

#if defined(OS_WIN)
  typedef ULONG(WINAPI * PFN_GetAdaptersInfo)(__out_bcount_opt(*SizePointer)
                                                  PIP_ADAPTER_INFO AdapterInfo,
                                              __inout PULONG SizePointer);
  // no logic, just for compile
  base::NativeLibraryOptions opt;
  const base::NativeLibraryOptions& options = opt;
  base::NativeLibrary dll_handle = base::LoadNativeLibraryWithOptions(
      base::FilePath(L"Iphlpapi.dll"), options, nullptr);

  if (dll_handle) {
    PFN_GetAdaptersInfo pfn_adapters_info =
        (PFN_GetAdaptersInfo)base::GetFunctionPointerFromNativeLibrary(
            dll_handle, "GetAdaptersInfo");

    if (pfn_adapters_info) {
      PIP_ADAPTER_INFO pip_adapter_info = NULL;
      ULONG output_buffer_len;
      pip_adapter_info = (PIP_ADAPTER_INFO)malloc(sizeof(IP_ADAPTER_INFO));
      output_buffer_len = sizeof(IP_ADAPTER_INFO);
      if (ERROR_BUFFER_OVERFLOW ==
          pfn_adapters_info(pip_adapter_info, &output_buffer_len)) {
        free(pip_adapter_info);
        pip_adapter_info = (PIP_ADAPTER_INFO)malloc(output_buffer_len);
      }

      BYTE system_info_array[100] = {0};
      if (NO_ERROR == pfn_adapters_info(pip_adapter_info, &output_buffer_len)) {
        PIP_ADAPTER_INFO tmp_adapter = pip_adapter_info;

        while (tmp_adapter != NULL && tmp_adapter->AddressLength < 100U) {
          DWORD media_sub_type = 0;
          if (IsPhysicalAdapter(tmp_adapter->AdapterName, media_sub_type)) {
            CopyMemory(system_info_array, tmp_adapter->Address,
                       tmp_adapter->AddressLength);

            if (system_info_array[0] + system_info_array[1] +
                    system_info_array[2] + system_info_array[3] +
                    system_info_array[4] + system_info_array[5] !=
                0) {
              FormatString(mac_address_string, "%02x%02x%02x%02x%02x%02x",
                           system_info_array[0], system_info_array[1],
                           system_info_array[2], system_info_array[3],
                           system_info_array[4], system_info_array[5]);

              if (media_sub_type != 0x2)
                break;
            }
          }
          tmp_adapter = tmp_adapter->Next;
        }
      }
      free(pip_adapter_info);
    }
    base::UnloadNativeLibrary(dll_handle);
  }
#endif /*OS_WIN*/
  return mac_address_string;
}

std::string GetRegMachineId() {
#if defined(OS_WIN)
  base::win::RegKey key(HKEY_LOCAL_MACHINE,
                        L"SOFTWARE\\Microsoft\\Cryptography",
                        KEY_READ | KEY_WOW64_64KEY);
  std::wstring mid;
  key.ReadValue(L"MachineGuid", &mid);
  return base::UTF16ToUTF8(mid);
#elif defined(OS_MACOSX)
  return GetHardwareUUID();
#else
  DLOG(WARNING) << "Unknown operating system";
#endif
  return std::string();
}

//获取主板信息
void GetBoardInfo(std::string* board_info) {
#if defined(OS_WIN)
  HKEY root_key;              //主键
  LPCTSTR ip_sub_key;         //子键名称
  std::wstring brand_buffer;  //品牌
  std::wstring model_buffer;  //型号

  root_key = HKEY_LOCAL_MACHINE;
  ip_sub_key = L"HARDWARE\\DESCRIPTION\\System\\BIOS";
  base::win::RegKey key(root_key, ip_sub_key, KEY_READ | KEY_WOW64_64KEY);
  key.ReadValue(L"SystemManufacturer", &brand_buffer);
  key.ReadValue(L"SystemProductName", &model_buffer);
  board_info->clear();
  board_info->assign(base::UTF16ToUTF8(brand_buffer) + "_" +
                     base::UTF16ToUTF8(model_buffer));
#elif defined(OS_MACOSX)
  board_info->assign(base::SysInfo::HardwareModelName());
#else
  DLOG(WARNING) << "Uknown operating system";
#endif
  return;
}
#if defined(OS_WIN)
// 判断系统位数，64位返回TRUE，32位返回FALSE
// BOOL IsWow64()
// {
//   typedef BOOL(WINAPI *LPFN_ISWOW64PROCESS) (HANDLE, PBOOL);
//   LPFN_ISWOW64PROCESS fnIsWow64Process;
//   BOOL bIsWow64 = FALSE;
//   fnIsWow64Process =
//   (LPFN_ISWOW64PROCESS)GetProcAddress(GetModuleHandle(L"kernel32"),
//   "IsWow64Process"); if (NULL != fnIsWow64Process)
//   {
//     fnIsWow64Process(GetCurrentProcess(), &bIsWow64);
//   }
//   return bIsWow64;
// }

// void SetProxyEnabled(bool enabled) {
//  base::win::RegKey key(HKEY_CURRENT_USER,
//             L"SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Internet
//             Settings", KEY_SET_VALUE | KEY_WOW64_64KEY);
//  key.WriteValue(L"ProxyEnable", enabled ? 0x1 : 0x0);
//}
//
// void SetProxyInfo(
//  const base::string16& server,
//  const base::string16& except) {
//  base::win::RegKey key(HKEY_CURRENT_USER,
//             L"SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Internet
//             Settings", KEY_SET_VALUE | KEY_WOW64_64KEY);
//  key.WriteValue(L"ProxyServer", server.c_str());
//  key.WriteValue(L"ProxyOverride", except.c_str());
//}

BOOL SetProxyInfo(bool enabled,
                  const base::string16& server,
                  const base::string16& except) {
  INTERNET_PER_CONN_OPTION_LIST option_list;
  BOOL return_value;
  DWORD buffer_size = sizeof(option_list);

  // Fill the list structure.
  option_list.dwSize = sizeof(option_list);

  // NULL == LAN, otherwise connectoid name.
  option_list.pszConnection = NULL;

  // Set three options.
  if (server.empty() && except.empty())
    option_list.dwOptionCount = 1;
  else
    option_list.dwOptionCount = 3;
  option_list.pOptions = new INTERNET_PER_CONN_OPTION[3];

  // Ensure that the memory was allocated.
  if (NULL == option_list.pOptions) {
    // Return FALSE if the memory wasn't allocated.
    return FALSE;
  }

  // Set flags.
  option_list.pOptions[0].dwOption = INTERNET_PER_CONN_FLAGS;
  if (enabled) {
    option_list.pOptions[0].Value.dwValue =
        PROXY_TYPE_DIRECT | PROXY_TYPE_PROXY | PROXY_TYPE_AUTO_DETECT;
  } else {
    option_list.pOptions[0].Value.dwValue =
        PROXY_TYPE_DIRECT | PROXY_TYPE_AUTO_DETECT;
  }

  // Set proxy name.
  option_list.pOptions[1].dwOption = INTERNET_PER_CONN_PROXY_SERVER;
  option_list.pOptions[1].Value.pszValue = const_cast<LPWSTR>(server.data());

  // Set proxy override.
  option_list.pOptions[2].dwOption = INTERNET_PER_CONN_PROXY_BYPASS;
  option_list.pOptions[2].Value.pszValue = const_cast<LPWSTR>(except.data());

  // Set the options on the connection.
  return_value = InternetSetOption(NULL, INTERNET_OPTION_PER_CONNECTION_OPTION,
                                   &option_list, buffer_size);

  // Free the allocated memory.
  delete[] option_list.pOptions;

  return return_value;
}

BOOL SetPACInfo(bool enabled, const base::string16& pac) {
  INTERNET_PER_CONN_OPTION_LIST option_list;
  BOOL return_value;
  DWORD buffer_size = sizeof(option_list);

  // Fill the list structure.
  option_list.dwSize = sizeof(option_list);

  // NULL == LAN, otherwise connectoid name.
  option_list.pszConnection = NULL;

  // Set three options.
  if (pac.empty())
    option_list.dwOptionCount = 1;
  else
    option_list.dwOptionCount = 2;
  option_list.pOptions = new INTERNET_PER_CONN_OPTION[2];

  // Ensure that the memory was allocated.
  if (NULL == option_list.pOptions) {
    // Return FALSE if the memory wasn't allocated.
    return FALSE;
  }

  // Set flags.
  option_list.pOptions[0].dwOption = INTERNET_PER_CONN_FLAGS;
  if (enabled) {
    option_list.pOptions[0].Value.dwValue =
        PROXY_TYPE_DIRECT | PROXY_TYPE_AUTO_PROXY_URL | PROXY_TYPE_AUTO_DETECT;
  } else {
    option_list.pOptions[0].Value.dwValue =
        PROXY_TYPE_DIRECT | PROXY_TYPE_AUTO_DETECT;
  }

  // Set auto proxy url.
  option_list.pOptions[1].dwOption = INTERNET_PER_CONN_AUTOCONFIG_URL;
  option_list.pOptions[1].Value.pszValue = const_cast<LPWSTR>(pac.data());

  // Set the options on the connection.
  return_value = InternetSetOption(NULL, INTERNET_OPTION_PER_CONNECTION_OPTION,
                                   &option_list, buffer_size);

  // Free the allocated memory.
  delete[] option_list.pOptions;

  return return_value;
}

// void SetPACInfo(const base::string16& pac) {
//  base::win::RegKey key(HKEY_CURRENT_USER,
//             L"SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Internet
//             Settings", KEY_SET_VALUE | KEY_WOW64_64KEY);
//
//  if(pac.empty())
//    key.DeleteValue(L"AutoConfigURL");
//  else
//    key.WriteValue(L"AutoConfigURL", pac.c_str());
//}
#endif /*OS_WIN*/

std::string GetStringFromDictionary(const base::DictionaryValue* dict,
                                    const std::string& node_path) {
  if (dict) {
    std::string value;
    if (dict->GetString(node_path, &value))
      return value;
  }

  return std::string();
}

base::string16 GetString16FromDictionary(const base::DictionaryValue* dict,
                                         const std::string& node_path) {
  if (dict) {
    base::string16 value;
    if (dict->GetString(node_path, &value))
      return value;
  }

  return base::string16();
}
}  // namespace
// YSP+ { config cryptor
namespace cryptorkey {
// config data cache crypto key
const std::string& key = "3zVssP6JRSduWS2XppyvkaAi64R8rcoh";
}  // namespace cryptorkey

namespace {
class ConfigCryptor {
 public:
  ConfigCryptor();
  ~ConfigCryptor();
  bool EncryptString(const std::string& key,
                     const std::string& plain_text,
                     std::string* cipher_text);
  bool DecryptString(const std::string& key,
                     const std::string& cipher_text,
                     std::string* plain_text);
};

ConfigCryptor::ConfigCryptor() {}

ConfigCryptor::~ConfigCryptor() {}

bool ConfigCryptor::EncryptString(const std::string& key,
                                  const std::string& plain_text,
                                  std::string* cipher_text) {
  AES_KEY aes_key;
  char key_data[17] = {0};
  memcpy(key_data, key.c_str(), 8);
  memcpy(key_data + 8, key.c_str() + 8, 8);
  if (AES_set_encrypt_key(reinterpret_cast<const uint8_t*>(key_data),
                          AES_BLOCK_SIZE * 8, &aes_key) != 0) {
    return false;
  }

  const size_t out_size = plain_text.size();
  std::string pla_padding = plain_text;
  int offset = AES_BLOCK_SIZE - out_size % AES_BLOCK_SIZE;
  size_t out_size_new = out_size + offset;
  char padding[AES_BLOCK_SIZE];
  memset(padding, offset, AES_BLOCK_SIZE);
  pla_padding.append(padding, offset);
  std::string result;
  uint8_t* out_ptr =
      reinterpret_cast<uint8_t*>(base::WriteInto(&result, out_size_new + 1));

  uint8_t ivec[AES_BLOCK_SIZE] = {0};
  memcpy(ivec, key.c_str() + 16, AES_BLOCK_SIZE);

  AES_cbc_encrypt(reinterpret_cast<const uint8_t*>(pla_padding.data()), out_ptr,
                  pla_padding.size(), &aes_key, ivec, AES_ENCRYPT);
  cipher_text->swap(result);
  return true;
}

bool ConfigCryptor::DecryptString(const std::string& key,
                                  const std::string& cipher_text,
                                  std::string* plain_text) {
  AES_KEY aes_key;
  char key_data[17] = {0};
  memcpy(key_data, key.c_str(), 8);
  memcpy(key_data + 8, key.c_str() + 8, 8);
  if (AES_set_decrypt_key(reinterpret_cast<const uint8_t*>(key_data),
                          AES_BLOCK_SIZE * 8, &aes_key) != 0) {
    return false;
  }
  const size_t out_size = cipher_text.size();
  std::string result;
  uint8_t* out_ptr =
      reinterpret_cast<uint8_t*>(base::WriteInto(&result, out_size + 1));

  uint8_t ivec[AES_BLOCK_SIZE] = {0};
  memcpy(ivec, key.c_str() + 16, AES_BLOCK_SIZE);

  AES_cbc_encrypt(reinterpret_cast<const uint8_t*>(cipher_text.data()), out_ptr,
                  cipher_text.size(), &aes_key, ivec, AES_DECRYPT);
  size_t length = result.length();
  int offset = result.c_str()[length - 1];
  plain_text->clear();
  plain_text->assign(result, 0, length - offset);
  return true;
}

}  // namespace
// YSP+ } /*config cryptor*/
YSPLoginManager::YSPLoginManager()
    : login_fetcher_(nullptr),
      replace_fetcher_(nullptr),
      single_signon_fetcher_(nullptr),
      report_fetcher_(nullptr),
      get_user_fetcher_(nullptr),
      get_application_fetcher_(nullptr),
      get_configuration_strategy_fetcher_(nullptr),
      get_sso_token_fetcher_(nullptr),
      get_swa_fetcher_(nullptr),
      get_configuration_pc_fetcher_(nullptr),
      get_auth_token_fetcher_(nullptr),
      get_sdp_device_fetcher_(nullptr),
      put_sdp_device_fetcher_(nullptr),
      put_appliction_status_fetcher_(nullptr),
      get_gateway_appliction_fetcher_(nullptr),
      put_modify_password_fetcher_(nullptr),
      post_upload_avatar_fetcher_(nullptr),
      autoConfig_fetcher_(nullptr),
      login_info_(nullptr),
      application_info_(nullptr),
      strategy_info_(nullptr),
      token_info_(nullptr),
      swa_info_(nullptr),
      pc_info_(nullptr),
      auto_config_info_(nullptr),
      login_status_code_(false),
      application_status_code_(false),
      strategy_status_code_(false),
      token_status_code_(false),
      swa_status_code_(false),
      pc_status_code_(false),
      single_signon_info_(nullptr),
      window_frame_color_(-1),
      window_inactive_color_(0),
      should_parse_response_(false),
      download_(false),
      login_status_(SATUS_LOGOUT),
      ntp_login_status_(false),
      single_signon_status_(false),
      web_initialized_(false),
      offline_status_(true) {}

YSPLoginManager::~YSPLoginManager() {
  if (login_fetcher_) {
    delete login_fetcher_;
    login_fetcher_ = nullptr;
  }
  // YSP+ { Resource Replace }
  if (replace_fetcher_) {
    delete replace_fetcher_;
    replace_fetcher_ = nullptr;
  }
  // YSP+ } /*Fetcher resource*/
  // YSP+ { SingleSignOn config
  if (single_signon_fetcher_) {
    delete single_signon_fetcher_;
    single_signon_fetcher_ = nullptr;
  }
  // YSP+ } /*SingleSignOn config*/
  if (report_fetcher_) {
    delete report_fetcher_;
    report_fetcher_ = nullptr;
  }
  if (get_application_fetcher_) {
    delete get_application_fetcher_;
    get_application_fetcher_ = nullptr;
  }
  if (get_configuration_strategy_fetcher_) {
    delete get_configuration_strategy_fetcher_;
    get_configuration_strategy_fetcher_ = nullptr;
  }
  if (get_sso_token_fetcher_) {
    delete get_sso_token_fetcher_;
    get_sso_token_fetcher_ = nullptr;
  }
  if (get_swa_fetcher_) {
    delete get_swa_fetcher_;
    get_swa_fetcher_ = nullptr;
  }
  if (get_configuration_pc_fetcher_) {
    delete get_configuration_pc_fetcher_;
    get_configuration_pc_fetcher_ = nullptr;
  }
  if (get_auth_token_fetcher_) {
    delete get_auth_token_fetcher_;
    get_auth_token_fetcher_ = nullptr;
  }
  if (autoConfig_fetcher_) {
    delete autoConfig_fetcher_;
    autoConfig_fetcher_ = nullptr;
  }
  if (get_sdp_device_fetcher_) {
    delete get_sdp_device_fetcher_;
    get_sdp_device_fetcher_ = nullptr;
  }
  if (put_sdp_device_fetcher_) {
    delete put_sdp_device_fetcher_;
    put_sdp_device_fetcher_ = nullptr;
  }
  if (put_appliction_status_fetcher_) {
    delete put_appliction_status_fetcher_;
    put_appliction_status_fetcher_ = nullptr;
  }
  if (get_gateway_appliction_fetcher_) {
    delete get_gateway_appliction_fetcher_;
    get_gateway_appliction_fetcher_ = nullptr;
  }
  if (put_modify_password_fetcher_) {
    delete put_modify_password_fetcher_;
    put_modify_password_fetcher_ = nullptr;
  }
  observers_.clear();
}

// static
YSPLoginManager* YSPLoginManager::GetInstance() {
  if (!g_instance) {
    g_instance = new YSPLoginManager;
  }
  return g_instance;
}

void YSPLoginManager::Init() {
  // load login info
  base::FilePath file_path = FilePathOfLoginCache("login");
  if (base::PathExists(file_path)) {
    DCHECK(auto_login_);
    int error_code = JSONFileValueDeserializer::JSON_NO_ERROR;
    std::string error_str;
    // YSP+ { config cryptor
    std::string enc_value = "";
    std::string dec_value = "";
    error_code = base::ReadFileToString(file_path, &enc_value);
    if ((!error_code) != JSONFileValueDeserializer::JSON_NO_ERROR) {
      DLOG(INFO) << "Read config failure.";
    }

    if (!enc_value.empty()) {
      ConfigCryptor aes_crypted;
      aes_crypted.DecryptString(cryptorkey::key, enc_value, &dec_value);
    }
    std::unique_ptr<base::Value> rootValue = base::JSONReader::Read(dec_value);
    // YSP+ } /*config cryptor*/
    if ((!error_code) == JSONFileValueDeserializer::JSON_NO_ERROR) {
      login_info_ = base::DictionaryValue::From(std::move(rootValue));
    }
  }
  // load application info
  file_path = FilePathOfLoginCache("application");
  if (base::PathExists(file_path)) {
    // DCHECK(auto_login_);
    int error_code = JSONFileValueDeserializer::JSON_NO_ERROR;
    std::string error_str;
    // YSP+ { config cryptor
    std::string enc_value = "";
    std::string dec_value = "";
    error_code = base::ReadFileToString(file_path, &enc_value);
    if ((!error_code) != JSONFileValueDeserializer::JSON_NO_ERROR) {
      DLOG(INFO) << "Read config failure.";
    }
    if (!enc_value.empty()) {
      ConfigCryptor aes_crypted;
      aes_crypted.DecryptString(cryptorkey::key, enc_value, &dec_value);
    }
    std::unique_ptr<base::Value> rootValue = base::JSONReader::Read(dec_value);
    // YSP+ } /*config cryptor*/
    if ((!error_code) == JSONFileValueDeserializer::JSON_NO_ERROR) {
      application_info_ = base::DictionaryValue::From(std::move(rootValue));
    }
  }
  // load confituration_pc info
  file_path = FilePathOfLoginCache("confituration_pc");
  if (base::PathExists(file_path)) {
    DCHECK(auto_login_);
    int error_code = JSONFileValueDeserializer::JSON_NO_ERROR;
    std::string error_str;
    // YSP+ { config cryptor
    std::string enc_value = "";
    std::string dec_value = "";
    error_code = base::ReadFileToString(file_path, &enc_value);
    if ((!error_code) != JSONFileValueDeserializer::JSON_NO_ERROR) {
      DLOG(INFO) << "Read config failure.";
    }
    if (!enc_value.empty()) {
      ConfigCryptor aes_crypted;
      aes_crypted.DecryptString(cryptorkey::key, enc_value, &dec_value);
    }
    std::unique_ptr<base::Value> rootValue = base::JSONReader::Read(dec_value);
    // YSP+ } /*config cryptor*/
    if ((!error_code) == JSONFileValueDeserializer::JSON_NO_ERROR) {
      pc_info_ = base::DictionaryValue::From(std::move(rootValue));
    }
  }
  // load confituration_strategy info
  file_path = FilePathOfLoginCache("confituration_strategy");
  if (base::PathExists(file_path)) {
    DCHECK(auto_login_);
    int error_code = JSONFileValueDeserializer::JSON_NO_ERROR;
    std::string error_str;
    // YSP+ { config cryptor
    std::string enc_value = "";
    std::string dec_value = "";
    error_code = base::ReadFileToString(file_path, &enc_value);
    if ((!error_code) != JSONFileValueDeserializer::JSON_NO_ERROR) {
      DLOG(INFO) << "Read config failure.";
    }
    if (!enc_value.empty()) {
      ConfigCryptor aes_crypted;
      aes_crypted.DecryptString(cryptorkey::key, enc_value, &dec_value);
    }
    std::unique_ptr<base::Value> rootValue = base::JSONReader::Read(dec_value);
    // YSP+ } /*config cryptor*/
    if ((!error_code) == JSONFileValueDeserializer::JSON_NO_ERROR) {
      strategy_info_ = base::DictionaryValue::From(std::move(rootValue));
    }
  }
  // load sso_token info
  file_path = FilePathOfLoginCache("sso_token");
  if (base::PathExists(file_path)) {
    DCHECK(auto_login_);
    int error_code = JSONFileValueDeserializer::JSON_NO_ERROR;
    std::string error_str;
    // YSP+ { config cryptor
    std::string enc_value = "";
    std::string dec_value = "";
    error_code = base::ReadFileToString(file_path, &enc_value);
    if ((!error_code) != JSONFileValueDeserializer::JSON_NO_ERROR) {
      DLOG(INFO) << "Read config failure.";
    }
    if (!enc_value.empty()) {
      ConfigCryptor aes_crypted;
      aes_crypted.DecryptString(cryptorkey::key, enc_value, &dec_value);
    }
    std::unique_ptr<base::Value> rootValue = base::JSONReader::Read(dec_value);
    // YSP+ } /*config cryptor*/
    if ((!error_code) == JSONFileValueDeserializer::JSON_NO_ERROR) {
      token_info_ = base::DictionaryValue::From(std::move(rootValue));
    }
  }
  // load swa info
  file_path = FilePathOfLoginCache("swa");
  if (base::PathExists(file_path)) {
    DCHECK(auto_login_);
    int error_code = JSONFileValueDeserializer::JSON_NO_ERROR;
    std::string error_str;
    // YSP+ { config cryptor
    std::string enc_value = "";
    std::string dec_value = "";
    error_code = base::ReadFileToString(file_path, &enc_value);
    if ((!error_code) != JSONFileValueDeserializer::JSON_NO_ERROR) {
      DLOG(INFO) << "Read config failure.";
    }
    if (!enc_value.empty()) {
      ConfigCryptor aes_crypted;
      aes_crypted.DecryptString(cryptorkey::key, enc_value, &dec_value);
    }
    std::unique_ptr<base::Value> rootValue = base::JSONReader::Read(dec_value);
    // YSP+ } /*config cryptor*/
    if ((!error_code) == JSONFileValueDeserializer::JSON_NO_ERROR) {
      swa_info_ = base::DictionaryValue::From(std::move(rootValue));
    }
  }
  if (login_info_ && application_info_ && pc_info_ && strategy_info_ &&
      token_info_ && swa_info_) {
    SetLoginStatus(SATUS_VERIFY_CONFIG_SUCCESS);
    ntp_login_status_ = true;
    base::string16 username = GetYSPUserName();
    std::string head_image_url = GetHeadImageUrl();
    if (offline_status_) {
      offline_status_ = false;
      std::vector<YSPLoginManagerObserver*>::iterator iter = observers_.begin();
      for (; iter != observers_.end(); ++iter) {
        (*iter)->OnLoginSuccess(username, head_image_url);
      }
    }
  }
  UpdateLoginManagerSettings();

  cid_ = GetLastId(prefs::kYSPLoginLastCID);
  account_ = GetLastId(prefs::kYSPLoginLastUID);
  password_ = GetLastId(prefs::kYSPLoginLastPWD);
  PrefService* prefs = g_browser_process->local_state();
  auto_login_ = prefs->GetBoolean(prefs::kYSPAutoLogin);
}

std::string YSPLoginManager::GetCompanyId() {
  if (login_info_) {
    std::string cid;
    if (login_info_->GetString("data.company.id", &cid)) {
      return cid;
    }
  }
  return std::string();
}

void YSPLoginManager::ModifyPassword(const std::string& old_password,
                                     const std::string& new_password) {
  LOG(INFO) << "YSPLoginManager::ModifyPassword:";
  if (!put_modify_password_fetcher_) {
    put_modify_password_fetcher_ = new YSPFetcherResource(
        this, g_browser_process->system_request_context());
  } else if (put_modify_password_fetcher_->IsLoading()) {
    return;
  }

  std::vector<std::string> header_list;
  std::string access_token = GetAccessToken();
  if (!access_token.empty()) {
    header_list.push_back("access-token: " + access_token);
  }

  std::string url = GetManageServer() + kModifyPasswordPath;
  std::string post_data;
  net::AddMultipartValueForUpload("id", GetUserId(), kMultipartBoundary, "",
                                  &post_data);
  net::AddMultipartValueForUpload("newPassword", new_password,
                                  kMultipartBoundary, "", &post_data);
  net::AddMultipartValueForUpload("oldPassword", old_password,
                                  kMultipartBoundary, "", &post_data);
  net::AddMultipartFinalDelimiterForUpload(kMultipartBoundary, &post_data);
  put_modify_password_fetcher_->StarFetcherResource(
      net::URLFetcher::PUT, url, header_list, post_data, true);
  header_list.clear();
}

void YSPLoginManager::UploadAvatar(const std::string& file_name,
                                   const std::string& avatar_data) {
  if (file_name.empty() || avatar_data.empty() ||
      avatar_data.find(",") == std::string::npos ||
      avatar_data.find(":") == std::string::npos ||
      avatar_data.find(";") == std::string::npos)
    return;

  if (!post_upload_avatar_fetcher_) {
    post_upload_avatar_fetcher_ = new YSPFetcherResource(
        this, g_browser_process->system_request_context());
  } else if (post_upload_avatar_fetcher_->IsLoading()) {
    return;
  }
  std::vector<std::string> header_list;
  std::string access_token = GetAccessToken();
  if (!access_token.empty()) {
    header_list.push_back("access-token: " + access_token);
  }

  std::string url = GetManageServer() + kUploadAvatar;
  std::string post_data;
  net::AddMultipartValueForUpload("id", GetUserId(), kMultipartBoundary, "",
                                  &post_data);
  net::AddMultipartValueForUpload("companyId", GetCompanyId(),
                                  kMultipartBoundary, "", &post_data);
  post_data.append("--");
  post_data.append(kMultipartBoundary);
  post_data.append("\r\nContent-Disposition: form-data; name=\"");
  post_data.append("photo");
  post_data.append("\"; filename=\"");
  post_data.append(file_name);
  int index = avatar_data.find(",");
  std::string src_avatar =
      avatar_data.substr(index + 1, avatar_data.length() - index - 1);
  std::string avatar;
  base::Base64Decode(src_avatar, &avatar);
  std::string content_type =
      avatar_data.substr(avatar_data.find(":") + 1,
                         avatar_data.find(";") - avatar_data.find(":") - 1);
  post_data.append("\"\r\nContent-Type: ");
  post_data.append(content_type);
  post_data.append("\r\n\r\n");
  post_data.append(avatar.data(), avatar.size());
  post_data.append("\r\n");
  net::AddMultipartFinalDelimiterForUpload(kMultipartBoundary, &post_data);
  post_upload_avatar_fetcher_->StarFetcherResource(
      net::URLFetcher::POST, url, header_list, post_data, false);
  header_list.clear();
}

void YSPLoginManager::GetGatewayApplictionFetch(bool auto_fetch) {
  std::vector<std::string> header_list;
  std::string access_token = GetAccessToken();
  // if (get_gateway_appliction_fetcher_ &&
  // get_gateway_appliction_fetcher_->IsLoading())
  //  return;

  //获取需要敲门的应用列表
  if (!GetUserId().empty()) {
    header_list.push_back("userid: " + GetUserId());
    if (!access_token.empty())
      header_list.push_back("access-token: " + access_token);

    LOG(INFO) << "YSPLoginManager::GetGatewayApplictionFetch";
    if (!get_gateway_appliction_fetcher_) {
      get_gateway_appliction_fetcher_ = new YSPFetcherResource(
          this, g_browser_process->system_request_context());
    }
    std::string url =
        GetManageServer() + kGatewayApplictionPath + GetCompanyId();
    if (get_gateway_appliction_fetcher_) {
      get_gateway_appliction_fetcher_->StarFetcherResource(
          net::URLFetcher::GET, url, header_list, "", true);
    }
    header_list.clear();
  }
}

void YSPLoginManager::PutApplictionStatusFetch(
    const std::string& application_id,
    bool application_status) {
  std::vector<std::string> header_list;
  std::string access_token = GetAccessToken();
  // if (put_appliction_status_fetcher_ &&
  // put_appliction_status_fetcher_->IsLoading())
  //  return;

  //上传应用状态
  if (!GetUserId().empty()) {
    header_list.push_back("userid: " + GetUserId());
    if (!access_token.empty())
      header_list.push_back("access-token: " + access_token);
    std::string post_data;
    net::AddMultipartValueForUpload("status", application_status ? "1" : "0",
                                    kMultipartBoundary, "", &post_data);
    net::AddMultipartFinalDelimiterForUpload(kMultipartBoundary, &post_data);

    LOG(INFO) << "YSPLoginManager::PutApplictionStatusFetch";
    if (!put_appliction_status_fetcher_) {
      put_appliction_status_fetcher_ = new YSPFetcherResource(
          this, g_browser_process->system_request_context());
    }
    std::string url = GetManageServer() + kApplictionStatusPath + GetUserId() +
                      "/" + application_id;
    if (put_appliction_status_fetcher_) {
      put_appliction_status_fetcher_->StarFetcherResource(
          net::URLFetcher::PUT, url, header_list, post_data, true);
    }
    header_list.clear();
  }
}

bool YSPLoginManager::SetManageServer(const std::string& manage_server) {
  DLOG(INFO) << "YSPLoginManager::SetManageServer set manager url: "
             << manage_server;
  if (!GURL(manage_server).is_valid())
    return false;

  PrefService* prefs = g_browser_process->local_state();
  prefs->SetString(prefs::kYSPUserSetServerAddress, manage_server);
  return true;
}

std::string YSPLoginManager::GetManageServer() {
  PrefService* prefs = g_browser_process->local_state();
  std::string user_setting_server =
      prefs->GetString(prefs::kYSPUserSetServerAddress);
  if (user_setting_server.length() > 0)
    return user_setting_server;

  std::string server = prefs->GetString(prefs::kYSPServerAddress);
  if (server.length() > 0)
    return server;

  return std::string();
}

// ysp+ { AES DES and SMS4 crypt
std::string YSPLoginManager::GetCryptKey() {
  if (pc_info_) {
    base::DictionaryValue* data_dict = nullptr;
    if (pc_info_->GetDictionary("data", &data_dict)) {
      base::DictionaryValue* func_dict = nullptr;
      if (data_dict &&
          data_dict->GetDictionary("cookieEncryptMethod", &func_dict)) {
        std::string crypt_key;
        std::string value, key;
        if (func_dict) {
          func_dict->GetString("value", &value);
          func_dict->GetString("key", &key);
          if (value == "noEncrypt")
            crypt_key = "1" + key;
          else
            crypt_key = value.c_str()[0] + key;
          return crypt_key;
        }
      }
    }
  }

  return std::string();
}  // ysp+ }

// YSP+ { cache encryption
bool YSPLoginManager::GetCacheCrypt() {
  if (pc_info_) {
    base::DictionaryValue* data_dict = nullptr;
    if (pc_info_->GetDictionary("data", &data_dict)) {
      base::DictionaryValue* func_dict = nullptr;
      if (data_dict &&
          data_dict->GetDictionary("cacheEncryptMethod", &func_dict)) {
        if (func_dict) {
          std::string value;
          func_dict->GetString("value", &value);
          if (value == "1")
            return true;
        }
      }
    }
  }
  return false;
}
// ysp+ }

base::string16 YSPLoginManager::GetYSPUserName() {
  if (login_info_) {
    base::DictionaryValue* data_dict = nullptr;
    if (login_info_->GetDictionary("data", &data_dict)) {
      base::DictionaryValue* user_dict = nullptr;
      if (data_dict && data_dict->GetDictionary("user", &user_dict)) {
        base::string16 name;
        if (user_dict && user_dict->GetString("name", &name))
          return name;
      }
    }
  }

  return base::string16();
}

std::string YSPLoginManager::GetHeadImageUrl() {
  if (login_info_) {
    base::DictionaryValue* data_dict = nullptr;
    if (login_info_->GetDictionary("data", &data_dict)) {
      base::DictionaryValue* user_dict = nullptr;
      if (data_dict && data_dict->GetDictionary("user", &user_dict)) {
        std::string url;
        if (user_dict && user_dict->GetString("avatarPath", &url))
          return url;
      }
    }
  }

  return std::string();
}

void YSPLoginManager::OnApplicationFetcherResponseParse(
    std::unique_ptr<base::DictionaryValue>& response_data,
    bool from_local,
    bool auto_fetch) {
  LOG(INFO) << "YSPLoginManager::OnApplicationFetcherResponseParse auto_fetch:"
            << auto_fetch;
  std::string folder = "application";
  std::string response_status = GetResponseStatusCode(response_data);
  application_status_code_ = (response_status == "0");
  if (!from_local) {
    if (auto_login_ && !folder.empty()) {
      base::FilePath saved_path = FilePathOfLoginCache(folder);
      // YSP+ { config cryptor
      std::string response_data_string;
      std::string enc_response;
      base::JSONWriter::Write(*response_data, &response_data_string);
      ConfigCryptor aes_crypted;
      aes_crypted.EncryptString(cryptorkey::key, response_data_string,
                                &enc_response);
      if (-1 == (base::WriteFile(saved_path, enc_response.data(),
                                 enc_response.length()))) {
        DLOG(INFO) << "fail write fetch resource cache data.";
      }
      // YSP+ } /*config cryptor*/
    }
  }
  application_info_.reset();
  application_info_.reset(response_data.release());

  if (auto_fetch) {
    NotifyConfigureUpdate("application", GetDataJSAsJSONString());
  }
}

void YSPLoginManager::OnStrategyFetcherResponseParse(
    std::unique_ptr<base::DictionaryValue>& response_data,
    bool from_local,
    bool auto_fetch) {
  LOG(INFO) << "YSPLoginManager::OnStrategyFetcherResponseParse auto_fetch:"
            << auto_fetch;
  std::string folder = "confituration_strategy";
  std::string response_status = GetResponseStatusCode(response_data);
  strategy_status_code_ = (response_status == "0");
  if (!from_local) {
    if (auto_login_ && !folder.empty()) {
      base::FilePath saved_path = FilePathOfLoginCache(folder);
      // YSP+ { config cryptor
      std::string response_data_string;
      std::string enc_response;
      base::JSONWriter::Write(*response_data, &response_data_string);
      ConfigCryptor aes_crypted;
      aes_crypted.EncryptString(cryptorkey::key, response_data_string,
                                &enc_response);
      if (-1 == (base::WriteFile(saved_path, enc_response.data(),
                                 enc_response.length()))) {
        DLOG(INFO) << "fail write fetch resource cache data.";
      }
      // YSP+ } /*config cryptor*/
    }
  }
  strategy_info_.reset();
  strategy_info_.reset(response_data.release());
  if (auto_fetch) {
    NotifyConfigureUpdate("strategy", GetDataJSAsJSONString());
  }
}

void YSPLoginManager::OnTokenFetcherResponseParse(
    std::unique_ptr<base::DictionaryValue>& response_data,
    bool from_local,
    bool auto_fetch) {
  LOG(INFO) << "YSPLoginManager::OnTokenFetcherResponseParse auto_fetch:"
            << auto_fetch;
  std::string folder = "sso_token";
  std::string response_status = GetResponseStatusCode(response_data);
  token_status_code_ = (response_status == "0");
  if (!from_local) {
    if (auto_login_ && !folder.empty()) {
      base::FilePath saved_path = FilePathOfLoginCache(folder);
      // YSP+ { config cryptor
      std::string response_data_string;
      std::string enc_response;
      base::JSONWriter::Write(*response_data, &response_data_string);
      ConfigCryptor aes_crypted;
      aes_crypted.EncryptString(cryptorkey::key, response_data_string,
                                &enc_response);
      if (-1 == (base::WriteFile(saved_path, enc_response.data(),
                                 enc_response.length()))) {
        DLOG(INFO) << "fail write fetch resource cache data.";
      }
      // YSP+ } /*config cryptor*/
    }
  }

  token_info_.reset();
  token_info_.reset(response_data.release());
}

void YSPLoginManager::OnSwaFetcherResponseParse(
    std::unique_ptr<base::DictionaryValue>& response_data,
    bool from_local,
    bool auto_fetch) {
  LOG(INFO) << "YSPLoginManager::OnSwaFetcherResponseParse auto_fetch:"
            << auto_fetch;
  std::string folder = "swa";
  std::string response_status = GetResponseStatusCode(response_data);
  swa_status_code_ = (response_status == "0");
  if (!from_local) {
    if (auto_login_ && !folder.empty()) {
      base::FilePath saved_path = FilePathOfLoginCache(folder);
      // YSP+ { config cryptor
      std::string response_data_string;
      std::string enc_response;
      base::JSONWriter::Write(*response_data, &response_data_string);
      ConfigCryptor aes_crypted;
      aes_crypted.EncryptString(cryptorkey::key, response_data_string,
                                &enc_response);
      if (-1 == (base::WriteFile(saved_path, enc_response.data(),
                                 enc_response.length()))) {
        DLOG(INFO) << "fail write fetch resource cache data.";
      }
      // YSP+ } /*config cryptor*/
    }
  }

  swa_info_.reset();
  swa_info_.reset(response_data.release());
}

void YSPLoginManager::OnPcFetcherResponseParse(
    std::unique_ptr<base::DictionaryValue>& response_data,
    bool from_local,
    bool auto_fetch) {
  LOG(INFO) << "YSPLoginManager::OnPcFetcherResponseParse auto_fetch:"
            << auto_fetch;
  std::string folder = "confituration_pc";
  std::string response_status = GetResponseStatusCode(response_data);
  pc_status_code_ = (response_status == "0");
  if (!from_local) {
    if (auto_login_ && !folder.empty()) {
      base::FilePath saved_path = FilePathOfLoginCache(folder);
      // YSP+ { config cryptor
      std::string response_data_string;
      std::string enc_response;
      base::JSONWriter::Write(*response_data, &response_data_string);
      ConfigCryptor aes_crypted;
      aes_crypted.EncryptString(cryptorkey::key, response_data_string,
                                &enc_response);
      if (-1 == (base::WriteFile(saved_path, enc_response.data(),
                                 enc_response.length()))) {
        DLOG(INFO) << "fail write fetch resource cache data.";
      }
      // YSP+ } /*config cryptor*/
    }
  }

  pc_info_.reset();
  pc_info_.reset(response_data.release());
  if (auto_fetch) {
    NotifyConfigureUpdate("pc", "");
  }
}

void YSPLoginManager::OnUserFetcherResponseParse(
    std::unique_ptr<base::DictionaryValue>& response_data,
    bool from_local,
    bool auto_fetch) {
  LOG(INFO) << "YSPLoginManager::OnUserFetcherResponseParse auto_fetch:"
            << auto_fetch;
  std::string response_status = GetResponseStatusCode(response_data);
  login_status_code_ = (response_status == "0");
  std::string folder = "login";
  if (!from_local) {
    if (auto_login_ && !folder.empty()) {
      base::FilePath saved_path = FilePathOfLoginCache(folder);
      // YSP+ { config cryptor
      std::string response_data_string;
      std::string enc_response;
      base::JSONWriter::Write(*response_data, &response_data_string);
      ConfigCryptor aes_crypted;
      aes_crypted.EncryptString(cryptorkey::key, response_data_string,
                                &enc_response);
      if (-1 == (base::WriteFile(saved_path, enc_response.data(),
                                 enc_response.length()))) {
        DLOG(INFO) << "fail write fetch resource cache data.";
      }
      // YSP+ } /*config cryptor*/
    }
  }

  base::DictionaryValue* user_dict_new = nullptr;
  base::DictionaryValue* company_dict_new = nullptr;
  response_data->GetDictionary("data.user", &user_dict_new);
  response_data->GetDictionary("data.company", &company_dict_new);

  if (login_info_ && user_dict_new && company_dict_new) {
    login_info_->Set("data.user", user_dict_new->CreateDeepCopy());
    login_info_->Set("data.company", company_dict_new->CreateDeepCopy());
  }

  response_data->Clear();
  if (auto_fetch) {
    NotifyConfigureUpdate("accountInfo", GetUserInfoAsJSONString());
  }
}

void YSPLoginManager::OnAutoTokenFetcherResponseParse(
    std::unique_ptr<base::DictionaryValue>& response_data,
    bool from_local,
    bool auto_fetch) {
  LOG(INFO) << "YSPLoginManager::OnAutoTokenFetcherResponseParse auto_fetch:"
            << auto_fetch;
  std::string new_access_token = "";
  response_data->GetString("data.access_token", &new_access_token);
  SaveLastId(prefs::kYSPAccessToken, new_access_token);
  if (!send_request_list_.empty()) {
    std::vector<std::string>::iterator iter = send_request_list_.begin();
    for (; iter != send_request_list_.end(); ++iter) {
      if ((*iter).find(kGetApplicationPath) != std::string::npos)
        GetApplicationfetcher(false);
      else if ((*iter).find(kGetConfigurationPCPath) != std::string::npos)
        GetPcfetcher(false);
      else if ((*iter).find(kGetConfigurationStrategyPath) != std::string::npos)
        GetStrategyfetcher(false);
      else if ((*iter).find(kGetSsoTokenPath) != std::string::npos)
        GetTokenfetcher(false);
      else if ((*iter).find(kGetSWAPath) != std::string::npos)
        GetSwafetcher(false);
      else if ((*iter).find(kGetUserPath) != std::string::npos)
        GetUserfetcher(false);
      else if ((*iter).find(kAutoGetConfigPath) != std::string::npos)
        GetAutoConfigfetcher();
      else if ((*iter).find(kLoginUploadPath) != std::string::npos)
        Restore();
      else if ((*iter).find(kSdpDevicePath) != std::string::npos)
        GetSdpDevicefetcher(false);
    }
    send_request_list_.clear();
  }
}

void YSPLoginManager::OnSdpDeviceFetcherResponseParse(
    std::unique_ptr<base::DictionaryValue>& response_data,
    bool from_local,
    bool auto_fetch) {
  LOG(INFO) << "YSPLoginManager::OnSdpDeviceFetcherResponseParse auto_fetch:"
            << auto_fetch;
  std::string response_status = GetResponseStatusCode(response_data);
  bool device_status_code = (response_status == "0");
  base::ListValue* data_dict = nullptr;
  response_data->GetList("data", &data_dict);
  std::string result = "";
  if (data_dict) {
    PrefService* prefs = g_browser_process->local_state();
    std::string device_id = GetRegMachineId();
    if (device_id.empty())
      device_id = prefs->GetString(prefs::kYSPDeviceID);
    if (device_id.empty()) {
      device_id = base::GenerateGUID();
      prefs->SetString(prefs::kYSPDeviceID, device_id);
    }
    response_data->SetString("localDeviceId", device_id);
    base::JSONWriter::Write(*(response_data.get()), &result);
    NotifyConfigureUpdate("getLoginDeviceInfo", result);
  } else {
    if (device_status_code)
      GetSdpDevicefetcher();
    //通知页面删除设备成功
    base::JSONWriter::Write(*(response_data.get()), &result);
    NotifyConfigureUpdate("deleteLoginDevice", result);
  }
}

void YSPLoginManager::OnGatewayApplictionResponseParse(
    std::unique_ptr<base::DictionaryValue>& response_data) {
  LOG(INFO) << "YSPLoginManager::OnGatewayApplictionResponseParse";
  std::string responseStatus = GetResponseStatusCode(response_data);
  if (responseStatus == "0") {
    base::ListValue* gatewayDomainList = nullptr;
    if (response_data->GetList("data", &gatewayDomainList)) {
      if (gatewayDomainList && !gatewayDomainList->empty()) {
        std::string gatewayDomainListString = "";
        base::JSONWriter::Write(*gatewayDomainList, &gatewayDomainListString);
        NotifyConfigureUpdate("gatewayDomain", gatewayDomainListString);
      }
    }
  }
}

void YSPLoginManager::OnModifyPasswordResponseParse(std::string response) {
  LOG(INFO) << "YSPLoginManager::OnModifyPasswordResponseParse";
  NotifyConfigureUpdate("modifyPassword", response);
}

void YSPLoginManager::OnUploadAvatarResponseParse(
    std::unique_ptr<base::DictionaryValue>& response_data) {
  if (GetResponseStatusCode(response_data) != "0")
    return;

  base::DictionaryValue* data = nullptr;
  if (response_data->GetDictionary("data", &data)) {
    std::string url;
    data->GetString("url", &url);
    if (!url.empty()) {
      login_info_->SetString("data.user.avatarPath", url);
    }
  }
}

// YSP+ { Fetcher resource
std::string YSPLoginManager::GetUserId() {
  if (login_info_) {
    std::string id;
    if (login_info_->GetString("data.user.id", &id))
      return id;
  }
  return std::string();
}

//获取策略版本
int YSPLoginManager::GetStrategyVersion() {
  int version = 0;
  if (login_info_) {
    if (login_info_->GetInteger("data.strategy.version", &version)) {
      return version;
    }
  }
  return version;
}

// 获取账号创建source类型
// 1. 手动创建 2. 批量导入 3.定制导入 4.AD导入
// 注：4. 类型不能修改密码
int YSPLoginManager::GetAccountSourceType() {
  int source_type = 0;
  if (login_info_)
    login_info_->GetInteger("data.user.source", &source_type);

  return source_type;
}

// 获取策略ID
std::string YSPLoginManager::GetActivelId() {
  if (login_info_) {
    std::string active_id;
    if (login_info_->GetString("data.strategy.activeId", &active_id)) {
      if (active_id.empty() && auto_config_info_)
        auto_config_info_->GetString("data.activeStrategyId", &active_id);
      return active_id;
    }
  }
  return std::string("null");
}

// 获取access_token
std::string YSPLoginManager::GetAccessTokenForLogin() {
  if (login_info_) {
    std::string access_token;
    if (login_info_->GetString("data.token.access_token", &access_token)) {
      // DLOG(INFO) << "login_info_ access token: ", access_token;
      return access_token;
    }
  }
  return std::string();
}

std::string YSPLoginManager::GetAccessToken() {
  // DLOG(INFO) << "prefs access token: " <<
  // prefs->GetString(prefs::kYSPAccessToken);
  return GetLastId(prefs::kYSPAccessToken);
}

// 获取refresh_token
std::string YSPLoginManager::GetRefreshTokenForLogin() {
  if (login_info_) {
    std::string refresh_token;
    if (login_info_->GetString("data.token.refresh_token", &refresh_token)) {
      return refresh_token;
    }
  }
  return std::string();
}

std::string YSPLoginManager::GetRefreshToken() {
  return GetLastId(prefs::kYSPRefreshToken);
}

void YSPLoginManager::GetApplicationfetcher(bool auto_fetch) {
  std::vector<std::string> header_list;
  std::string access_token = GetAccessToken();
  // if (get_application_fetcher_ && get_application_fetcher_->IsLoading())
  //  return;
  header_list.push_back("userid: " + GetUserId());
  if (!access_token.empty())
    header_list.push_back("access-token: " + access_token);

  //获取用户应用
  LOG(INFO) << "YSPLoginManager::GetApplicationfetcher auto_fetch:"
            << auto_fetch;
  if (!get_application_fetcher_) {
    get_application_fetcher_ = new YSPFetcherResource(
        this, g_browser_process->system_request_context());
  }
  std::string url = GetManageServer() + kGetApplicationPath + GetActivelId();
  if (get_application_fetcher_) {
    get_application_fetcher_->StarFetcherResource(net::URLFetcher::GET, url,
                                                  header_list, "", auto_fetch);
  }
  header_list.clear();
}

void YSPLoginManager::GetStrategyfetcher(bool auto_fetch) {
  std::vector<std::string> header_list;
  std::string access_token = GetAccessToken();
  // if (get_configuration_strategy_fetcher_ &&
  // get_configuration_strategy_fetcher_->IsLoading())
  //  return;

  header_list.push_back("userid: " + GetUserId());
  if (!access_token.empty())
    header_list.push_back("access-token: " + access_token);

  //获取用户功能配置
  LOG(INFO) << "YSPLoginManager::GetStrategyfetcher auto_fetch:" << auto_fetch;
  if (!get_configuration_strategy_fetcher_) {
    get_configuration_strategy_fetcher_ = new YSPFetcherResource(
        this, g_browser_process->system_request_context());
  }
  std::string url = GetManageServer() + kGetConfigurationStrategyPath +
                    GetUserId() + "/" + GetActivelId();
  if (get_configuration_strategy_fetcher_) {
    get_configuration_strategy_fetcher_->StarFetcherResource(
        net::URLFetcher::GET, url, header_list, "", auto_fetch);
  }
  header_list.clear();
}

void YSPLoginManager::GetTokenfetcher(bool auto_fetch) {
  std::vector<std::string> header_list;
  std::string access_token = GetAccessToken();
  // if (get_sso_token_fetcher_ && get_sso_token_fetcher_->IsLoading())
  //  return;
  if (GetUserId().empty() || GetCompanyId().empty())
    return;
  header_list.push_back("userid: " + GetUserId());
  if (!access_token.empty())
    header_list.push_back("access-token: " + access_token);

  // 获取sso的token
  LOG(INFO) << "YSPLoginManager::GetTokenfetcher auto_fetch:" << auto_fetch;
  std::string post_data;
  net::AddMultipartValueForUpload("companyId", GetCompanyId(),
                                  kMultipartBoundary, "", &post_data);
  net::AddMultipartValueForUpload("username", account_, kMultipartBoundary, "",
                                  &post_data);
  net::AddMultipartValueForUpload("password", password_, kMultipartBoundary, "",
                                  &post_data);
  net::AddMultipartFinalDelimiterForUpload(kMultipartBoundary, &post_data);
  if (!get_sso_token_fetcher_) {
    get_sso_token_fetcher_ = new YSPFetcherResource(
        this, g_browser_process->system_request_context());
  }

  std::string url = GetManageServer() + kGetSsoTokenPath;
  if (get_sso_token_fetcher_) {
    get_sso_token_fetcher_->StarFetcherResource(
        net::URLFetcher::POST, url, header_list, post_data, auto_fetch);
  }
  header_list.clear();
}

void YSPLoginManager::GetSwafetcher(bool auto_fetch) {
  std::vector<std::string> header_list;
  std::string access_token = GetAccessToken();
  // if (get_swa_fetcher_ && get_swa_fetcher_->IsLoading())
  //  return;
  header_list.push_back("userid: " + GetUserId());
  if (!access_token.empty())
    header_list.push_back("access-token: " + access_token);

  // 获取SWA配置
  LOG(INFO) << "YSPLoginManager::GetSwafetcher auto_fetch:" << auto_fetch;
  if (!get_swa_fetcher_) {
    get_swa_fetcher_ = new YSPFetcherResource(
        this, g_browser_process->system_request_context());
  }
  std::string url =
      GetManageServer() + kGetSWAPath + GetUserId() + "/" + GetActivelId();
  if (get_swa_fetcher_) {
    get_swa_fetcher_->StarFetcherResource(net::URLFetcher::GET, url,
                                          header_list, "", auto_fetch);
  }
  header_list.clear();
}

void YSPLoginManager::GetPcfetcher(bool auto_fetch) {
  std::vector<std::string> header_list;
  std::string access_token = GetAccessToken();
  // if (get_configuration_pc_fetcher_ &&
  // get_configuration_pc_fetcher_->IsLoading())
  //  return;
  if (GetUserId().empty() || GetCompanyId().empty())
    return;

  header_list.push_back("userid: " + GetUserId());
  if (!access_token.empty())
    header_list.push_back("access-token: " + access_token);

  // 获取PC配置
  LOG(INFO) << "YSPLoginManager::GetPcfetcher auto_fetch:" << auto_fetch;
  if (!get_configuration_pc_fetcher_) {
    get_configuration_pc_fetcher_ = new YSPFetcherResource(
        this, g_browser_process->system_request_context());
  }
  std::string url =
      GetManageServer() + kGetConfigurationPCPath + GetCompanyId();
  if (get_configuration_pc_fetcher_) {
    get_configuration_pc_fetcher_->StarFetcherResource(
        net::URLFetcher::GET, url, header_list, "", auto_fetch);
  }
  header_list.clear();
}

void YSPLoginManager::GetUserfetcher(bool auto_fetch) {
  std::vector<std::string> header_list;
  std::string access_token = GetAccessToken();
  // if (get_user_fetcher_ && get_user_fetcher_->IsLoading())
  //  return;

  //获取企业和用户信息
  if (!GetUserId().empty() && !GetCompanyId().empty()) {
    header_list.push_back("userid: " + GetUserId());
    if (!access_token.empty())
      header_list.push_back("access-token: " + access_token);

    LOG(INFO) << "YSPLoginManager::GetUserfetcher auto_fetch:" << auto_fetch;
    if (!get_user_fetcher_) {
      get_user_fetcher_ = new YSPFetcherResource(
          this, g_browser_process->system_request_context());
    }
    std::string url =
        GetManageServer() + kGetUserPath + GetCompanyId() + "/" + GetUserId();
    if (get_user_fetcher_) {
      get_user_fetcher_->StarFetcherResource(net::URLFetcher::GET, url,
                                             header_list, "", auto_fetch);
    }
    header_list.clear();
  }
}

void YSPLoginManager::GetAuthTokenfetcher(std::string source_url,
                                          bool auto_fetch) {
  if (!send_request_list_.empty()) {
    int count = 0;
    std::vector<std::string>::iterator iter = send_request_list_.begin();
    for (; iter != send_request_list_.end(); ++iter) {
      if (*iter == source_url)
        count++;
    }
    if (count == 0)
      send_request_list_.push_back(source_url);
  } else {
    send_request_list_.push_back(source_url);
  }

  std::vector<std::string> header_list;
  std::string access_token = GetAccessToken();
  if (get_auth_token_fetcher_ && get_auth_token_fetcher_->IsLoading())
    return;
  header_list.push_back("userid: " + GetUserId());

  // 获取auth的token
  LOG(INFO) << "YSPLoginManager::GetAuthTokenfetcher auto_fetch:" << auto_fetch;
  std::string post_data;
  net::AddMultipartValueForUpload("refresh_token", GetRefreshToken(),
                                  kMultipartBoundary, "", &post_data);
  net::AddMultipartValueForUpload("access_token", GetAccessToken(),
                                  kMultipartBoundary, "", &post_data);
  net::AddMultipartFinalDelimiterForUpload(kMultipartBoundary, &post_data);
  if (!get_auth_token_fetcher_) {
    get_auth_token_fetcher_ = new YSPFetcherResource(
        this, g_browser_process->system_request_context());
  }
  std::string url = GetManageServer() + kGetAuthTokenPath;
  if (get_auth_token_fetcher_) {
    get_auth_token_fetcher_->StarFetcherResource(
        net::URLFetcher::POST, url, header_list, post_data, auto_fetch);
  }

  header_list.clear();
}

void YSPLoginManager::GetAutoConfigfetcher(bool auto_fetch) {
  std::vector<std::string> header_list;
  std::string access_token = GetAccessToken();

  //获取企业和用户信息
  if (!GetUserId().empty() && !GetCompanyId().empty()) {
    header_list.push_back("userid: " + GetUserId());
    if (!access_token.empty())
      header_list.push_back("access-token: " + access_token);
    LOG(INFO) << "YSPLoginManager::GetAutoConfigfetcher.";
    int strategy_version = YSPLoginManager::GetInstance()->GetStrategyVersion();
    char version_str[10] = "";
    sprintf(version_str, "%d", strategy_version);
    if (!autoConfig_fetcher_) {
      autoConfig_fetcher_ =
          new YSPAutoConfigFetcher(g_browser_process->system_request_context());
    }
    std::string url = GetManageServer() + kAutoGetConfigPath + GetCompanyId() +
                      "/1/" + GetUserId() + "/" + GetActivelId() + "/" +
                      version_str;
    if (autoConfig_fetcher_) {
      autoConfig_fetcher_->StartGetConfigVersion(url, header_list, auto_fetch);
    }
    header_list.clear();
  }
}

void YSPLoginManager::GetSdpDevicefetcher(bool auto_fetch) {
  std::vector<std::string> header_list;
  std::string access_token = GetAccessToken();
  // if (get_sdp_device_fetcher_ && get_sdp_device_fetcher_->IsLoading())
  //  return;

  //获取用户登陆过的设备
  if (!GetUserId().empty()) {
    header_list.push_back("userid: " + GetUserId());
    if (!access_token.empty())
      header_list.push_back("access-token: " + access_token);
    LOG(INFO) << "YSPLoginManager::GetSdpDevicefetcher auto_fetch:"
              << auto_fetch;
    if (!get_sdp_device_fetcher_) {
      get_sdp_device_fetcher_ = new YSPFetcherResource(
          this, g_browser_process->system_request_context());
    }
    std::string url = GetManageServer() + kSdpDevicePath + GetUserId();
    if (get_sdp_device_fetcher_) {
      get_sdp_device_fetcher_->StarFetcherResource(net::URLFetcher::GET, url,
                                                   header_list, "", auto_fetch);
    }
    header_list.clear();
  }
}

// ysp+ { ysp single sign on
std::string YSPLoginManager::GetYSPSingleSignOnString() {
  if (pc_info_) {
    base::DictionaryValue* data_dict = nullptr;
    if (pc_info_->GetDictionary("data", &data_dict)) {
      base::ListValue* domain_dict = nullptr;
      if (data_dict && data_dict->GetList("ssoTokenUrls.list", &domain_dict)) {
        std::string domain_list_string = "";
        if (base::JSONWriter::Write(*domain_dict, &domain_list_string))
          return domain_list_string;
      }
    }
  }
  return std::string();
}
// ysp+ }

void YSPLoginManager::PutSdpDevicefetcher(const std::string& device_id,
                                          bool auto_fetch) {
  std::vector<std::string> header_list;
  std::string access_token = GetAccessToken();
  // if (put_sdp_device_fetcher_ && put_sdp_device_fetcher_->IsLoading())
  //  return;

  //删除用户登陆过的设备
  if (!GetUserId().empty()) {
    header_list.push_back("userid: " + GetUserId());
    if (!access_token.empty())
      header_list.push_back("access-token: " + access_token);
    std::string post_data;
    net::AddMultipartValueForUpload("deviceId", device_id, kMultipartBoundary,
                                    "", &post_data);
    net::AddMultipartFinalDelimiterForUpload(kMultipartBoundary, &post_data);
    LOG(INFO) << "YSPLoginManager::PutSdpDevicefetcher auto_fetch:"
              << auto_fetch;
    if (!put_sdp_device_fetcher_) {
      put_sdp_device_fetcher_ = new YSPFetcherResource(
          this, g_browser_process->system_request_context());
    }
    std::string url =
        GetManageServer() + kSdpDevicePath + GetUserId() + "/" + device_id;
    if (put_sdp_device_fetcher_) {
      put_sdp_device_fetcher_->StarFetcherResource(
          net::URLFetcher::PUT, url, header_list, post_data, auto_fetch);
    }
    header_list.clear();
  }
}

std::string YSPLoginManager::GetResponseStatusCode(
    std::unique_ptr<base::DictionaryValue>& response_data) {
  std::string status_code = "-1";
  if (response_data) {
    response_data->GetString("errCode", &status_code);
  }
  return status_code;
}

base::string16 YSPLoginManager::GetResponseMessage(
    std::unique_ptr<base::DictionaryValue>& response_data) {
  base::string16 message = base::string16();
  if (response_data) {
    response_data->GetString("message", &message);
  }
  DLOG(ERROR) << "GetResponseMessage message: " << message;
  return message;
}

base::string16 YSPLoginManager::GetResponseErrorMessage(
    const std::string error_code) {
  base::string16 message = base::string16();
  if (error_code == "E1001") {
    message = l10n_util::GetStringUTF16(IDS_YSP_ACCESS_PROHIBITION);
  } else if (error_code == "E1002") {
    message = l10n_util::GetStringUTF16(IDS_YSP_ACCESS_TOKEN_EXPIRED);
  } else if (error_code == "E1003") {
    message = l10n_util::GetStringUTF16(IDS_YSP_REFRESH_TOKEN_EXPIRED);
  } else if (error_code == "E2001") {
    message = l10n_util::GetStringUTF16(IDS_YSP_COMPANY_NOT_FOUND);
  } else if (error_code == "E3001") {
    message = l10n_util::GetStringUTF16(IDS_YSP_USER_NOT_FOUND);
  } else if (error_code == "E3002") {
    message = l10n_util::GetStringUTF16(IDS_YSP_WRONG_PASSWORD);
  } else if (error_code == "E3003") {
    message = l10n_util::GetStringUTF16(IDS_YSP_AD_USERS_FORBID_UPDATING);
  } else if (error_code == "E3004") {
    message = l10n_util::GetStringUTF16(IDS_YSP_USER_DISABLED);
  } else if (error_code == "E4001") {
    message = l10n_util::GetStringUTF16(IDS_YSP_DEVICE_DISABLED);
  } else if (error_code == "E9999") {
    message = l10n_util::GetStringUTF16(IDS_YSP_HANDLE_ERROR);
  } else if (error_code == "-1") {
    base::string16 errstr =
        l10n_util::GetStringUTF16(IDS_YSP_SERVER_ERROR_CODE);
    message = errstr + base::UTF8ToUTF16(error_code + " ") +
              l10n_util::GetStringUTF16(IDS_YSP_CONNENT_SERVICE_FAILED);
  } else if (error_code == "404" || error_code == "403" ||
             error_code == "400" || error_code == "410") {
    base::string16 errstr =
        l10n_util::GetStringUTF16(IDS_YSP_SERVER_ERROR_CODE);
    message = errstr + base::UTF8ToUTF16(error_code + " ") +
              l10n_util::GetStringUTF16(IDS_YSP_SERVER_RESOURCE_ERROR);
  } else if (error_code == "502" || error_code == "500" ||
             error_code == "501" || error_code == "503" ||
             error_code == "504" || error_code == "505") {
    base::string16 errstr =
        l10n_util::GetStringUTF16(IDS_YSP_SERVER_ERROR_CODE);
    message = errstr + base::UTF8ToUTF16(error_code + " ") +
              l10n_util::GetStringUTF16(IDS_YSP_SERVER_INTERNAL_ERROR);
  } else {
    message = l10n_util::GetStringUTF16(IDS_YSP_UNKNOWN_ERROR);
  }
  LOG(ERROR) << "GetResponseErrorMessage message: " << message;
  return message;
}
// YSP+ } /*Fetcher resource*/
std::string YSPLoginManager::GetLoginStatusCode() {
  std::string status_code = "-1";
  if (login_info_) {
    login_info_->GetString("errCode", &status_code);
  }
  DLOG(INFO) << "YSPLoginManager::GetLoginStatusCode: " << status_code;
  return status_code;
}

void YSPLoginManager::SetLoginStatus(int status) {
  DLOG(INFO) << "YSPLoginManager::SetLoginStatus status:" << status;
  login_status_ = status;
  // TODO(who): to noify all observers that the status is changed.
  // std::vector<YSPLoginManagerObserver*>::iterator iter = observers_.begin();
  // for (; iter != observers_.end(); ++iter) {
  //  (*iter)->OnStatusChanged(status);
  //}
}

base::string16 YSPLoginManager::GetLoginErrorMessage() {
  base::string16 message = base::string16();
  if (login_info_) {
    login_info_->GetString("message", &message);
  }
  return message;
}

std::string YSPLoginManager::GetUserInfoAsJSONString() {
  if (login_info_) {
    base::DictionaryValue* data_dict = nullptr;
    if (login_info_->GetDictionary("data", &data_dict)) {
      if (data_dict) {
        std::string json_string;
        base::JSONWriter::Write(*data_dict, &json_string);
        return json_string;
      }
    }
  }

  return "";
}

std::string YSPLoginManager::GetDataJSAsJSONString() {
  if (application_info_) {
    base::ListValue* data_list = nullptr;
    if (application_info_->GetList("data", &data_list)) {
      if (data_list) {
        std::string json_string;
        base::JSONWriter::Write(*data_list, &json_string);
        return json_string;
      }
    }
  }

  return "";
}
// ysp+ { show config
std::string YSPLoginManager::GetShowConfigAsJSONString() {
  std::string result = "";
  base::DictionaryValue config_dict;
  if (login_info_) {
    base::DictionaryValue* temp_dict = nullptr;
    login_info_->GetDictionary("data", &temp_dict);
    if (temp_dict) {
      config_dict.SetDictionary("login", temp_dict->CreateDeepCopy());
    }
  }
  if (application_info_) {
    base::ListValue* temp_dict = nullptr;
    application_info_->GetList("data", &temp_dict);
    if (temp_dict) {
      config_dict.SetList("application", temp_dict->CreateDeepCopy());
    }
  }
  if (strategy_info_) {
    base::DictionaryValue* temp_dict = nullptr;
    strategy_info_->GetDictionary("data", &temp_dict);
    if (temp_dict) {
      config_dict.SetDictionary("strategy", temp_dict->CreateDeepCopy());
    }
  }
  if (token_info_) {
    base::DictionaryValue* temp_dict = nullptr;
    token_info_->GetDictionary("data", &temp_dict);
    if (temp_dict) {
      config_dict.SetDictionary("tocken", temp_dict->CreateDeepCopy());
    }
  }
  if (swa_info_) {
    base::ListValue* temp_dict = nullptr;
    swa_info_->GetList("data", &temp_dict);
    if (temp_dict) {
      config_dict.SetList("swa", temp_dict->CreateDeepCopy());
    }
  }
  if (pc_info_) {
    base::DictionaryValue* temp_dict = nullptr;
    pc_info_->GetDictionary("data", &temp_dict);
    if (temp_dict) {
      config_dict.SetDictionary("pc", temp_dict->CreateDeepCopy());
    }
  }
  base::JSONWriter::Write(config_dict, &result);

  return result;
}

// ysp+ } /*show config*/
// TODO(matianzhi) ysp+{push server api}
void YSPLoginManager::SetPushData(const std::string& value) {
  if (!value.empty()) {
    std::unique_ptr<base::DictionaryValue> push_data_dict = nullptr;
    std::unique_ptr<base::Value> root_value = base::JSONReader::Read(value);
    push_data_dict = base::DictionaryValue::From(std::move(root_value));
    std::string event_id;
    push_data_dict->GetString("eventID", &event_id);
    if (event_id == "allowLogin") {
      // push data crypto key
      std::string key = "CSkBQrCX7X7Tlsp38jZQj5uRD0w6MwUx";
      std::string dec_user_info = "";
      std::string user_info, user_info_base64;
      push_data_dict.get()->GetString("data.identity", &user_info_base64);
      ConfigCryptor aes_crypted;
      base::Base64Decode(user_info_base64, &user_info);
      aes_crypted.DecryptString(key, user_info, &dec_user_info);
      if (!dec_user_info.empty()) {
        std::unique_ptr<base::Value> user_info_value =
            base::JSONReader::Read(dec_user_info);
        std::unique_ptr<base::DictionaryValue> user_info_dict =
            base::DictionaryValue::From(std::move(user_info_value));
        std::string server, username, password;
        user_info_dict->GetString("server", &server);
        user_info_dict->GetString("username", &username);
        user_info_dict->GetString("password", &password);
        if (!server.empty() && !username.empty() && !password.empty()) {
          StartLogin(server, username, password);
        }
        DLOG(INFO) << "user info: " << dec_user_info;
      }
    } else if (event_id == "pushNotification") {
      GetAutoConfigfetcher();
    } else if (event_id == "pushMessage") {
      std::string type = "";
      if (push_data_dict &&
          push_data_dict.get()->GetString("data.name", &type)) {
        if (type == "removeDevice") {
          std::string device_id = "";
          if (push_data_dict && push_data_dict.get()->GetString(
                                    "data.content.deviceId", &device_id)) {
            std::string key = "deviceId";
            std::string local_device_id = GetValueForKey(key);
            if (local_device_id == device_id) {
              // clear user data
              NotifyConfigureUpdate("removeDevice", "");
              Logout();
            } else {
              GetSdpDevicefetcher();
            }
          }
        }
      }
    } else if (event_id == "reconnect") {
      GetAutoConfigfetcher();
    } else {
      DLOG(INFO) << "YSPLoginManager::SetPushData value: " << value;
    }
    UpdateLoginManagerSettings();
  }
}
// ysp+
bool YSPLoginManager::Restore() {
  if (!auto_login_ || cid_.empty() || account_.empty() || password_.empty())
    return false;
  if (login_info_) {
    double expiry_date = 0;
    login_info_->GetDouble("data.token.expiryDate", &expiry_date);
    if (expiry_date != 0) {
      if (base::Time::Now().ToJavaTime() >= expiry_date) {
        Logout();
        return false;
      }
    }
  }

  SetLoginStatus(SATUS_LOGIN_FETCH_START);
  std::vector<std::string> header_list;
  if (!GetAccessToken().empty())
    header_list.push_back("access-token: " + GetAccessToken());
  DLOG(INFO) << "prefs access token: " << GetAccessToken();
  should_parse_response_ = true;
  login_status_code_ = application_status_code_ = strategy_status_code_ =
      token_status_code_ = swa_status_code_ = pc_status_code_ = false;

  // login online
  std::string domain_name = "";
  std::string port = GURL(cid_).port();
  if (port.empty())
    domain_name = GURL(cid_).host();
  else
    domain_name = GURL(cid_).host() + ":" + GURL(cid_).port();
  if (!login_fetcher_) {
    login_fetcher_ =
        new YSPLoginFetcher(this, g_browser_process->system_request_context());
  }
  if (login_fetcher_) {
    std::string device_info = GetDeviceInfo();
    PrefService* prefs = g_browser_process->local_state();
    std::string device_id = GetRegMachineId();
    if (device_id.empty())
      device_id = prefs->GetString(prefs::kYSPDeviceID);
    if (device_id.empty()) {
      device_id = base::GenerateGUID();
      prefs->SetString(prefs::kYSPDeviceID, device_id);
    }
    content::BrowserThread::PostTask(
        content::BrowserThread::IO, FROM_HERE,
        base::Bind(&net::TransportConnectJob::SetLoginSpaValue, device_id,
                   account_, domain_name));
    login_fetcher_->StartLogin(GetManageServer(), domain_name, account_,
                               password_, header_list, device_info);
  }

  return true;  // restored
}

void YSPLoginManager::StartLogin(const std::string& cid,
                                 const std::string& account,
                                 const std::string& password) {
  account_ = account;
  password_ = password;
  cid_ = cid;
  std::vector<std::string> header_list;
  SetLoginStatus(SATUS_LOGIN_FETCH_START);
  SaveLastId(prefs::kYSPLoginLastCID, cid_);
  SaveLastId(prefs::kYSPLoginLastUID, account_);
  should_parse_response_ = true;

  if (GURL(cid).is_valid() == false) {
    std::vector<YSPLoginManagerObserver*>::iterator iter = observers_.begin();
    for (; iter != observers_.end(); ++iter) {
      (*iter)->OnLoginFailure(
          l10n_util::GetStringUTF16(IDS_YSP_LOGIN_ERROR_REQUEST_FAILED));
    }
    return;
  }
  PrefService* prefs = g_browser_process->local_state();
  prefs->SetString(prefs::kYSPServerAddress, cid);
  prefs->SetString(prefs::kYSPUserSetServerAddress, cid);

  login_status_code_ = false;
  application_status_code_ = false;
  strategy_status_code_ = false;
  token_status_code_ = false;
  swa_status_code_ = false;
  pc_status_code_ = false;

  DLOG(INFO) << "YSPLoginManager::StartLogin login online";
  // login online
  std::string domain_name = "";
  std::string port = GURL(cid).port();
  if (port.empty())
    domain_name = GURL(cid).host();
  else
    domain_name = GURL(cid).host() + ":" + GURL(cid).port();
  if (!login_fetcher_) {
    login_fetcher_ =
        new YSPLoginFetcher(this, g_browser_process->system_request_context());
  }
  if (login_fetcher_) {
    std::string device_info = GetDeviceInfo();
    std::string device_id = GetRegMachineId();
    if (device_id.empty())
      device_id = prefs->GetString(prefs::kYSPDeviceID);
    if (device_id.empty()) {
      device_id = base::GenerateGUID();
      prefs->SetString(prefs::kYSPDeviceID, device_id);
    }
    content::BrowserThread::PostTask(
        content::BrowserThread::IO, FROM_HERE,
        base::Bind(&net::TransportConnectJob::SetLoginSpaValue, device_id,
                   account_, domain_name));
    login_fetcher_->StartLogin(GetManageServer(), domain_name, account_,
                               password, header_list, device_info);
  }
}

void YSPLoginManager::ClearCache() {
  base::FilePath file_path = FilePathOfLoginCache("login");
  if (base::PathExists(file_path)) {
    base::DeleteFile(file_path, false);
  }
  login_info_.reset();
  file_path = FilePathOfLoginCache("application");
  if (base::PathExists(file_path)) {
    base::DeleteFile(file_path, false);
  }
  application_info_.reset();
  file_path = FilePathOfLoginCache("confituration_pc");
  if (base::PathExists(file_path)) {
    base::DeleteFile(file_path, false);
  }
  pc_info_.reset();
  file_path = FilePathOfLoginCache("confituration_strategy");
  if (base::PathExists(file_path)) {
    base::DeleteFile(file_path, false);
  }
  strategy_info_.reset();
  file_path = FilePathOfLoginCache("sso_token");
  if (base::PathExists(file_path)) {
    base::DeleteFile(file_path, false);
  }
  token_info_.reset();
  file_path = FilePathOfLoginCache("swa");
  if (base::PathExists(file_path)) {
    base::DeleteFile(file_path, false);
  }
  swa_info_.reset();
  // YSP+ { SingleSignOn config
  single_signon_info_.reset();
  content::BrowserThread::PostTask(
      content::BrowserThread::IO, FROM_HERE,
      base::Bind(&net::URLRequestHttpJob::SetSsoConfigValue, ""));
  // YSP+ } /*SingleSignOn config*/
  login_status_code_ = false;
  application_status_code_ = false;
  strategy_status_code_ = false;
  token_status_code_ = false;
  swa_status_code_ = false;
  pc_status_code_ = false;
}

void YSPLoginManager::Logout() {
  window_frame_color_ = -1;
  window_inactive_color_ = 0;
  should_parse_response_ = false;
  SaveLastId(prefs::kYSPLoginLastPWD, "");
  SaveLastId(prefs::kYSPAccessToken, "");
  SaveLastId(prefs::kYSPRefreshToken, "");
  password_.clear();
  SetLoginStatus(SATUS_LOGOUT);  // ysp+ { auto get config }
  ntp_login_status_ = false;
  // Once user does logout manually, the auto_login becomes false.
  net::URLRequestHttpJob::ClearHeader();
  std::vector<YSPLoginManagerObserver*>::iterator iter = observers_.begin();
  for (; iter != observers_.end(); ++iter) {
    (*iter)->OnLogout();
  }
  // clear local cache
  ClearCache();
}

// YSP+ { Resource Replace
// YSPReplaceFetcherDelegate:
void YSPLoginManager::OnReplaceRequestFailure(const std::string& error) {
  DLOG(INFO) << "YSPLoginManager::OnReplaceRequestFailure";
  LOG(INFO) << "Resource replace file download failure !";
  // std::vector<YSPLoginManagerObserver*>::iterator iter = observers_.begin();
  // for (; iter != observers_.end(); ++iter) {
  //  (*iter)->OnLoginRequestFailure();
  //}
}

void YSPLoginManager::OnReplaceResponseParseSuccess(
    const std::string& response_data,
    const std::string& path_url) {
  if (response_data.empty()) {
    DLOG(INFO)
        << "YSPLoginManager::OnReplaceResponseParseSuccess ignore response.";
    return;
  }
  DLOG(INFO) << "YSPLoginManager::OnReplaceResponseParseSuccess";
  OnReplaceResponseParseSuccessInternal(response_data, path_url, false);
}

void YSPLoginManager::OnReplaceResponseParseSuccessInternal(
    const std::string& response_data,
    const std::string& path_url,
    bool from_local) {
  DLOG(INFO) << "YSPLoginManager::OnReplaceResponseParseSuccessInternal from "
                "local ? : "
             << from_local;
  // save to local cache
  if (!from_local) {
    base::FilePath save_directory_path =
        FileDirectoryPathOfReplaceCache(path_url);
    base::CreateDirectoryAndGetError(save_directory_path, NULL);
    base::FilePath saved_path = FilePathOfReplaceCache(path_url);
    DLOG(INFO) << "saved_path:" << saved_path.value();
    if (-1 == (base::WriteFile(saved_path, response_data.c_str(),
                               response_data.length()))) {
      DLOG(INFO) << "fail write cache data.";
    }
  }
}

#ifdef IE_REDCORE
void YSPLoginManager::SetIEUrlTrusted(const UrlTrusted& trust) {
  if (trust.trust_url.empty() || trust.policy.empty())
    return;

  IInternetZoneManager* zone_manager = NULL;
  HRESULT result_handle =
      ::CoCreateInstance(CLSID_InternetZoneManager, NULL, CLSCTX_SERVER,
                         IID_IInternetZoneManager, (void**)&zone_manager);
  if (SUCCEEDED(result_handle)) {
    ZONEATTRIBUTES zone_attributes;
    result_handle =
        zone_manager->GetZoneAttributes(URLZONE_TRUSTED, &zone_attributes);
    result_handle = zone_attributes.dwFlags &= ~ZAFLAGS_REQUIRE_VERIFICATION;
    result_handle = zone_manager->SetZoneAttributes((DWORD)URLZONE_TRUSTED,
                                                    &zone_attributes);
    std::map<std::wstring, std::wstring>::const_iterator policy_iter =
        trust.policy.begin();
    for (; policy_iter != trust.policy.end(); policy_iter++) {
      DWORD policy = wcstol(policy_iter->second.c_str(), NULL, 16);
      DWORD action = wcstol(policy_iter->first.c_str(), NULL, 16);
      result_handle = zone_manager->SetZoneActionPolicy(
          URLZONE_TRUSTED, action, (BYTE*)&policy, sizeof(DWORD),
          URLZONEREG_DEFAULT);
    }
    zone_manager->Release();
  }

  IInternetSecurityManager* security_manager = NULL;
  result_handle = CoCreateInstance(
      CLSID_InternetSecurityManager, NULL, CLSCTX_INPROC_SERVER,
      IID_IInternetSecurityManager, (void**)&security_manager);

  if (SUCCEEDED(result_handle)) {
    std::vector<std::wstring>::const_iterator url_iter =
        trust.trust_url.begin();
    for (; url_iter != trust.trust_url.end(); url_iter++) {
      result_handle = security_manager->SetZoneMapping(
          URLZONE_TRUSTED, (*url_iter).c_str(), SZM_CREATE);
    }
    security_manager->Release();
  }
}

std::wstring YSPLoginManager::Utf8ToUTF16(const std::string& str) {
  std::wstring ret = L"";
  std::wstring_convert<std::codecvt_utf8_utf16<wchar_t>> converter;
  ret = converter.from_bytes(str);
  return ret;
}
#endif  // IE_REDCORE

void YSPLoginManager::OnReplaceResponseParseFailure(const std::string& error) {
  DLOG(INFO) << "YSPLoginManager::OnReplaceResponseParseFailure";
  LOG(INFO) << "Resource replace file download failure !";
}
// YSP+ } /*Resource Replace*/
// YSP+ { SingleSignOn config
void YSPLoginManager::OnSingleSignOnRequestFailure() {
  DLOG(INFO) << "YSPLoginManager::OnSingleSignOnRequestFailure";
  content::BrowserThread::PostTask(
      content::BrowserThread::IO, FROM_HERE,
      base::Bind(&net::URLRequestHttpJob::SetSsoConfigValue, ""));
}

void YSPLoginManager::OnSingleSignOnResponseParseSuccess(
    std::unique_ptr<base::DictionaryValue> response_data) {
  if (!response_data) {
    DLOG(INFO) << "YSPLoginManager::OnSingleSignOnResponseParseSuccess ignore "
                  "response.";
    return;
  }
  DLOG(INFO) << "YSPLoginManager::OnSingleSignOnResponseParseSuccess";
  OnSingleSignOnResponseParseSuccessInternal(response_data, false);
}

void YSPLoginManager::OnSingleSignOnResponseParseFailure(
    const std::string& error) {
  DLOG(INFO) << "YSPLoginManager::OnSingleSignOnResponseParseFailure (" << error
             << ")";
  content::BrowserThread::PostTask(
      content::BrowserThread::IO, FROM_HERE,
      base::Bind(&net::URLRequestHttpJob::SetSsoConfigValue, ""));
}

void YSPLoginManager::OnSingleSignOnResponseParseSuccessInternal(
    std::unique_ptr<base::DictionaryValue>& response_data,
    bool from_local) {
  DLOG(INFO) << "YSPLoginManager::OnSingleSignOnResponseParseSuccessInternal "
                "from local ? : "
             << from_local;
  if (auto_login_) {
    single_signon_info_.reset(response_data.release());
  }
  if (single_signon_info_) {
    single_signon_info_.get()->SetString(
        "data.timestamp", base::Int64ToString(base::Time::Now().ToTimeT()));
  }
  std::string singlesignon_string = "";
  if (single_signon_info_) {
    base::DictionaryValue* data_dict = nullptr;
    if (single_signon_info_->GetDictionary("data", &data_dict)) {
      base::JSONWriter::Write(*data_dict, &singlesignon_string);
      content::BrowserThread::PostTask(
          content::BrowserThread::IO, FROM_HERE,
          base::Bind(&net::URLRequestHttpJob::SetSsoConfigValue,
                     singlesignon_string));
    }
  }
}
// YSP+ } /*SingleSignOn config*/
// YSPLoginFetcherDelegate:
void YSPLoginManager::OnLoginRequestFailure(const std::string& error) {
  LOG(INFO) << "YSPLoginManager::OnLoginRequestFailure";
  if (error != "-1" && error != "404" && error != "403" && error != "400" &&
      error != "410" && error != "502" && error != "500" && error != "501" &&
      error != "503" && error != "504" && error != "505") {
    SetLoginStatus(SATUS_LOGIN_REQUEST_FAIL);
    ntp_login_status_ = false;
  }
  base::string16 error_message = GetResponseErrorMessage(error);
  std::vector<YSPLoginManagerObserver*>::iterator iter = observers_.begin();
  for (; iter != observers_.end(); ++iter) {
    (*iter)->OnLoginRequestFailure(base::UTF16ToUTF8(error_message));
  }
}

void YSPLoginManager::OnLoginResponseParseSuccess(
    std::unique_ptr<base::DictionaryValue> response_data) {
  if (!should_parse_response_) {
    LOG(INFO)
        << "YSPLoginManager::OnLoginResponseParseSuccess ignore response.";
    return;
  }
  LOG(INFO) << "YSPLoginManager::OnLoginResponseParseSuccess";
  OnLoginResponseParseSuccessInternal(response_data, false);
}

void YSPLoginManager::OnLoginResponseParseSuccessInternal(
    std::unique_ptr<base::DictionaryValue>& response_data,
    bool from_local) {
  SetLoginStatus(SATUS_LOGIN_FETCH_RECEIVED);
  DLOG(INFO)
      << "YSPLoginManager::OnLoginResponseParseSuccessInternal from local ? : "
      << from_local;
  // save to local cache
  if (!from_local) {
    if (auto_login_) {
      base::DictionaryValue* token_dict = nullptr;
      if (login_info_) {
        login_info_->GetDictionary("data.token", &token_dict);
        if (token_dict && !token_dict->empty())
          response_data->Set("data.token", std::make_unique<base::Value>(
                                               token_dict->DeepCopy()));
      }
      base::FilePath saved_path = FilePathOfLoginCache("login");
      // YSP+ { config cryptor
      std::string response_data_string;
      std::string enc_response;
      base::JSONWriter::Write(*response_data, &response_data_string);
      ConfigCryptor aes_crypted;
      LOG(INFO) << " login response: " << response_data;
      aes_crypted.EncryptString(cryptorkey::key, response_data_string,
                                &enc_response);
      if (-1 == (base::WriteFile(saved_path, enc_response.data(),
                                 enc_response.length()))) {
        DLOG(INFO) << "fail write login cache data.";
      }
      // YSP+ } /*config cryptor*/
    }
    login_info_.reset(response_data.release());
    DLOG(INFO) << "login_info_ access token: " << GetAccessTokenForLogin();
    if (!GetAccessTokenForLogin().empty() &&
        !GetRefreshTokenForLogin().empty()) {
      SaveLastId(prefs::kYSPAccessToken, GetAccessTokenForLogin());
      SaveLastId(prefs::kYSPRefreshToken, GetRefreshTokenForLogin());
    }
  }
  std::string status = GetLoginStatusCode();
  login_status_code_ = (status == "0");
  if (login_status_code_) {
    SaveLastId(prefs::kYSPLoginLastCID, cid_);
    SaveLastId(prefs::kYSPLoginLastUID, account_);
    SaveLastId(prefs::kYSPLoginLastPWD, password_);

    SetLoginStatus(SATUS_VERIFY_ACCOUNT_SUCCESS);
    // should dispatch the satus to the login page.
    SetLoginStatus(SATUS_LOGIN_FETCH_RESOURCE_START);
    // YSP+ { Fetcher resource
    GetApplicationfetcher(false);
    GetStrategyfetcher(false);
    GetTokenfetcher(false);
    GetSwafetcher(false);
    GetPcfetcher(false);
    GetGatewayApplictionFetch();
    // YSP+ } /*Fetcher resource*/
  } else {
    if (status == "E1002")
      GetAuthTokenfetcher(GetManageServer() + kLoginUploadPath, false);
    else if (status == "E1003") {
      std::vector<YSPLoginManagerObserver*>::iterator iter = observers_.begin();
      for (; iter != observers_.end(); ++iter) {
        (*iter)->OnTokenStatusChanged("TokenExpired");
      }
    } else {
      if (status == "E3002") {
        std::vector<YSPLoginManagerObserver*>::iterator iter =
            observers_.begin();
        for (; iter != observers_.end(); ++iter) {
          (*iter)->OnTokenStatusChanged("failure");
        }
      } else if (status == "E4002") {
        Logout();
      }

      SetLoginStatus(SATUS_VERIFY_ACCOUNT_FAIL);
      ntp_login_status_ = false;
      NotifyFailure();
      ClearCache();
    }

    return;
  }

  // YSP+ { SingleSignOn config
  {
    if (single_signon_status_) {
      GetSingleSignOnConfig();
      single_signon_status_ = false;
    }
  }
  // YSP+ } /* SingleSignOn config */
}

void YSPLoginManager::OnLoginResponseParseFailure(const std::string& error) {
  LOG(INFO) << "YSPLoginManager::OnLoginResponseParseFailure";
  SetLoginStatus(SATUS_LOGIN_RESPONSE_FAIL);
  ntp_login_status_ = false;
  base::string16 error_message = GetResponseErrorMessage(error);
  std::vector<YSPLoginManagerObserver*>::iterator iter = observers_.begin();
  for (; iter != observers_.end(); ++iter) {
    (*iter)->OnLoginResponseParseFailure(base::UTF16ToUTF8(error_message));
  }
}
// YSP+ { Fetcher resource
void YSPLoginManager::OnFetcherResourceRequestFailure(
    const GURL& url,
    bool auto_fecth,
    const std::string& error) {
  LOG(INFO) << "YSPLoginManager::OnFetcherResourceRequestFailure original url:"
            << url;
  if (error != "-1" && error != "404" && error != "403" && error != "400" &&
      error != "410" && error != "502" && error != "500" && error != "501" &&
      error != "503" && error != "504" && error != "505") {
    if (!auto_fecth) {
      SetLoginStatus(SATUS_LOGIN_FETCH_RESOURCE_REQUEST_FAIL);
      ntp_login_status_ = false;
    }
  }
  base::string16 error_message = GetResponseErrorMessage(error);
  std::vector<YSPLoginManagerObserver*>::iterator iter = observers_.begin();

  // ModifyPassword
  if (url.spec().find(kModifyPasswordPath) != std::string::npos) {
    OnModifyPasswordResponseParse(base::UTF16ToUTF8(error_message));
    return;
  }

  for (; iter != observers_.end(); ++iter) {
    (*iter)->OnLoginRequestFailure(base::UTF16ToUTF8(error_message));
  }
}

void YSPLoginManager::OnFetcherResourceResponseParseSuccess(
    const GURL& url,
    std::unique_ptr<base::DictionaryValue> response_data,
    bool auto_fetch) {
  if (!should_parse_response_) {
    LOG(INFO) << "YSPLoginManager::OnFetcherResourceResponseParseSuccess "
                 "ignore response. original url:"
              << url;
    return;
  }
  LOG(INFO)
      << "YSPLoginManager::OnFetcherResourceResponseParseSuccess original url:"
      << url << " auto_fetch:" << auto_fetch;
  OnFetcherResourceResponseParseSuccessInternal(url, response_data, false,
                                                auto_fetch);
}

void YSPLoginManager::OnFetcherResourceResponseParseSuccessInternal(
    const GURL& url,
    std::unique_ptr<base::DictionaryValue>& response_data,
    bool from_local,
    bool auto_fetch) {
  std::string response_status = GetResponseStatusCode(response_data);
  LOG(INFO) << "response_status: " << response_status;
  bool status_code = (response_status == "0");
  if (!auto_fetch)
    SetLoginStatus(SATUS_LOGIN_FETCH_RESOURCE_RECEIVED);
  if (status_code) {
    if (url.spec().find(kGetApplicationPath) != std::string::npos) {
      OnApplicationFetcherResponseParse(response_data, from_local, auto_fetch);
    } else if (url.spec().find(kGetConfigurationPCPath) != std::string::npos) {
      OnPcFetcherResponseParse(response_data, from_local, auto_fetch);
    } else if (url.spec().find(kGetConfigurationStrategyPath) !=
               std::string::npos) {
      OnStrategyFetcherResponseParse(response_data, from_local, auto_fetch);
    } else if (url.spec().find(kGetSsoTokenPath) != std::string::npos) {
      OnTokenFetcherResponseParse(response_data, from_local, auto_fetch);
    } else if (url.spec().find(kGetSWAPath) != std::string::npos) {
      OnSwaFetcherResponseParse(response_data, from_local, auto_fetch);
    } else if (url.spec().find(kUploadAvatar) != std::string::npos) {
      OnUploadAvatarResponseParse(response_data);
    } else if (url.spec().find(kGetUserPath) != std::string::npos) {
      OnUserFetcherResponseParse(response_data, from_local, auto_fetch);
    } else if (url.spec().find(kGetAuthTokenPath) != std::string::npos) {
      OnAutoTokenFetcherResponseParse(response_data, from_local, auto_fetch);
    } else if (url.spec().find(kSdpDevicePath) != std::string::npos) {
      OnSdpDeviceFetcherResponseParse(response_data, from_local, auto_fetch);
    } else if (url.spec().find(kGatewayApplictionPath) != std::string::npos) {
      OnGatewayApplictionResponseParse(response_data);
    } else if (url.spec().find(kModifyPasswordPath) != std::string::npos) {
      OnModifyPasswordResponseParse(response_status);
    }

    LOG(INFO) << " login_status_code_: " << login_status_code_
              << " application_status_code_: " << application_status_code_
              << " pc_status_code_: " << pc_status_code_
              << " strategy_status_code_: " << strategy_status_code_
              << " token_status_code_: " << token_status_code_
              << " swa_status_code_: " << swa_status_code_;
    if (login_status_code_ && login_info_ && application_info_ && pc_info_ &&
        strategy_info_ && token_info_ && swa_info_) {
      PrefService* prefs = g_browser_process->local_state();
      prefs->SetBoolean(prefs::kYSPFirstLogin, false);
      if (!auto_fetch)
        SetLoginStatus(SATUS_VERIFY_CONFIG_SUCCESS);  // ysp+ { auto get config
                                                      // }
      ntp_login_status_ = true;

      if (!auto_fetch) {
        std::vector<YSPLoginManagerObserver*>::iterator iter =
            observers_.begin();
        for (; iter != observers_.end(); ++iter) {
          (*iter)->OnTokenStatusChanged("TokenAvailable");
        }
      }

      UpdateLoginManagerSettings();

      if (!auto_fetch) {
        base::string16 username = GetYSPUserName();
        std::string head_image_url = GetHeadImageUrl();
        std::vector<YSPLoginManagerObserver*>::iterator iter =
            observers_.begin();
        for (; iter != observers_.end(); ++iter) {
          (*iter)->OnLoginSuccess(username, head_image_url);
        }
      }
      // YSP+ { Resource Replace
      {
        DLOG(INFO) << "YSPLoginManager::OnLoginResponseParseSuccessInternal "
                      "download online";
        base::DictionaryValue* root_dict = GetManagedResourceReplace();
        base::ListValue* resource_replace = nullptr;
        if (root_dict &&
            root_dict->GetList("resourceReplace", &resource_replace)) {
          if (resource_replace && !resource_replace->empty()) {
            for (size_t i = 0; i < resource_replace->GetSize(); ++i) {
              base::DictionaryValue* bm_dict = nullptr;
              if (resource_replace->GetDictionary(i, &bm_dict)) {
                std::string source_url, dest_url;
                bm_dict->GetString("sourceUrl", &source_url);
                bm_dict->GetString("destUrl", &dest_url);
                DLOG(INFO) << "sourceUrl: " << source_url
                           << " destUrl: " << dest_url;
                if (!dest_url.empty() && !source_url.empty()) {
                  YSPReplaceFetcher* replace_fetcher = new YSPReplaceFetcher(
                      this, g_browser_process->system_request_context());
                  if (replace_fetcher)
                    replace_fetcher->StartGetReplace(dest_url, cid_,
                                                     source_url);
                }
              }
            }
          }
        }
      }
      // YSP+ } /* Resource Replace */
    }
  } else {
    if (url.spec().find(kGetAuthTokenPath) != std::string::npos &&
        (response_status == "E1003")) {
      std::vector<YSPLoginManagerObserver*>::iterator iter = observers_.begin();
      for (; iter != observers_.end(); ++iter) {
        (*iter)->OnTokenStatusChanged("TokenExpired");
      }
    } else if (response_status == "E1002") {
      GetAuthTokenfetcher(url.spec(), false);
    } else if (url.spec().find(kModifyPasswordPath) != std::string::npos) {
      OnModifyPasswordResponseParse(response_status);
    } else {
      if (!auto_fetch) {
        SetLoginStatus(SATUS_LOGIN_RESPONSE_FAIL);  // ysp+ { auto get config }
        ntp_login_status_ = false;
        NotifyFailure();
      }
      ClearCache();
    }
  }
}

void YSPLoginManager::OnFetcherResourceResponseParseFailure(
    const GURL& url,
    bool auto_fecth,
    const std::string& error) {
  LOG(INFO)
      << "YSPLoginManager::OnFetcherResourceResponseParseFailure original url:"
      << url;
  if (!auto_fecth) {
    SetLoginStatus(SATUS_LOGIN_FETCH_RESOURCE_RESPONSE_FAIL);
    ntp_login_status_ = false;
  }
  base::string16 error_message = GetResponseErrorMessage(error);
  std::vector<YSPLoginManagerObserver*>::iterator iter = observers_.begin();

  // ModifyPassword
  if (url.spec().find(kModifyPasswordPath) != std::string::npos) {
    OnModifyPasswordResponseParse(base::UTF16ToUTF8(error_message));
    return;
  }

  for (; iter != observers_.end(); ++iter) {
    (*iter)->OnLoginResponseParseFailure(base::UTF16ToUTF8(error_message));
  }
}

void YSPLoginManager::OnAutoConfigParseSuccess(
    std::unique_ptr<base::DictionaryValue> response_data,
    bool auto_fetch) {
  if (response_data->empty())
    return;
  std::string auto_config_status = GetResponseStatusCode(response_data);
  if (auto_config_status == "E1002")
    GetAuthTokenfetcher(GetManageServer() + kAutoGetConfigPath, false);
  if (!auto_config_info_) {
    auto_config_info_.reset(response_data.release());
    return;
  }
  std::string active_id = "";
  std::string last_login_time = "";
  response_data->GetString("data.activeStrategyId", &active_id);
  if (!active_id.empty() && login_info_)
    login_info_->SetString("data.strategy.activeId", active_id);
  response_data->GetString("data.time", &last_login_time);
  if (!last_login_time.empty() && login_info_) {
    login_info_->SetString("data.user.lastLoginTime", last_login_time);
    if (!GetWatermarkEnabled()) {
      std::vector<YSPLoginManagerObserver*>::iterator iter = observers_.begin();
      for (; iter != observers_.end(); ++iter) {
        (*iter)->OnConfigDataUpdated("strategy", GetDataJSAsJSONString());
      }
    }
  }
  if (!auto_fetch) {
    GetUserfetcher(auto_fetch);
    GetApplicationfetcher(auto_fetch);
    GetStrategyfetcher(auto_fetch);
    GetSwafetcher(auto_fetch);
    GetPcfetcher(auto_fetch);
    GetTokenfetcher(auto_fetch);
  }
  base::ListValue* new_list = nullptr;
  response_data->GetList("data.versionList", &new_list);
  base::ListValue* old_list = nullptr;
  auto_config_info_->GetList("data.versionList", &old_list);
  if (new_list && old_list) {
    for (size_t i = 0; i < old_list->GetSize(); i++) {
      base::DictionaryValue* new_dict = nullptr;
      base::DictionaryValue* old_dict = nullptr;
      int type = 0, new_version = 0, old_version = 0;
      old_list->GetDictionary(i, &old_dict);
      new_list->GetDictionary(i, &new_dict);
      if (old_dict && new_dict) {
        old_dict->GetInteger("version", &old_version);
        new_dict->GetInteger("version", &new_version);
        if (old_version != new_version) {
          old_dict->SetInteger("version", new_version);
          old_dict->GetInteger("type", &type);
          if (type == 1) {
            if (login_info_)
              login_info_->SetInteger("data.strategy.version", new_version);
            GetApplicationfetcher(auto_fetch);
            GetStrategyfetcher(auto_fetch);
            GetSwafetcher(auto_fetch);
          } else if (type == 2) {
            GetPcfetcher(auto_fetch);
          } else if (type == 3) {
            GetApplicationfetcher(auto_fetch);
            GetSwafetcher(auto_fetch);
          } else if (type == 5) {
            GetUserfetcher(auto_fetch);
            GetStrategyfetcher(auto_fetch);
            GetSwafetcher(auto_fetch);
          } else if (type == 6) {
            GetSwafetcher(auto_fetch);
          }
        }
      }
    }
  }
  auto_config_info_.reset(response_data.release());
}

// YSP+ } /*Fetcher resource*/
void YSPLoginManager::AddObserver(YSPLoginManagerObserver* observer) {
  if (!observer)
    return;

  std::vector<YSPLoginManagerObserver*>::iterator iter =
      std::find(observers_.begin(), observers_.end(), observer);

  if (iter == observers_.end())
    observers_.push_back(observer);
}

void YSPLoginManager::RemoveObserver(YSPLoginManagerObserver* observer) {
  std::vector<YSPLoginManagerObserver*>::iterator iter =
      std::find(observers_.begin(), observers_.end(), observer);

  if (iter != observers_.end())
    observers_.erase(iter);
}

std::string YSPLoginManager::generateUserInfoForSettings() {
  std::string result = "";
  if (login_info_) {
    base::DictionaryValue* data_dict = nullptr;
    if (login_info_->GetDictionary("data", &data_dict)) {
      if (data_dict) {
        base::DictionaryValue return_dict;
        std::string ret;
        if (data_dict->GetString("user.id", &ret)) {
          return_dict.SetString("userId", ret);
        }
        if (data_dict->GetString("company.id", &ret)) {
          return_dict.SetString("companyId", ret);
        }

        if (data_dict->GetString("user.name", &ret)) {
          return_dict.SetString("name", ret);
        }

        if (data_dict->GetString("user.email", &ret)) {
          return_dict.SetString("email", ret);
        }
        if (data_dict->GetString("user.mobile", &ret)) {
          return_dict.SetString("mobile", ret);
        }
        if (data_dict->GetString("user.avatarPath", &ret)) {
          return_dict.SetString("avatarPath", ret);
        }
        if (data_dict->GetString("user.title", &ret)) {
          return_dict.SetString("title", ret);
        }
        if (data_dict->GetString("user.title", &ret)) {
          return_dict.SetString("title", ret);
        }
        int source = 0;
        if (data_dict->GetInteger("user.source", &source)) {
          return_dict.SetInteger("source", source);
        }
        if (data_dict->GetString("company.name", &ret)) {
          return_dict.SetString("companyName", ret);
        }

        base::string16 username = GetYSPUserName();
        base::char16 head_name = 0;
        size_t pos = 0;
        if (!username.empty()) {
          head_name = username[0];
          if ((head_name < L'a' || head_name > L'z') &&
              (head_name < L'A' || head_name > L'Z')) {
            pos = username.length() > 2 ? username.length() - 2 : 0;
            return_dict.SetString("headName", base::string16(username, pos));
          } else
            return_dict.SetString("headName", base::string16(1, head_name));
        }
        SkColor head_color =
            kAvatarBackground[(int)head_name % kAvatarBackgroundSize];
        return_dict.SetDouble("headColor", head_color);

        const std::string id("managerAddress");
        return_dict.SetString(id, GetManageServer());

        base::JSONWriter::Write(return_dict, &result);
      }
    }
  }
  return result;
}

std::string YSPLoginManager::GetValueForKey(const std::string& key) {
  std::string result = "";
  if (key.compare("loggingStatus") == 0) {
    result = ntp_login_status_ ? "100" : "-1";
  } else if (key.compare("autoLogin") == 0) {
    result = auto_login_ ? "1" : "0";
  } else if (key.compare("address") == 0) {
    result = GetManageServer();
  } else if (key.compare("domain") == 0) {
    result = cid_;
  } else if (key.compare("companyId") == 0) {
    result = GetCompanyId();
  } else if (key.compare("accountInfo") == 0) {
    result = GetUserInfoAsJSONString();
  } else if (key.compare("lastUuid") == 0) {
    result = account_;  // 用户名
  } else if (key.compare("onlyid") == 0) {
    result =
        GetStringFromDictionary(login_info_.get(), "data.user.id");  // uuid
  } else if (key.compare("password") == 0) {
    result = password_;
  } else if (key.compare("appGroupList") == 0) {
    result = GetDataJSAsJSONString();
  } else if (key.compare("firstLogin") == 0) {
    PrefService* prefs = g_browser_process->local_state();
    bool first_login = prefs->GetBoolean(prefs::kYSPFirstLogin);
    result =
        (first_login || (password_.empty() && account_.empty())) ? "1" : "0";
  } else if (key.compare("deviceId") == 0) {
    PrefService* prefs = g_browser_process->local_state();
    std::string device_id = GetRegMachineId();
    if (device_id.empty())
      device_id = prefs->GetString(prefs::kYSPDeviceID);
    if (device_id.empty()) {
      device_id = base::GenerateGUID();
      prefs->SetString(prefs::kYSPDeviceID, device_id);
    }
    result = device_id;
  } else if (key.compare("ShowConfig") == 0) {
    result = GetShowConfigAsJSONString();
  } else if (key.compare("managerAddress") == 0) {
    result = GetManageServer();
  } else if (key.compare("lastLoginInfo") == 0) {
    base::DictionaryValue data_dict;
    const std::string domain("domain");
    data_dict.SetString(domain, GetLastCID());
    const std::string userid("userid");
    data_dict.SetString(userid, GetLastUID());
    base::JSONWriter::Write(data_dict, &result);
  } else if (key.compare("pinCode") == 0) {
    result = GetUserPinKey();
  }
  return result;
}
// YSP+ { passwords AD manager
std::string YSPLoginManager::GetUserInfoForKey(int key) {
  std::string result = "";
  std::string login_name = "";
  std::string email = "";
  std::string mobile = "";
  if (login_info_) {
    login_info_->GetString("data.user.email", &email);
    login_info_->GetString("data.user.mobile", &mobile);
    login_info_->GetString("data.user.loginName", &login_name);
  }
  if (key == 1) {
    result = login_name;
  } else if (key == 2) {
    result = email;
  } else if (key == 3) {
    if (!email.empty()) {
      size_t offset = email.find('@', 0);
      if (offset != std::string::npos)
        result.assign(email, 0, offset);
    }
  } else if (key == 4) {
    result = mobile;
  }
  return result;
}

bool YSPLoginManager::GetLoginStatus() {
  return ntp_login_status_;
}

bool YSPLoginManager::GetAutoLoginStatus() {
  return login_status_ > 0;
}

// ysp+ } /*auto get config*/
std::string YSPLoginManager::GetLoginInfo() {
  std::string result = "";
  base::DictionaryValue return_dict;
  base::DictionaryValue dict;
  std::string ret;

  base::DictionaryValue* data_dict = nullptr;
  if (!login_info_ || GetManagedADProxyAuth() == nullptr)
    return std::string();

  if (login_info_->GetDictionary("data", &data_dict)) {
    base::DictionaryValue* user_dict = nullptr;
    if (data_dict && data_dict->GetDictionary("user", &user_dict)) {
      const std::string password("password");
      dict.SetString(password, password_);

      if (user_dict->GetString("email", &ret)) {
        const std::string id("email");
        dict.SetString(id, ret);
      }

      if (user_dict->GetString("id", &ret)) {
        const std::string id("id");
        dict.SetString(id, ret);
      }

      if (user_dict->GetString("mobile", &ret)) {
        const std::string id("mobile");
        dict.SetString(id, ret);
      }

      if (user_dict->GetString("loginName", &ret)) {
        const std::string id("loginName");
        dict.SetString(id, ret);
      }

      return_dict.SetDictionary("user", dict.CreateDeepCopy());
    }
  }

  return_dict.SetList("swaList", GetManagedADProxyAuth()->CreateDeepCopy());

  base::JSONWriter::Write(return_dict, &result);
  return result;
}

// YSP+ { app version
base::string16 YSPLoginManager::BuildYSPBrowserVersionString() {
  std::string version = version_info::GetYSPVersionNumber();

  std::string modifier = chrome::GetChannelName();
  if (!modifier.empty())
    version += " " + modifier;

#if defined(ARCH_CPU_64_BITS)
  version += " (64-bit)";
#endif

  return base::UTF8ToUTF16(version);
}
// YSP+ } /*app version*/

std::string YSPLoginManager::GetDeviceInfo() {
  std::string mac_address_string = GetLocalMacAdd();
  PrefService* prefs = g_browser_process->local_state();
  std::string device_id = GetRegMachineId();
  if (device_id.empty())
    device_id = prefs->GetString(prefs::kYSPDeviceID);
  if (device_id.empty()) {
    device_id = base::GenerateGUID();
    prefs->SetString(prefs::kYSPDeviceID, device_id);
  }
  std::string board_info = "";
  GetBoardInfo(&board_info);
  DLOG(INFO) << "YSPLoginManager::GetDeviceInfo, device id: " << device_id;

  base::DictionaryValue dict;
  dict.SetString("systemVersion", GetOsVersionStr());
  dict.SetString("appVersion", BuildYSPBrowserVersionString());
  dict.SetString("deviceId", device_id);
  dict.SetString("model", board_info);
  dict.SetString("architecture", /*IsWow64()*/ true ? "64" : "32");
  dict.SetString("platform", "1");

  std::string device_info;
  base::JSONWriter::Write(dict, &device_info);

  return device_info;
}

bool YSPLoginManager::HasManagedBookmarks() {
  return (GetManagedBookmarks() != nullptr);
}

base::DictionaryValue* YSPLoginManager::GetManagedBookmarks() {
  if (login_info_) {
    base::DictionaryValue* data_dict = nullptr;
    if (login_info_->GetDictionary("data", &data_dict)) {
      base::DictionaryValue* dict = nullptr;
      if (data_dict && data_dict->GetDictionary(
                           "managedBookmarks.roots.bookmark_bar", &dict)) {
        return dict;
      }
    }
  }

  return nullptr;
}
//------------------------------------------
std::string YSPLoginManager::GetUserAgent() {
  std::string user_agent;
  if (login_info_) {
    base::DictionaryValue* data_dict = nullptr;
    if (login_info_->GetDictionary("data", &data_dict)) {
      if (data_dict)
        data_dict->GetString("userAgent", &user_agent);
    }
  }

  return user_agent;
}

// ysp+ { auto get config---------------------------------------
std::string YSPLoginManager::GetAutoConfigUrl() {
  if (login_info_) {
    base::DictionaryValue* data_dict = nullptr;
    if (login_info_->GetDictionary("data", &data_dict)) {
      base::DictionaryValue* func_dict = nullptr;
      if (data_dict &&
          data_dict->GetDictionary("functionControl", &func_dict)) {
        std::string auto_config_url;
        if (func_dict &&
            func_dict->GetString("ConfigVersionUrl", &auto_config_url))
          return auto_config_url;
      }
    }
  }

  return std::string();
}

int YSPLoginManager::GetAutoConfigDelta() {
  return GetFunctionControlIntWithDefault("IntervalTime", 60);
}
// ysp+ } /*auto get config*/

base::DictionaryValue* YSPLoginManager::GetAutoUpgrade() {
  if (pc_info_) {
    base::DictionaryValue* data_dict = nullptr;
    if (pc_info_->GetDictionary("data", &data_dict)) {
      base::DictionaryValue* dict = nullptr;
      if (data_dict && data_dict->GetDictionary("autoUpgrade", &dict)) {
        return dict;
      }
    }
  }

  return nullptr;
}

// ysp+ { URL Blacklist And Whitelist
base::DictionaryValue* YSPLoginManager::GetWebsiteListEnabled() {
  if (strategy_info_) {
    base::DictionaryValue* data_dict = nullptr;
    if (strategy_info_->GetDictionary("data", &data_dict)) {
      base::DictionaryValue* dict = nullptr;
      if (data_dict &&
          data_dict->GetDictionary("isWebsiteListEnabled", &dict)) {
        return dict;
      }
    }
  }

  return nullptr;
}
// ysp+ } //URL Blacklist And Whitelist
// ysp+ { Resource Replace
base::DictionaryValue* YSPLoginManager::GetManagedResourceReplace() {
  if (pc_info_) {
    base::DictionaryValue* data_dict = nullptr;
    if (pc_info_->GetDictionary("data", &data_dict)) {
      base::DictionaryValue* dict = nullptr;
      if (data_dict && data_dict->GetDictionary("resourceOverload", &dict)) {
        return dict;
      }
    }
  }

  return nullptr;
}
// ysp + } //Resource Replace
// ysp+ { Kernel switching
base::DictionaryValue* YSPLoginManager::GetManagedKernels() {
  if (pc_info_) {
    base::DictionaryValue* data_dict = nullptr;
    if (pc_info_->GetDictionary("data", &data_dict)) {
      base::DictionaryValue* dict = nullptr;
      if (data_dict && data_dict->GetDictionary("siteCompatibility", &dict)) {
        return dict;
      }
    }
  }

  return nullptr;
}

std::string YSPLoginManager::GetDefaultCore() {
  std::string default_core;
  if (pc_info_) {
    base::DictionaryValue* data_dict = nullptr;
    if (pc_info_->GetDictionary("data", &data_dict)) {
      base::DictionaryValue* kernel_dict = nullptr;
      if (data_dict &&
          data_dict->GetDictionary("siteCompatibility", &kernel_dict)) {
        if (kernel_dict)
          kernel_dict->GetString("defaultCore", &default_core);
      }
    }
  }

  return default_core;
}
// ysp+ } //Kernel switching

// YSP+ { sangfor GM ssl
base::DictionaryValue* YSPLoginManager::GetManagedGMStream() {
  if (pc_info_) {
    base::DictionaryValue* data_dict = nullptr;
    if (pc_info_->GetDictionary("data", &data_dict)) {
      if (data_dict) {
        return data_dict;
      }
    }
  }

  return nullptr;
}
// YSP+ } //sangfor GM ssl

// YSP+ { SingleSignOn config
void YSPLoginManager::GetSingleSignOnConfig() {
  DLOG(INFO)
      << "YSPLoginManager::GetSingleSignOnConfig SingleSignOn config online";
  if (login_info_) {
    if (GetUserId().empty() || GetCompanyId().empty())
      return;
    std::string post_data;
    net::AddMultipartValueForUpload("companyId", GetCompanyId(),
                                    kMultipartBoundary, "", &post_data);
    net::AddMultipartValueForUpload("username", account_, kMultipartBoundary,
                                    "", &post_data);
    net::AddMultipartValueForUpload("password", password_, kMultipartBoundary,
                                    "", &post_data);
    net::AddMultipartFinalDelimiterForUpload(kMultipartBoundary, &post_data);
    std::vector<std::string> header_list;
    header_list.push_back("userId: " + GetUserId());
    YSPFetcherResource* single_signon_fetcher = new YSPFetcherResource(
        this, g_browser_process->system_request_context());
    if (single_signon_fetcher) {
      single_signon_fetcher->StarFetcherResource(
          net::URLFetcher::POST, GetManageServer() + kGetSsoTokenPath,
          header_list, post_data, false);
    }
  }
}

base::DictionaryValue* YSPLoginManager::GetManagedSingleSignOnConfig() {
  if (token_info_) {
    base::DictionaryValue* data_dict = nullptr;
    if (token_info_->GetDictionary("data", &data_dict)) {
      return data_dict;
    }
  }

  return nullptr;
}
// YSP+ } /*SingleSignOn config*/

// ysp+ { passwords AD manager
base::ListValue* YSPLoginManager::GetManagedADProxyAuth() {
  if (swa_info_ && !swa_info_->empty()) {
    base::ListValue* data_dict = nullptr;
    if (swa_info_->GetList("data", &data_dict)) {
      if (data_dict && !data_dict->empty()) {
        return data_dict;
      }
    }
  }

  return nullptr;
}
// ysp + } /*passwords AD manager*/

// ysp+ { AES DES and SMS4 crypt
std::string YSPLoginManager::GetEncryptionAndKey() {
  return GetCryptKey();
}
// ysp+ } //AES DES and SMS4 crypt

// YSP+ { cache encryption
bool YSPLoginManager::GetCacheEncryption() {
  return GetCacheCrypt();
}
// YSP+ }

// ysp+ { crypto http header
std::string YSPLoginManager::GetCryptoHeaderKey() {
  if (pc_info_) {
    base::DictionaryValue* data_dict = nullptr;
    if (pc_info_->GetDictionary("data", &data_dict)) {
      base::DictionaryValue* func_dict = nullptr;
      if (data_dict &&
          data_dict->GetDictionary("httpHeadersCertify", &func_dict)) {
        std::string crypt_key;
        if (func_dict && func_dict->GetString("setting.User-Agent", &crypt_key))
          return crypt_key;
      }
    }
  }

  return std::string();
}

std::string YSPLoginManager::GetUserAgentTypes() {
  if (pc_info_) {
    base::DictionaryValue* data_dict = nullptr;
    if (pc_info_->GetDictionary("data", &data_dict)) {
      base::DictionaryValue* func_dict = nullptr;
      if (data_dict &&
          data_dict->GetDictionary("httpHeadersCertify", &func_dict)) {
        std::string ua_types;
        if (func_dict && func_dict->GetString("setting.UAtypes", &ua_types))
          return ua_types;
      }
    }
  }

  return std::string();
}
// ysp+ } /*crypto http header*/
// YSP+ { lock screen time
int YSPLoginManager::GetLockScreenTime() {
  if (pc_info_) {
    base::DictionaryValue* data_dict = nullptr;
    if (pc_info_->GetDictionary("data", &data_dict)) {
      base::DictionaryValue* func_dict = nullptr;
      if (data_dict && data_dict->GetDictionary("lockScreenTime", &func_dict)) {
        int lock_screen_time = 0;
        if (func_dict && func_dict->GetInteger("value", &lock_screen_time))
          return lock_screen_time;
      }
    }
  }

  return 0;
}
// ysp+ } /*lock screen time*/
// YSP+ { window popup
base::DictionaryValue* YSPLoginManager::GetPopupSetting() {
  if (pc_info_) {
    base::DictionaryValue* data_dict = nullptr;
    if (pc_info_->GetDictionary("data", &data_dict)) {
      base::DictionaryValue* dict = nullptr;
      if (data_dict && data_dict->GetDictionary("popUpBlockerSetting", &dict)) {
        return dict;
      }
    }
  }

  return nullptr;
}
// YSP+ } /*window popup*/

// TODO(matianzhi): YSP+ { startup and home pages
base::ListValue* YSPLoginManager::GetStartupPages() {
  if (pc_info_) {
    base::ListValue* dict_list = nullptr;
    if (pc_info_->GetList("data.startPageSetting.list", &dict_list)) {
      return dict_list;
    }
  }
  return nullptr;
}

bool YSPLoginManager::isStartupPages() {
  bool defalut_main_page = false;
  if (pc_info_) {
    int status = 1;
    pc_info_->GetInteger("data.startPageSetting.value", &status);
    if (status == 2)
      defalut_main_page = true;
  }
  return defalut_main_page;
}
// YSP+ } /*startup and home pages*/

// YSP+ { doc online preview
base::DictionaryValue* YSPLoginManager::GetPreviewDocOnline() {
  if (pc_info_) {
    base::DictionaryValue* data_dict = nullptr;
    if (pc_info_->GetDictionary("data", &data_dict)) {
      base::DictionaryValue* doc_dict = nullptr;
      if (data_dict &&
          data_dict->GetDictionary("docOnlinePreview.setting", &doc_dict)) {
        return doc_dict;
      }
    }
  }
  return nullptr;
}

bool YSPLoginManager::GetPreviewDocOnlineEnable() {
  bool status = false;
  if (pc_info_) {
    base::DictionaryValue* data_dict = nullptr;
    if (pc_info_->GetDictionary("data", &data_dict)) {
      if (data_dict->GetBoolean("docOnlinePreview.value", &status)) {
        return status;
      }
    }
  }
  return status;
}
// YSP+ } /*doc online preview*/

// ysp+ { private DNS
base::DictionaryValue* YSPLoginManager::GetPrivateDNS() {
  if (pc_info_) {
    base::DictionaryValue* data_dict = nullptr;
    if (pc_info_->GetDictionary("data", &data_dict)) {
      base::DictionaryValue* doc_dict = nullptr;
      if (data_dict && data_dict->GetDictionary("privateDNS", &doc_dict)) {
        return doc_dict;
      }
    }
  }
  return nullptr;
}
// ysp+ }

void YSPLoginManager::AddHeaders() {
  if (!pc_info_)
    return;

  base::DictionaryValue* data_dict = nullptr;
  if (pc_info_->GetDictionary("data", &data_dict)) {
    base::DictionaryValue* header_dict = nullptr;
    if (data_dict &&
        data_dict->GetDictionary("httpHeadersCertify.setting", &header_dict)) {
      if (header_dict) {
        base::DictionaryValue::Iterator iter(*header_dict);
        for (; !iter.IsAtEnd(); iter.Advance()) {
          if (iter.value().is_string()) {
            std::string value;
            iter.value().GetAsString(&value);
            net::URLRequestHttpJob::AddGlobalHeader(iter.key(), value);
          }
        }
      }
    }
  }
}

bool YSPLoginManager::GetFunctionControlBool(const std::string& key,
                                             bool default_value) {
  bool functionEnabled = default_value;
  if (strategy_info_ && !key.empty()) {
    base::DictionaryValue* data_dict = nullptr;
    if (strategy_info_->GetDictionary("data", &data_dict)) {
      base::DictionaryValue* dict = nullptr;
      if (data_dict && data_dict->GetDictionary(key, &dict)) {
        if (dict) {
          dict->GetBoolean("status", &functionEnabled);
        }
      }
    }
  }

  return !functionEnabled;
}

int YSPLoginManager::GetFunctionControlInt(const std::string& key) {
  return GetFunctionControlIntWithDefault(key, 0);
}

int YSPLoginManager::GetFunctionControlIntWithDefault(const std::string& key,
                                                      int default_value) {
  int value = default_value;
  if (strategy_info_ && !key.empty()) {
    base::DictionaryValue* data_dict = nullptr;
    if (strategy_info_->GetDictionary("data", &data_dict)) {
      if (data_dict) {
        data_dict->GetInteger(key, &value);
      }
    }
  }

  return value;
}

double YSPLoginManager::GetFunctionControlDoubleWithDefault(
    const std::string& key,
    double default_value) {
  double value = default_value;
  if (strategy_info_ && !key.empty()) {
    base::DictionaryValue* data_dict = nullptr;
    if (strategy_info_->GetDictionary("data", &data_dict)) {
      if (data_dict) {
        data_dict->GetDouble(key, &value);
      }
    }
  }

  return value;
}

base::string16 YSPLoginManager::GetFunctionControlString(
    const std::string& key) {
  base::string16 value;
  if (strategy_info_ && !key.empty()) {
    base::DictionaryValue* data_dict = nullptr;
    if (strategy_info_->GetDictionary("data", &data_dict)) {
      base::DictionaryValue* dict = nullptr;
      if (data_dict && data_dict->GetDictionary("functionControl", &dict)) {
        if (dict) {
          dict->GetString(key, &value);
        }
      }
    }
  }

  return value;
}

std::vector<base::string16> YSPLoginManager::GetFunctionControlListString(
    const std::string& key) {
  std::vector<base::string16> download_info_vector;
  if (strategy_info_ && !key.empty()) {
    base::DictionaryValue* data_dict = nullptr;
    if (strategy_info_->GetDictionary("data", &data_dict)) {
      base::ListValue* list = nullptr;
      if (data_dict && data_dict->GetList(key, &list)) {
        if (list) {
          for (size_t i = 0; i < list->GetSize(); i++) {
            std::string list_key = "", str = "";
            list->GetString(i, &list_key);
            if (login_info_ && !list_key.empty()) {
              str = list_key;
              if (list_key == "?timestamp?")
                login_info_->GetString("data.user.lastLoginTime", &str);
              if (list_key == "?device_id?")
                str = GetRegMachineId();
              if (!str.empty())
                download_info_vector.push_back(base::UTF8ToUTF16(str));
            }
          }
        }
      }
    }
  }

  return download_info_vector;
}

bool YSPLoginManager::GetAddressBarEnabled() {
  return GetFunctionControlBool("isAddressBarDisabled", false);
}

bool YSPLoginManager::GetStatusBarEnabled() {
  return GetFunctionControlBool("isStateBarDisabled", false);
}

bool YSPLoginManager::GetSaveFileEnabled() {
  return GetFunctionControlBool("isSaveAsDisabled", false);
}

bool YSPLoginManager::GetPrintEnabled() {
  return GetFunctionControlBool("isPrintDisabled", false);
}

bool YSPLoginManager::GetMouseRightButtonEnabled() {
  return GetFunctionControlBool("isMouseRightDisabled", false);
}

bool YSPLoginManager::GetCutCopyEnabled() {
  return GetFunctionControlBool("isCopyDisabled", false);
}

void YSPLoginManager::LoadWindowFrameColors() {
  window_frame_color_ =
      GetFunctionControlIntWithDefault("windowFrameColor", -1);
  window_inactive_color_ = GetFunctionControlInt("windowFrameColor");
}

bool YSPLoginManager::GetWatermarkEnabled() {
  return GetFunctionControlBool("isWatermarkEnabled", false);
}

std::vector<base::string16> YSPLoginManager::GetWatermarkString() {
  if (GetWatermarkColor() == 0)
    return std::vector<base::string16>();

  return GetFunctionControlListString("isWatermarkEnabled.watermarkList");
}

uint32_t YSPLoginManager::GetWatermarkColor() {
  if (GetWatermarkEnabled())
    return 0;
  uint32_t color = (uint32_t)GetFunctionControlDoubleWithDefault(
      "isWatermarkEnabled.watermarkColor", 0);
  if (color < 1)
    return 0;

  uint32_t a = color >> 24;
  if (a < 0x05)
    a = 0x05;

  uint32_t r = (color >> 16) & 0xFF;
  uint32_t g = (color >> 8) & 0xFF;
  uint32_t b = color & 0xFF;
  color = (a << 24) | (b << 16) | (g << 8) | r;

  return color;
}

int YSPLoginManager::GetWatermarkFontSize() {
  int font_size = GetFunctionControlInt("isWatermarkEnabled.watermarkSize");
  if (font_size < 12)
    return 12;

  return font_size;
}

bool YSPLoginManager::GetScreenCaptureEnabled() {
  return GetFunctionControlBool("enableScreenCapture", false);
}

bool YSPLoginManager::GetDevToolsEnabled() {
  return GetFunctionControlBool("isDeveloperDisabled", false);
}

void YSPLoginManager::SaveLastId(const std::string& key,
                                 const std::string& value) {
  if (key.empty())
    return;

  PrefService* prefs = g_browser_process->local_state();
  if (value.empty()) {
    prefs->SetString(key, "");
  } else {
    std::string base64_string;
    base::Base64Encode(value, &base64_string);
    prefs->SetString(key, base64_string);
  }
}

std::string YSPLoginManager::GetLastId(const std::string& key) {
  PrefService* prefs = g_browser_process->local_state();
  std::string base64_string = prefs->GetString(key);
  if (base64_string.empty())
    return base64_string;

  std::string decoded_string;
  base::Base64Decode(base64_string, &decoded_string);
  return decoded_string;
}

void YSPLoginManager::InitPinKeys() {
  PrefService* prefs = g_browser_process->local_state();
  const base::ListValue* pin_list = prefs->GetList(prefs::kYSPPinKeys);
  std::vector<std::string> pin_keys;
  for (int index = 0; (size_t)index < pin_list->GetSize(); index++) {
    const base::Value* key_value = NULL;
    if (pin_list->Get(index, &key_value)) {
      pin_keys.push_back(key_value->GetString());
    }
  }
  YspCryptoSingleton::GetInstance()->SetPinKeys(pin_keys);
}

void YSPLoginManager::UpdatePinKey(const std::string& value) {
  YspCryptoSingleton::GetInstance()->UpdateCurrentPinKey(value);
  base::ListValue list_value;
  list_value.AppendStrings(YspCryptoSingleton::GetInstance()->GetPinKeys());
  PrefService* prefs = g_browser_process->local_state();
  prefs->Set(prefs::kYSPPinKeys, list_value);
}

std::string YSPLoginManager::GetUserPinKey() {
  return YspCryptoSingleton::GetInstance()->GetCurrentPinKey();
}

std::string YSPLoginManager::SHA256HashString(const std::string& text) {
  return YspCryptoSingleton::GetInstance()->SHA256HashString(text);
}

std::string YSPLoginManager::GetLastCID() {
  return GetLastId(prefs::kYSPLoginLastCID);
}

std::string YSPLoginManager::GetLastUID() {
  return GetLastId(prefs::kYSPLoginLastUID);
}
//-------------------------------------
base::string16 YSPLoginManager::GetManagedBookmarksFolderName() {
  return GetFunctionControlString("bookmarkFolderName");
}

bool YSPLoginManager::GetWebsiteListStatus() {
  bool status = false;
  if (strategy_info_) {
    base::DictionaryValue* data_dict = nullptr;
    if (strategy_info_->GetDictionary("data", &data_dict)) {
      data_dict->GetBoolean("isWebsiteListEnabled.status", &status);
    }
  }
  return status;
}

std::string YSPLoginManager::GetWebsiteListType() {
  std::string type = "";
  if (strategy_info_) {
    base::DictionaryValue* data_dict = nullptr;
    if (strategy_info_->GetDictionary("data", &data_dict)) {
      data_dict->GetString("isWebsiteListEnabled.type", &type);
    }
  }
  return type;
}

bool YSPLoginManager::GetLoadURLAllowed(const GURL& url) {
  bool status = GetWebsiteListStatus();
  if (!status)
    return true;

  if (url == GURL(url::kAboutBlankURL))
    return true;

  if (!(url.SchemeIs(url::kAboutScheme) || url.SchemeIs(url::kBlobScheme) ||
        url.SchemeIs(url::kContentScheme) || url.SchemeIs(url::kDataScheme) ||
        url.SchemeIs(url::kFileScheme) ||
        url.SchemeIs(url::kFileSystemScheme) || url.SchemeIs(url::kFtpScheme) ||
        url.SchemeIs(url::kGopherScheme) || url.SchemeIs(url::kHttpScheme) ||
        url.SchemeIs(url::kHttpsScheme) ||
        url.SchemeIs(url::kJavaScriptScheme) ||
        url.SchemeIs(url::kMailToScheme) || url.SchemeIs(url::kWsScheme) ||
        url.SchemeIs(url::kWssScheme))) {
    return true;
  }
  return false;
  // if(URLInBacklist(url))
  //  return false;
  //
  // return URLInWhitelist(url);
}

// ManagedSettingsProvider:
base::Value* YSPLoginManager::GetManagedValue(const std::string& path) {
  // if(path.compare("browser.show_home_button") == 0) {
  //  DLOG(INFO) << "YSPLoginManager::GetManagedValue!!!!";
  //  return new base::FundamentalValue(false);
  //}
  if (login_info_) {
    base::DictionaryValue* data_dict = nullptr;
    if (login_info_->GetDictionary("data", &data_dict)) {
      base::DictionaryValue* dict = nullptr;
      if (data_dict && data_dict->GetDictionary("managedSettings", &dict)) {
        base::Value* value = nullptr;
        if (dict) {
          // DLOG(INFO) << "YSPLoginManager::GetManagedValue: " << path;
          dict->Get(path, &value);
        }
        return value;
      }
    }
  }

  return nullptr;
}

bool YSPLoginManager::URLInBacklist(const GURL& url) {
  static const std::string path = "functionControl.urlBlacklist";
  if (EmptyPolicyList(path))
    return false;

  return URLInPolicylist(path, url);
}

bool YSPLoginManager::URLInWhitelist(const GURL& url) {
  static const std::string path = "functionControl.urlWhitelist";
  if (EmptyPolicyList(path))
    return true;

  return URLInPolicylist(path, url);
}

bool YSPLoginManager::URLInPolicylist(const std::string& policy_path,
                                      const GURL& url) {
  if (login_info_) {
    base::DictionaryValue* data_dict = nullptr;
    if (login_info_->GetDictionary("data", &data_dict)) {
      base::ListValue* list = nullptr;
      if (data_dict && data_dict->GetList(policy_path, &list)) {
        if (list == nullptr || list->empty())
          return false;

        const base::Value urlValue(url.host());
        bool in = (list->Find(urlValue) != list->end());
        // DLOG(INFO) << "YSPLoginManager::URLInPolicylist host: "
        //  << url.host()
        //  << (in ? " in " : " not in ")
        //  << "policy: "
        //  << policy_path;
        return in;
      }
    }
  }
  return false;
}

bool YSPLoginManager::GetDownloadFileAllowed(const base::FilePath& file_path) {
  if (file_path.empty())
    return true;

  if (FileTypeInBlacklist(file_path))
    return false;

  return true;
}

bool YSPLoginManager::FileTypeInBlacklist(const base::FilePath& file_path) {
  return FileTypeInPolicylist("isFileBlackListEnabled.blacklistList",
                              file_path);
}

bool YSPLoginManager::FileTypeInPolicylist(const std::string& policy_path,
                                           const base::FilePath& file_path) {
  if (!strategy_info_) {
    return false;
  }

  base::DictionaryValue* data_dict = nullptr;
  base::ListValue* list = nullptr;
  if (!strategy_info_->GetDictionary("data", &data_dict) ||
      !data_dict || !data_dict->GetList(policy_path, &list) ||
      !list || list->empty()) {
    return false;
  }

  base::FilePath::StringType dot_char = FILE_PATH_LITERAL(".");
  for (size_t i = 0; i < list->GetSize(); i++) {
    base::FilePath::StringType key;
    list->GetString(i, &key);
    if (key.find(dot_char, 0) != 0) {
      key = dot_char + key;
    }
    if (file_path.MatchesExtension(key))
      return true;
  }

  return false;
}

bool YSPLoginManager::EmptyPolicyList(const std::string& policy_path) {
  if (login_info_) {
    base::DictionaryValue* data_dict = nullptr;
    if (login_info_->GetDictionary("data", &data_dict)) {
      bool status = true;
      data_dict->GetBoolean("isWebsiteListEnabled.status", &status);
      base::ListValue* list = nullptr;
      if (data_dict && data_dict->GetList(policy_path, &list)) {
        return (status || list == nullptr || list->empty());
      }
    }
  }
  return true;
}

#if defined(OS_WIN)
typedef bool(__stdcall* InternetSetOpt)(_In_ HINTERNET,
                                        _In_ DWORD,
                                        _In_ LPVOID,
                                        _In_ DWORD);
InternetSetOpt MyInternetSetOpt;

void YSPLoginManager::UpdateProxySettings() {
  // 0: bypass proxy settings
  // 1: use managed proxy
  // 2: use user proxy
  // 3: not use proxy
  int proxy_switch = GetFunctionControlIntWithDefault("proxySwitch", 0);

  if (proxy_switch == 0)
    return;

  base::string16 proxy_server = GetFunctionControlString("proxyServer");
  base::string16 proxy_except = GetFunctionControlString("proxyException");

  bool enabled = (proxy_switch != 3);

  if (proxy_switch == 2) {
    proxy_server = base::string16();
    proxy_except = base::string16();
  }

  BOOL result = SetProxyInfo(enabled, proxy_server, proxy_except);
  if (!result) {
    DLOG(INFO) << "SetProxyInfo is failure!";
    return;
  }

  HMODULE wininet_module;
  bool proxy_tmp = false;
  wininet_module = LoadLibraryA("wininet.dll");
  MyInternetSetOpt =
      (InternetSetOpt)GetProcAddress(wininet_module, "InternetSetOptionW");
  if (MyInternetSetOpt != NULL) {
    proxy_tmp =
        MyInternetSetOpt(NULL, INTERNET_OPTION_SETTINGS_CHANGED, NULL, 0);
    if (!proxy_tmp)
      DLOG(INFO) << "UpdateProxySettings InternetSetOptionW is failure!";
  }
  // INTERNET_PER_CONN_OPTION option;
  // if (!InternetSetOption(NULL, INTERNET_OPTION_SETTINGS_CHANGED, NULL, 0)) {
  // LOG(INFO) << "InternetSetOption failure !";
  //}
}

// return download target file path
base::string16 YSPLoginManager::GetPACFileURL() {
  return GetFunctionControlString("proxyAutoConfigFile");
}

void YSPLoginManager::UpdatePACSettings(base::FilePath& pac_file) {
  DLOG(INFO) << "YSPLoginManager::UpdatePACSettings: '" << pac_file.value()
             << "'";
  BOOL result = FALSE;
  // 0: bypass auto proxy settings
  // 1: use managed auto proxy
  // 2: use user auto proxy
  // 3: not use auto proxy
  int pac_switch = GetFunctionControlIntWithDefault("proxyAutoConfigSwitch", 0);
  switch (pac_switch) {
    case 0:
      return;
    case 1:
      result = SetPACInfo(true, pac_file.value());
      break;
    case 2:
      result = SetPACInfo(true, base::string16());
      break;
    case 3:
      result = SetPACInfo(false, base::string16());
      break;
  }
  if (!result) {
    DLOG(INFO) << "SetPACInfo is failure!";
    return;
  }
  HMODULE wininet_module;
  bool pac_tmp = false;
  wininet_module = LoadLibraryA("wininet.dll");
  MyInternetSetOpt =
      (InternetSetOpt)GetProcAddress(wininet_module, "InternetSetOptionW");
  if (MyInternetSetOpt != NULL) {
    pac_tmp = MyInternetSetOpt(NULL, INTERNET_OPTION_SETTINGS_CHANGED, NULL, 0);
    if (!pac_tmp)
      DLOG(INFO) << "UpdatePACSettings InternetSetOptionW is failure!";
  }
  // if (!InternetSetOption(NULL, INTERNET_OPTION_SETTINGS_CHANGED, NULL, 0)) {
  // LOG(INFO) << "InternetSetOption failure !";
  //}
}
#endif  // OS_WIN

#ifdef IE_REDCORE

std::vector<ActivexDownloadInfo> YSPLoginManager::GetActivexDownloadInfo() {
  std::vector<ActivexDownloadInfo> download_info_vector;
  if (pc_info_.get() == NULL)
    return download_info_vector;
  base::DictionaryValue* data_dict = NULL;
  if (pc_info_->GetDictionary("data", &data_dict) == false)
    return download_info_vector;
  base::ListValue* downloadList = NULL;
  if (data_dict && data_dict->GetList("distributedSoftware", &downloadList)) {
    size_t i = 0;
    for (; i < downloadList->GetSize(); i++) {
      base::DictionaryValue* temp = NULL;
      if (downloadList->GetDictionary(i, &temp) == false)
        continue;
      ActivexDownloadInfo info;
      temp->GetString("fileName", &info.filename);
      temp->GetString("displayName", &info.display_name);
      temp->GetString("downLoadUrl", &info.download_url);
      temp->GetString("md5", &info.md5);
      download_info_vector.push_back(info);
    }
  }
  return download_info_vector;
}

UrlTrusted YSPLoginManager::GetUrlTrusted() {
  UrlTrusted trust;
  if (pc_info_.get() == NULL)
    return trust;

  base::DictionaryValue* data_dict = NULL;
  if (pc_info_->GetDictionary("data", &data_dict) == false)
    return trust;
  base::DictionaryValue* url_trusted_value = NULL;
  if (data_dict &&
      data_dict->GetDictionary("trustedSiteLevel", &url_trusted_value)) {
    base::ListValue* url_array = NULL;
    if (url_trusted_value->GetList("url", &url_array) == false)
      return trust;

    base::string16 url_temp = base::string16();
    for (size_t i = 0; i < url_array->GetSize(); i++) {
      url_array->GetString(i, &url_temp);
      trust.trust_url.push_back(url_temp);
    }

    base::DictionaryValue* policy_value = NULL;
    if (url_trusted_value->GetDictionary("policy", &policy_value) == false)
      return trust;

    base::DictionaryValue::Iterator iter(*policy_value);
    while (iter.IsAtEnd() == false) {
      std::string key = iter.key();
      std::wstring value = base::string16();
      iter.value().GetAsString(&value);
      trust.policy.insert(std::make_pair(Utf8ToUTF16(key), value));
      iter.Advance();
    }
  }

  return trust;
}

bool YSPLoginManager::GetUserAgent(std::string* ua_types,
                                   std::string* ua_string) {
  bool ret = false;
  if (pc_info_) {
    base::DictionaryValue* data_dict = nullptr;
    if (pc_info_->GetDictionary("data", &data_dict)) {
      base::DictionaryValue* func_dict = nullptr;
      if (data_dict &&
          data_dict->GetDictionary("httpHeadersCertify", &func_dict)) {
        std::string crypt_key;
        if (func_dict && ua_types) {
          ret = func_dict->GetString("setting.UAtypes", ua_types);
          if (!ret)
            return ret;
        }
        if (func_dict && ua_string)
          ret = func_dict->GetString("setting.User-Agent", ua_string);
      }
    }
  }

  return ret;
}
#endif

void YSPLoginManager::ReportURLLoading(const GURL& url) {
  if (!login_info_ || !GetUSReportEnabled())
    return;

  if (url.SchemeIs("ep"))
    return;

  // init report fetcher
  if (!report_fetcher_) {
    report_fetcher_ =
        new YSPUSReportFetcher(g_browser_process->system_request_context());
  }

  if (!report_fetcher_)
    return;

  std::string device_id = GetRegMachineId();
  if (device_id.empty()) {
    PrefService* prefs = g_browser_process->local_state();
    device_id = prefs->GetString(prefs::kYSPDeviceID);
  }
  report_fetcher_->StartReport(
      GetManageServer(), GetCompanyId(),
      GetString16FromDictionary(login_info_.get(), "data.company.name"),
      GetStringFromDictionary(login_info_.get(), "data.user.departmentId"),
      GetString16FromDictionary(login_info_.get(), "data.user.department"),
      GetStringFromDictionary(login_info_.get(), "data.user.id"),
      GetYSPUserName(), device_id, url.spec());
}

bool YSPLoginManager::GetUSReportEnabled() {
  return GetFunctionControlBool("enableStatistic", true);
}

void YSPLoginManager::set_auto_login(bool auto_login) {
  PrefService* prefs = g_browser_process->local_state();
  prefs->SetBoolean(prefs::kYSPAutoLogin, auto_login);
  auto_login_ = auto_login;
  if (!auto_login_)
    ClearCache();
}

void YSPLoginManager::NotifyFailure() {
  base::string16 msg = GetLoginErrorMessage();
  std::vector<YSPLoginManagerObserver*>::iterator iter = observers_.begin();
  for (; iter != observers_.end(); ++iter) {
    (*iter)->OnLoginFailure(msg);
  }
}

void YSPLoginManager::NotifyConfigureUpdate(const std::string& type,
                                            const std::string& data) {
  std::vector<YSPLoginManagerObserver*>::iterator iter = observers_.begin();
  for (; iter != observers_.end(); ++iter) {
    (*iter)->OnConfigDataUpdated(type, data);
  }
}

//获取服务器时间差
int YSPLoginManager::GetTimeDifference() {
  double time_diff = 0;
  PrefService* prefs = g_browser_process->local_state();
  time_diff = prefs->GetDouble(prefs::kYSPTimeDifference);
  return time_diff;
}

void YSPLoginManager::UpdateLoginManagerSettings() {
  // crypto http header
  std::string crypt_header_key = GetCryptoHeaderKey();
  std::string ua_types = GetUserAgentTypes();
  if (ua_types == "3") {
    YspCryptoHeader::GetInstance()->Init((ua_types + crypt_header_key));
    YspCryptoHeader::GetInstance()->SetTimeDiff(GetTimeDifference());
  }

  AddHeaders();
  LoadWindowFrameColors();
#ifdef IE_REDCORE
  UpdateProxySettings();
  UrlTrusted trust = YSPLoginManager::GetInstance()->GetUrlTrusted();
  SetIEUrlTrusted(trust);
#endif
}
