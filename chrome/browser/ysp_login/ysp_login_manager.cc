//ysp+ { login }
//#prama warning(disable:4566)
#ifdef REDCORE
#include "chrome/browser/ysp_login/ysp_login_manager.h"
#if defined(OS_MACOSX)
#include "chrome/browser/ysp_login/mac/ysp_sysinfo.h"
#include "base/sys_info.h"
#endif

#include <utility>
#if defined(OS_WIN)
#include <wininet.h>
#include <Iphlpapi.h>
#include <comdef.h>
#include "setupapi.h"
#include <urlmon.h>
#include "base/win/registry.h"
#endif /*defined(OS_WIN)*/
#include "base/values.h"
#include "base/path_service.h"
#include "base/files/file_path.h"
#include "base/files/file_util.h"
#include "base/json/json_file_value_serializer.h"
#include "base/strings/utf_string_conversions.h"
#include "chrome/common/chrome_paths.h"
#include "chrome/browser/browser_process.h"
#include "content/public/browser/browser_thread.h"
#include "components/prefs/pref_service.h"
#include "chrome/common/pref_names.h"
#include "base/native_library.h"
#include "base/json/json_writer.h"
#include "base/guid.h"
#include "base/base64.h"
#include "net/url_request/url_request_http_job.h"
#include "components/metrics/metrics_pref_names.h"
#include "base/strings/string_util.h"
#include "ui/base/l10n/l10n_util.h"
#include "chrome/grit/generated_resources.h"
#ifdef IE_REDCORE
#include "chrome/browser/ui/ysp_download_activex_infobar_delegate.h"
#endif
#include <openssl/aes.h> //YSP+ { config cryptor }
#include "base/json/json_reader.h" //YSP+ { config cryptor }
#include <locale>
#ifdef IE_REDCORE
#include <codecvt>
#endif
#include "components/version_info/version_info.h" //YSP+ { app version }
#include "chrome/common/channel_info.h" //YSP+ { app version }
#include "base/win/windows_version.h" //YSP+ { system version }

#include "crypto/ysp_crypto_header.h"  //ysp+ { crypto http header }
#include "components/version_info/version_info.h" //ysp+ { crypto http header }
#include "base/strings/string_number_conversions.h" //YSP+ { SingleSignOn config }
#include "net/base/mime_util.h" //YSP+ { Fetcher resource }

#include "chrome/browser/themes/theme_properties.h"

#include "chrome/browser/ysp_login/ysp_us_report_fetcher.h"

#ifdef IE_REDCORE
#pragma comment(lib, "iphlpapi.lib")
#pragma comment (lib,"netapi32")
#pragma comment(lib, "setupapi")
#endif /*IE_REDCORE*/

const SkColor kAvatarBackground[] = {
  0xffe13737,
  0xfffc7936,
  0xffffad36,
  0xff71ad2c,
  0xff2dc6e0,
  0xff2590ee,
  0xff5169fa,
  0xff8158ec,
  0xffb757df
};
const int kAvatarBackgroundSize = sizeof(kAvatarBackground) / sizeof(SkColor);

UrlTrusted::UrlTrusted()
{
}

UrlTrusted::~UrlTrusted()
{
}
UrlTrusted::UrlTrusted(const UrlTrusted& other) = default;

namespace {
  //YSP+ { Fetcher resource
  const char kLoginUploadPath[] = "/client/v3/login"; //登录接口
  const char KGetUserPath[] = "/client/v3/user/"; //获取企业和用户信息
  const char kGetApplicationPath[] = "/client/v3/strategy/application/"; //获取用户应用
  const char kGetConfigurationStrategyPath[] = "/client/v3/strategy/configuration/"; //获取用户功能配置
  const char kGetSsoTokenPath[] = "/client/v3/sso/token"; //获取SSO的token
  const char kGetSWAPath[] = "/client/v3/swa/"; //获取SWA配置
  const char kGetConfigurationPCPath[] = "/client/v3/configuration/pc/"; //获取PC设置
  const char kAutoGetConfigPath[] = "/client/v3/version/"; //获取版本信息
  const char kGetAuthTokenPath[] = "/client/v3/auth/token"; //更新access_token
  const char kSdpDevicePath[] = "/client/v3/sdp/device/"; //获取/删除用户设备
  const char kModifyPasswordPath[] = "/client/v3/device/password"; //修改密码接口
  const char kApplictionStatusPath[] = "/client/v3/strategy/application/market/";  //启用/关闭应用市场应用
  const char kGatewayApplictionPath[] = "/client/v3/gateway/application/domain/";  //获取需要敲门的应用
  //const char kUploadContentType[] = "multipart/form-data";
  const char kMultipartBoundary[] = "------xdcehKrkohmfeHiyreFnWifghoDl------";
  //YSP+ } /*Fetcher resource*/

  YSPLoginManager* g_instance = nullptr;
  //YSP+ { Resource Replace
  static std::string pathParse(const std::string& path_url) {
#if defined(OS_WIN)
    std::string filePath;
    std::string path_url_parse;
    int i;

    std::string u16_url = path_url;
    int offset = u16_url.find('/', 0);
    filePath.assign(u16_url, offset + 2, (u16_url.length() - offset + 2));
    i = filePath.find('/');
    for (; i != -1; i = filePath.find('/'))
      filePath.replace(i, 1, "\\");

    int ColonOffset = filePath.find(':');
    if (filePath.find(':') != std::string::npos) {
      path_url_parse.append(filePath, 0, ColonOffset);
      path_url_parse.append(filePath, ColonOffset + 1, filePath.length() - ColonOffset - 1);
    }
    else
      path_url_parse.assign(filePath);
    //LOG(INFO) << "Path_url:" << path_url << " FilePath:" << filePath;

    return path_url_parse;
#else
        return path_url;
#endif
  }
  //YSP+ } /*Resource Replace*/
  base::FilePath FilePathOfLoginCache(const std::string& companyId) {
    base::FilePath path;
    if (companyId.empty())
      return path;

    base::PathService::Get(chrome::DIR_USER_DATA, &path);
    path = path.Append(FILE_PATH_LITERAL("Default"));
    path = path.Append(FILE_PATH_LITERAL("Cache"));
    path = path.AppendASCII(companyId + ".dat");
    return path;
  }
  //YSP+ { Resource Replace
  base::FilePath FileDirectoryPathOfReplaceCache(const std::string& path_url) {
    base::FilePath path;
    if (path_url.empty()) {
      return path;
    }
    std::string tmp_path = pathParse(path_url);

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
    std::string tmp_path = pathParse(path_url);

    base::PathService::Get(chrome::DIR_USER_DATA, &path);
    path = path.AppendASCII("Default");
    path = path.AppendASCII("Resource");
    path = path.AppendASCII(tmp_path.c_str());

    return path;
  }
  //YSP+ } /*Resource Replace*/
  //std::string GetOsVersionStr() {
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
  //YSP+ { system version
  std::string GetOsVersionStr() {
    std::string sysVersion = "";
#if defined(OS_WIN)
    base::win::Version version = base::win::GetVersion();
    if (version == base::win::VERSION_PRE_XP)
      sysVersion = "Not supported";
    else if (version == base::win::VERSION_XP)
      sysVersion = "Windows XP";
    else if (version == base::win::VERSION_SERVER_2003)
      sysVersion = "Windows XP";
    else if (version == base::win::VERSION_VISTA)
      sysVersion = "Winsows vista";
    else if (version == base::win::VERSION_WIN7)
      sysVersion = "Windows 7";
    else if (version == base::win::VERSION_WIN8)
      sysVersion = "Windows 8";
    else if (version == base::win::VERSION_WIN8_1)
      sysVersion = "Windows 8.1";
    else if (version == base::win::VERSION_WIN10)
      sysVersion = "Windows 10";
    else if (version == base::win::VERSION_WIN10_TH2)
      sysVersion = "Windows 10";
    else if (version == base::win::VERSION_WIN_LAST)
      sysVersion = "unknown";
#elif defined(OS_MACOSX)
        sysVersion = "OS X " + base::SysInfo::OperatingSystemVersion();
#else
        DLOG(WARNING) << "Unknown operating system";
#endif
        return sysVersion;
  }

#if defined(OS_WIN)
  //YSP+ } /*system version*/
  void FormatString(std::string& strRetValue, const char* pwsFormat, ...)
  {
    char szBuffer[512] = { 0 };
    va_list args;

    va_start(args, pwsFormat);
    int nBuf = _vsnprintf(szBuffer, sizeof(szBuffer) / sizeof(szBuffer[0]), pwsFormat, args);
    //nBuf;

    if ((unsigned int)nBuf < strlen(szBuffer) && nBuf != -1)
    {
      strRetValue = szBuffer;
    }
    else
    {
      int i = 1;
      char* pBuffer = NULL;

      do
      {
        if (pBuffer)
        {
          delete[] pBuffer;
          pBuffer = NULL;
        }

        if (++i >= 100)
        {
          break;
        }

        //
        pBuffer = new char[512 * i];
        memset(pBuffer, 0, 512 * i * sizeof(char));

        nBuf = _vsnprintf(pBuffer, 512 * i, pwsFormat, args);
      } while (nBuf >= 512 * i || nBuf == -1);

      if (pBuffer)
      {
        strRetValue = pBuffer;
        delete[] pBuffer;
        pBuffer = NULL;
      }
    }

    va_end(args);
  }

  BOOL IsPhysicalAdapter(const char* lpszAdpName, DWORD& dwMediaSubType)
  {
    BOOL bRet = FALSE;
    UUID guid;
    unsigned char szGuid[] = "4D36E972-E325-11CE-BFC1-08002BE10318";
    ::UuidFromStringA(szGuid, &guid);
    HDEVINFO hDevInfo = ::SetupDiGetClassDevsA(&guid, NULL, NULL, DIGCF_PRESENT);  // get the device info
    if (hDevInfo == INVALID_HANDLE_VALUE)
    {
      return bRet;
    }
    //
    unsigned char szNetCardKey[] = "System\\CurrentControlSet\\Control\\Network\\{4D36E972-E325-11CE-BFC1-08002BE10318}";
    char szDataBuf[MAX_PATH + 1] = { 0 };
    DWORD dwDataLen = MAX_PATH;
    DWORD dwType = REG_SZ;
    HKEY hLocalNet = NULL;
    std::string sPnpInstalceID;

    sprintf_s(szDataBuf, "%s\\%s\\Connection", szNetCardKey, lpszAdpName);
    if (::RegOpenKeyExA(HKEY_LOCAL_MACHINE, szDataBuf, 0, KEY_READ, &hLocalNet) != ERROR_SUCCESS)
    {
      SetupDiDestroyDeviceInfoList(hDevInfo);
      return bRet;
    }
    if (::RegQueryValueExA(hLocalNet, "PnpInstanceID", 0, &dwType, (BYTE*)szDataBuf, &dwDataLen))
    {
      SetupDiDestroyDeviceInfoList(hDevInfo);
      ::RegCloseKey(hLocalNet);
      return bRet;
    }
    sPnpInstalceID = szDataBuf;

    dwType = REG_DWORD;
    dwDataLen = sizeof(DWORD);
    ::RegQueryValueExA(hLocalNet, "MediaSubType", 0, &dwType, (BYTE*)&dwMediaSubType, &dwDataLen);

    char DeviceId[MAX_PATH] = { 0 };
    ULONG dwBusNum = (ULONG)-1;
    SP_DEVINFO_DATA DeviceInfoData;
    DeviceInfoData.cbSize = sizeof(SP_DEVINFO_DATA);
    for (int i = 0; ::SetupDiEnumDeviceInfo(hDevInfo, i, &DeviceInfoData); ++i)   //enum device info
    {
      ZeroMemory(DeviceId, sizeof(DeviceId));
      dwBusNum = (ULONG)-1;

      ::SetupDiGetDeviceInstanceIdA(hDevInfo, &DeviceInfoData, DeviceId, MAX_PATH, NULL);

      if (!::SetupDiGetDeviceRegistryPropertyA(hDevInfo, &DeviceInfoData, SPDRP_BUSNUMBER, NULL, (PBYTE)&dwBusNum,
        sizeof(ULONG), NULL))
      {
        continue;
      }
      if (sPnpInstalceID.compare(DeviceId) == 0 && dwBusNum != (ULONG)-1)
      {
        bRet = TRUE;
        break;
      }
    }
    ::RegCloseKey(hLocalNet);
    SetupDiDestroyDeviceInfoList(hDevInfo);
    return bRet;
  }

  // std::string GetResolution() {
  //   std::string strScreen = "";
  //   FormatString(strScreen, "%dx%d", GetSystemMetrics(SM_CXSCREEN), GetSystemMetrics(SM_CYSCREEN));
  //   return strScreen;
  // }
#endif //OS_WIN
  std::string GetLocalMacAdd() {
    static std::string strMac;
    if (!strMac.empty()) {
      return strMac;
    }
#if defined(OS_WIN)
    typedef ULONG(WINAPI *PFN_GetAdaptersInfo)(__out_bcount_opt(*SizePointer) PIP_ADAPTER_INFO AdapterInfo, __inout PULONG SizePointer);
    // no logic, just for compile
    base::NativeLibraryOptions opt;
    const base::NativeLibraryOptions& options = opt;
    base::NativeLibrary hDll = base::LoadNativeLibraryWithOptions(base::FilePath(L"Iphlpapi.dll"), options, nullptr);
    if (hDll) {
      PFN_GetAdaptersInfo pfn = (PFN_GetAdaptersInfo)base::GetFunctionPointerFromNativeLibrary(hDll, "GetAdaptersInfo");
      if (pfn) {
        PIP_ADAPTER_INFO pAdapterInfo = NULL;
        PIP_ADAPTER_INFO pAdapter = NULL;
        DWORD dwRetVal = 0;
        ULONG ulOutBufLen;
        pAdapterInfo = (PIP_ADAPTER_INFO)malloc(sizeof(IP_ADAPTER_INFO));
        ulOutBufLen = sizeof(IP_ADAPTER_INFO);
        if (pfn(pAdapterInfo, &ulOutBufLen) == ERROR_BUFFER_OVERFLOW)
        {
          free(pAdapterInfo);
          pAdapterInfo = (PIP_ADAPTER_INFO)malloc(ulOutBufLen);
        }

        BYTE szSystemInfo[100] = { 0 };
        if ((dwRetVal = pfn(pAdapterInfo, &ulOutBufLen)) == NO_ERROR)
        {
          pAdapter = pAdapterInfo;

          while (pAdapter != NULL && pAdapter->AddressLength < 100U)
          {
            DWORD dwMediaSubType = 0;
            if (IsPhysicalAdapter(pAdapter->AdapterName, dwMediaSubType))
            {
              CopyMemory(szSystemInfo, pAdapter->Address, pAdapter->AddressLength);

              if (szSystemInfo[0] +
                szSystemInfo[1] +
                szSystemInfo[2] +
                szSystemInfo[3] +
                szSystemInfo[4] +
                szSystemInfo[5] != 0)
              {
                FormatString(strMac, "%02x%02x%02x%02x%02x%02x",
                  szSystemInfo[0],
                  szSystemInfo[1],
                  szSystemInfo[2],
                  szSystemInfo[3],
                  szSystemInfo[4],
                  szSystemInfo[5]);

                if (dwMediaSubType != 0x2)
                  break;
              }
            }
            pAdapter = pAdapter->Next;
          }
        }
        free(pAdapterInfo);
      }
      base::UnloadNativeLibrary(hDll);
    }
#endif /*OS_WIN*/
    return strMac;
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
  void GetBoardInfo(std::string* BoardInfo)
  {
#if defined(OS_WIN)
    HKEY RootKey;           //主键
    LPCTSTR IpSubKey;         //子键名称
    std::wstring  brandBuff;             //品牌
    std::wstring  modelBuff;             //型号

    RootKey = HKEY_LOCAL_MACHINE;
    IpSubKey = L"HARDWARE\\DESCRIPTION\\System\\BIOS";
    base::win::RegKey key(RootKey, IpSubKey, KEY_READ | KEY_WOW64_64KEY);
    key.ReadValue(L"SystemManufacturer", &brandBuff);
    key.ReadValue(L"SystemProductName", &modelBuff);
    BoardInfo->clear();
    BoardInfo->assign(base::UTF16ToUTF8(brandBuff) + "_" + base::UTF16ToUTF8(modelBuff));
//#endif /*IE_REDCORE*/
#elif defined(OS_MACOSX)
        BoardInfo->assign(base::SysInfo::HardwareModelName());
#else
        DLOG(WARNING) << "Uknown operating system";
#endif
        return;
  }
#if defined(OS_WIN)
  //判断系统位数，64位返回TRUE，32位返回FALSE
  // BOOL IsWow64()
  // {
  //   typedef BOOL(WINAPI *LPFN_ISWOW64PROCESS) (HANDLE, PBOOL);
  //   LPFN_ISWOW64PROCESS fnIsWow64Process;
  //   BOOL bIsWow64 = FALSE;
  //   fnIsWow64Process = (LPFN_ISWOW64PROCESS)GetProcAddress(GetModuleHandle(L"kernel32"), "IsWow64Process");
  //   if (NULL != fnIsWow64Process)
  //   {
  //     fnIsWow64Process(GetCurrentProcess(), &bIsWow64);
  //   }
  //   return bIsWow64;
  // }

  //void SetProxyEnabled(bool enabled) {
  //  base::win::RegKey key(HKEY_CURRENT_USER,
  //             L"SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Internet Settings",
  //             KEY_SET_VALUE | KEY_WOW64_64KEY);
  //  key.WriteValue(L"ProxyEnable", enabled ? 0x1 : 0x0);
  //}
  //
  //void SetProxyInfo(
  //  const base::string16& server,
  //  const base::string16& except) {
  //  base::win::RegKey key(HKEY_CURRENT_USER,
  //             L"SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Internet Settings",
  //             KEY_SET_VALUE | KEY_WOW64_64KEY);
  //  key.WriteValue(L"ProxyServer", server.c_str());
  //  key.WriteValue(L"ProxyOverride", except.c_str());
  //}

  BOOL SetProxyInfo(bool enabled, const base::string16& server, const base::string16& except)
  {
    INTERNET_PER_CONN_OPTION_LIST optionList;
    BOOL    bReturn;
    DWORD   dwBufSize = sizeof(optionList);

    // Fill the list structure.
    optionList.dwSize = sizeof(optionList);

    // NULL == LAN, otherwise connectoid name.
    optionList.pszConnection = NULL;

    // Set three options.
    if (server.empty() && except.empty())
      optionList.dwOptionCount = 1;
    else
      optionList.dwOptionCount = 3;
    optionList.pOptions = new INTERNET_PER_CONN_OPTION[3];

    // Ensure that the memory was allocated.
    if (NULL == optionList.pOptions)
    {
      // Return FALSE if the memory wasn't allocated.
      return FALSE;
    }

    // Set flags.
    optionList.pOptions[0].dwOption = INTERNET_PER_CONN_FLAGS;
    if (enabled) {
      optionList.pOptions[0].Value.dwValue = PROXY_TYPE_DIRECT |
        PROXY_TYPE_PROXY | PROXY_TYPE_AUTO_DETECT;
    }
    else {
      optionList.pOptions[0].Value.dwValue = PROXY_TYPE_DIRECT | PROXY_TYPE_AUTO_DETECT;
    }

    // Set proxy name.
    optionList.pOptions[1].dwOption = INTERNET_PER_CONN_PROXY_SERVER;
    optionList.pOptions[1].Value.pszValue = const_cast<LPWSTR>(server.data());

    // Set proxy override.
    optionList.pOptions[2].dwOption = INTERNET_PER_CONN_PROXY_BYPASS;
    optionList.pOptions[2].Value.pszValue = const_cast<LPWSTR>(except.data());

    // Set the options on the connection.
    bReturn = InternetSetOption(NULL,
      INTERNET_OPTION_PER_CONNECTION_OPTION, &optionList, dwBufSize);

    // Free the allocated memory.
    delete[] optionList.pOptions;

    return bReturn;
  }

  BOOL SetPACInfo(bool enabled, const base::string16& pac)
  {
    INTERNET_PER_CONN_OPTION_LIST optionList;
    BOOL    bReturn;
    DWORD   dwBufSize = sizeof(optionList);

    // Fill the list structure.
    optionList.dwSize = sizeof(optionList);

    // NULL == LAN, otherwise connectoid name.
    optionList.pszConnection = NULL;

    // Set three options.
    if (pac.empty())
      optionList.dwOptionCount = 1;
    else
      optionList.dwOptionCount = 2;
    optionList.pOptions = new INTERNET_PER_CONN_OPTION[2];

    // Ensure that the memory was allocated.
    if (NULL == optionList.pOptions)
    {
      // Return FALSE if the memory wasn't allocated.
      return FALSE;
    }

    // Set flags.
    optionList.pOptions[0].dwOption = INTERNET_PER_CONN_FLAGS;
    if (enabled) {
      optionList.pOptions[0].Value.dwValue = PROXY_TYPE_DIRECT | PROXY_TYPE_AUTO_PROXY_URL | PROXY_TYPE_AUTO_DETECT;
    }
    else {
      optionList.pOptions[0].Value.dwValue = PROXY_TYPE_DIRECT | PROXY_TYPE_AUTO_DETECT;
    }

    // Set auto proxy url.
    optionList.pOptions[1].dwOption = INTERNET_PER_CONN_AUTOCONFIG_URL;
    optionList.pOptions[1].Value.pszValue = const_cast<LPWSTR>(pac.data());

    // Set the options on the connection.
    bReturn = InternetSetOption(NULL,
      INTERNET_OPTION_PER_CONNECTION_OPTION, &optionList, dwBufSize);

    // Free the allocated memory.
    delete[] optionList.pOptions;

    return bReturn;
  }

  //void SetPACInfo(const base::string16& pac) {
  //  base::win::RegKey key(HKEY_CURRENT_USER,
  //             L"SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Internet Settings",
  //             KEY_SET_VALUE | KEY_WOW64_64KEY);
  //
  //  if(pac.empty())
  //    key.DeleteValue(L"AutoConfigURL");
  //  else
  //    key.WriteValue(L"AutoConfigURL", pac.c_str());
  //}
#endif /*OS_WIN*/

  std::string GetStringFromDictionary(
    const base::DictionaryValue* dict, const std::string& node_path) {
    if (dict) {
      std::string value;
      if (dict->GetString(node_path, &value))
        return value;
    }

    return std::string();
  }

  base::string16 GetString16FromDictionary(
    const base::DictionaryValue* dict, const std::string& node_path) {
    if (dict) {
      base::string16 value;
      if (dict->GetString(node_path, &value))
        return value;
    }

    return base::string16();
  }
}  // namespace
   //YSP+ { config cryptor
namespace cryptorkey {
  std::string key = "3zVssP6JRSduWS2XppyvkaAi64R8rcoh"; //config data cache crypto key
} //namespace cryptorkey

namespace {
  class ConfigCryptor {
  public:
    ConfigCryptor();
    ~ConfigCryptor();
    bool EncryptString(const std::string key, const std::string& plaintext,
      std::string* ciphertext);
    bool DecryptString(const std::string key, const std::string& ciphertext,
      std::string* plaintext);
  };

  ConfigCryptor::ConfigCryptor() {}

  ConfigCryptor::~ConfigCryptor() {}

  bool ConfigCryptor::EncryptString(const std::string key, const std::string& plaintext,
    std::string* ciphertext) {
    AES_KEY aes_key;
    char key_data[17] = { 0 };
    memcpy(key_data, key.c_str(), 8);
    memcpy(key_data + 8, key.c_str() + 8, 8);
    if (AES_set_encrypt_key(reinterpret_cast<const uint8_t*>(key_data),
      AES_BLOCK_SIZE * 8, &aes_key) != 0) {
      return false;
    }
    const size_t out_size = plaintext.size();
    std::string pla_padding = plaintext;
    int offset = AES_BLOCK_SIZE - out_size % AES_BLOCK_SIZE;
    size_t out_size_new = out_size + offset;
    char padding[AES_BLOCK_SIZE];
    memset(padding, offset, AES_BLOCK_SIZE);
    pla_padding.append(padding, offset);
    std::string result;
    uint8_t* out_ptr =
      reinterpret_cast<uint8_t*>(base::WriteInto(&result, out_size_new + 1));

    uint8_t ivec[AES_BLOCK_SIZE] = { 0 };
    memcpy(ivec, key.c_str() + 16, AES_BLOCK_SIZE);

    AES_cbc_encrypt(reinterpret_cast<const uint8_t*>(pla_padding.data()), out_ptr,
      pla_padding.size(), &aes_key, ivec, AES_ENCRYPT);
    ciphertext->swap(result);
    return true;
  }

  bool ConfigCryptor::DecryptString(const std::string key, const std::string& ciphertext,
    std::string* plaintext) {
    AES_KEY aes_key;
    char key_data[17] = { 0 };
    memcpy(key_data, key.c_str(), 8);
    memcpy(key_data + 8, key.c_str() + 8, 8);
    if (AES_set_decrypt_key(reinterpret_cast<const uint8_t*>(key_data),
      AES_BLOCK_SIZE * 8, &aes_key) != 0) {
      return false;
    }
    const size_t out_size = ciphertext.size();
    std::string result;
    uint8_t* out_ptr =
      reinterpret_cast<uint8_t*>(base::WriteInto(&result, out_size + 1));

    uint8_t ivec[AES_BLOCK_SIZE] = { 0 };
    memcpy(ivec, key.c_str() + 16, AES_BLOCK_SIZE);

    AES_cbc_encrypt(reinterpret_cast<const uint8_t*>(ciphertext.data()), out_ptr,
      ciphertext.size(), &aes_key, ivec, AES_DECRYPT);
    size_t length = result.length();
    int offset = result.c_str()[length - 1];
    plaintext->clear();
    plaintext->assign(result, 0, length - offset);
    return true;
  }

} //namespace
  //YSP+ } /*config cryptor*/
YSPLoginManager::YSPLoginManager()
  : login_fetcher_(nullptr),
  replace_fetcher_(nullptr), //YSP+ { Resource Replace }
  singlesignon_fetcher_(nullptr), //YSP+ { SingleSignOn config }
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
  autoConfig_fetcher_(nullptr),
  login_info_(nullptr),
  application_info_(nullptr), //YSP+ { Fetcher resource }
  strategy_info_(nullptr), //YSP+ { Fetcher resource }
  token_info_(nullptr), //YSP+ { Fetcher resource }
  swa_info_(nullptr), //YSP+ { Fetcher resource }
  pc_info_(nullptr), //YSP+ { Fetcher resource }
  auto_config_info_(nullptr), //YSP+ { Fetcher resource }
  loginStatusCode_(false), //YSP+ { Fetcher resource }
  applicationStatusCode_(false), //YSP+ { Fetcher resource }
  strategyStatusCode_(false), //YSP+ { Fetcher resource }
  tokenStatusCode_(false), //YSP+ { Fetcher resource }
  swaStatusCode_(false), //YSP+ { Fetcher resource }
  pcStatusCode_(false), //YSP+ { Fetcher resource }
  singlesignon_info_(nullptr), //YSP+ { SingleSignOn config }
  window_frame_color_(-1),
  window_inactive_color_(0),
  should_parse_response_(false),
  web_initialized_(false),
  offline_status_(true)
{
}

YSPLoginManager::~YSPLoginManager() {
  if (login_fetcher_) {
    delete login_fetcher_;
    login_fetcher_ = nullptr;
  }
  //YSP+ { Resource Replace }
  if (replace_fetcher_) {
    delete replace_fetcher_;
    replace_fetcher_ = nullptr;
  }
  //YSP+ } /*Fetcher resource*/
  //YSP+ { SingleSignOn config
  if (singlesignon_fetcher_) {
    delete singlesignon_fetcher_;
    singlesignon_fetcher_ = nullptr;
  }
  //YSP+ } /*SingleSignOn config*/
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
  observers_.clear();
}

//static
YSPLoginManager* YSPLoginManager::GetInstance() {
  if (!g_instance) {
    g_instance = new YSPLoginManager;
  }
  return g_instance;
}

void YSPLoginManager::Init() {
  // load login info
  base::FilePath filePath = FilePathOfLoginCache("login");
  if (base::PathExists(filePath)) {
    DCHECK(auto_login_);
    int error_code = JSONFileValueDeserializer::JSON_NO_ERROR;
    std::string error_str;
    //YSP+ { config cryptor
    std::string enc_value = "";
    std::string dec_value = "";
    error_code = base::ReadFileToString(filePath, &enc_value);
    if ((!error_code) != JSONFileValueDeserializer::JSON_NO_ERROR) {
      DLOG(INFO) << "Read config failure.";
    }
    if (!enc_value.empty()) {
      ConfigCryptor AES_crypted_;
      AES_crypted_.DecryptString(cryptorkey::key, enc_value, &dec_value);
    }
    std::unique_ptr<base::Value> rootValue = base::JSONReader::Read(dec_value);
    //YSP+ } /*config cryptor*/
    if ((!error_code) == JSONFileValueDeserializer::JSON_NO_ERROR) {
      login_info_ = base::DictionaryValue::From(std::move(rootValue));
    }
  }
  // load application info
  filePath = FilePathOfLoginCache("application");
  if (base::PathExists(filePath)) {
    //DCHECK(auto_login_);
    int error_code = JSONFileValueDeserializer::JSON_NO_ERROR;
    std::string error_str;
    //YSP+ { config cryptor
    std::string enc_value = "";
    std::string dec_value = "";
    error_code = base::ReadFileToString(filePath, &enc_value);
    if ((!error_code) != JSONFileValueDeserializer::JSON_NO_ERROR) {
      DLOG(INFO) << "Read config failure.";
    }
    if (!enc_value.empty()) {
      ConfigCryptor AES_crypted_;
      AES_crypted_.DecryptString(cryptorkey::key, enc_value, &dec_value);
    }
    std::unique_ptr<base::Value> rootValue = base::JSONReader::Read(dec_value);
    //YSP+ } /*config cryptor*/
    if ((!error_code) == JSONFileValueDeserializer::JSON_NO_ERROR) {
      application_info_ = base::DictionaryValue::From(std::move(rootValue));
    }
  }
  // load confituration_pc info
  filePath = FilePathOfLoginCache("confituration_pc");
  if (base::PathExists(filePath)) {
    DCHECK(auto_login_);
    int error_code = JSONFileValueDeserializer::JSON_NO_ERROR;
    std::string error_str;
    //YSP+ { config cryptor
    std::string enc_value = "";
    std::string dec_value = "";
    error_code = base::ReadFileToString(filePath, &enc_value);
    if ((!error_code) != JSONFileValueDeserializer::JSON_NO_ERROR) {
      DLOG(INFO) << "Read config failure.";
    }
    if (!enc_value.empty()) {
      ConfigCryptor AES_crypted_;
      AES_crypted_.DecryptString(cryptorkey::key, enc_value, &dec_value);
    }
    std::unique_ptr<base::Value> rootValue = base::JSONReader::Read(dec_value);
    //YSP+ } /*config cryptor*/
    if ((!error_code) == JSONFileValueDeserializer::JSON_NO_ERROR) {
      pc_info_ = base::DictionaryValue::From(std::move(rootValue));
    }
  }
  // load confituration_strategy info
  filePath = FilePathOfLoginCache("confituration_strategy");
  if (base::PathExists(filePath)) {
    DCHECK(auto_login_);
    int error_code = JSONFileValueDeserializer::JSON_NO_ERROR;
    std::string error_str;
    //YSP+ { config cryptor
    std::string enc_value = "";
    std::string dec_value = "";
    error_code = base::ReadFileToString(filePath, &enc_value);
    if ((!error_code) != JSONFileValueDeserializer::JSON_NO_ERROR) {
      DLOG(INFO) << "Read config failure.";
    }
    if (!enc_value.empty()) {
      ConfigCryptor AES_crypted_;
      AES_crypted_.DecryptString(cryptorkey::key, enc_value, &dec_value);
    }
    std::unique_ptr<base::Value> rootValue = base::JSONReader::Read(dec_value);
    //YSP+ } /*config cryptor*/
    if ((!error_code) == JSONFileValueDeserializer::JSON_NO_ERROR) {
      strategy_info_ = base::DictionaryValue::From(std::move(rootValue));
    }
  }
  // load sso_token info
  filePath = FilePathOfLoginCache("sso_token");
  if (base::PathExists(filePath)) {
    DCHECK(auto_login_);
    int error_code = JSONFileValueDeserializer::JSON_NO_ERROR;
    std::string error_str;
    //YSP+ { config cryptor
    std::string enc_value = "";
    std::string dec_value = "";
    error_code = base::ReadFileToString(filePath, &enc_value);
    if ((!error_code) != JSONFileValueDeserializer::JSON_NO_ERROR) {
      DLOG(INFO) << "Read config failure.";
    }
    if (!enc_value.empty()) {
      ConfigCryptor AES_crypted_;
      AES_crypted_.DecryptString(cryptorkey::key, enc_value, &dec_value);
    }
    std::unique_ptr<base::Value> rootValue = base::JSONReader::Read(dec_value);
    //YSP+ } /*config cryptor*/
    if ((!error_code) == JSONFileValueDeserializer::JSON_NO_ERROR) {
      token_info_ = base::DictionaryValue::From(std::move(rootValue));
    }
  }
  // load swa info
  filePath = FilePathOfLoginCache("swa");
  if (base::PathExists(filePath)) {
    DCHECK(auto_login_);
    int error_code = JSONFileValueDeserializer::JSON_NO_ERROR;
    std::string error_str;
    //YSP+ { config cryptor
    std::string enc_value = "";
    std::string dec_value = "";
    error_code = base::ReadFileToString(filePath, &enc_value);
    if ((!error_code) != JSONFileValueDeserializer::JSON_NO_ERROR) {
      DLOG(INFO) << "Read config failure.";
    }
    if (!enc_value.empty()) {
      ConfigCryptor AES_crypted_;
      AES_crypted_.DecryptString(cryptorkey::key, enc_value, &dec_value);
    }
    std::unique_ptr<base::Value> rootValue = base::JSONReader::Read(dec_value);
    //YSP+ } /*config cryptor*/
    if ((!error_code) == JSONFileValueDeserializer::JSON_NO_ERROR) {
      swa_info_ = base::DictionaryValue::From(std::move(rootValue));
    }
  }
  if (login_info_ && application_info_ && pc_info_ && strategy_info_ && token_info_ && swa_info_) {
    SetLoginStatus(SATUS_VERIFY_CONFIG_SUCCESS);
    ntp_login_status_ = true;
    base::string16 userName = GetYSPUserName();
    std::string headImageUrl = GetHeadImageUrl();
  if (offline_status_) {
    offline_status_ = false;
    std::vector<YSPLoginManagerObserver*>::iterator iter = observers_.begin();
    for (; iter != observers_.end(); ++iter) {
      (*iter)->OnLoginSuccess(userName, headImageUrl);
    }
  }
  }
  UpdateLoginManagerSettings();

  cid_ = GetLastId(prefs::kYSPLoginLastCID);
  account_ = GetLastId(prefs::kYSPLoginLastUID);
  password_ = GetLastId(prefs::kYSPLoginLastPWD);
  PrefService* prefs = g_browser_process->local_state();
  auto_login_ = prefs->GetBoolean(prefs::kYSPAutoLogin);

  if (g_browser_process->local_state()->GetInteger(prefs::kYSPLockScreen)) {
    g_browser_process->local_state()->SetInteger(prefs::kYSPLockScreen, 0);
    Logout();
  }
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

void YSPLoginManager::ModifyPassword(const std::string& oldPassword, const std::string& newPassword)
{
	LOG(INFO) << "YSPLoginManager::ModifyPassword:";
	if (!put_modify_password_fetcher_) 
	{
		put_modify_password_fetcher_ = new YSPFetcherResource(this, g_browser_process->system_request_context());
	}
	else if (put_modify_password_fetcher_->IsLoading())
	{
		return;
	}

	std::vector<std::string> header_list;
	std::string accessToken = GetAccessToken();
	if (!accessToken.empty())
	{
		header_list.push_back("access-token: " + accessToken);
	}

	std::string url = GetManageServer() + kModifyPasswordPath;
	std::string post_data;
	net::AddMultipartValueForUpload("id", GetUserId(), kMultipartBoundary, "", &post_data);
	net::AddMultipartValueForUpload("newPassword", newPassword, kMultipartBoundary, "", &post_data);
	net::AddMultipartValueForUpload("oldPassword", oldPassword, kMultipartBoundary, "", &post_data);
	net::AddMultipartFinalDelimiterForUpload(kMultipartBoundary, &post_data);
	put_modify_password_fetcher_->StarFetcherResource(net::URLFetcher::PUT, url, header_list, post_data, false);
	header_list.clear();
}


void YSPLoginManager::GetGatewayApplictionFetch(bool auto_fetch) {
	std::vector<std::string> header_list;
	std::string accessToken = GetAccessToken();
	// if (get_gateway_appliction_fetcher_ && get_gateway_appliction_fetcher_->IsLoading())
	//	return;

	//获取需要敲门的应用列表
	if (!GetUserId().empty()) {
		header_list.push_back("userid: " + GetUserId());
		if (!accessToken.empty())
			header_list.push_back("access-token: " + accessToken);

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

void YSPLoginManager::PutApplictionStatusFetch(std::string& applicationId, bool applictionStatus) {
  std::vector<std::string> header_list;
  std::string accessToken = GetAccessToken();
  // if (put_appliction_status_fetcher_ && put_appliction_status_fetcher_->IsLoading())
  //	return;

  //上传应用状态
  if (!GetUserId().empty()) {
    header_list.push_back("userid: " + GetUserId());
    if (!accessToken.empty())
      header_list.push_back("access-token: " + accessToken);
    std::string post_data;
    net::AddMultipartValueForUpload("status", applictionStatus ? "1" : "0",
                                    kMultipartBoundary,
                                    "", &post_data);
    net::AddMultipartFinalDelimiterForUpload(kMultipartBoundary, &post_data);

    LOG(INFO) << "YSPLoginManager::PutApplictionStatusFetch";
    if (!put_appliction_status_fetcher_) {
      put_appliction_status_fetcher_ = new YSPFetcherResource(
          this, g_browser_process->system_request_context());
    }
    std::string url =
        GetManageServer() + kApplictionStatusPath + GetUserId() + "/" + applicationId;
    if (put_appliction_status_fetcher_) {
      put_appliction_status_fetcher_->StarFetcherResource(
          net::URLFetcher::PUT, url, header_list, post_data, true);
    }
    header_list.clear();
  }
}


bool YSPLoginManager::SetManageServer(std::string manageServer) {
  DLOG(INFO) << "YSPLoginManager::SetManageServer set manager url: " << manageServer;
  if (GURL(manageServer).is_valid() == false) {
    return false;
  }
  PrefService* prefs = g_browser_process->local_state();
  prefs->SetString(prefs::kYSPUserSetServerAddress, manageServer);
  return true;
}

std::string YSPLoginManager::GetManageServer() {
  PrefService* prefs = g_browser_process->local_state();
  std::string userSettingServer = prefs->GetString(prefs::kYSPUserSetServerAddress);
  if (userSettingServer.length() > 0)
    return userSettingServer;

  std::string server = prefs->GetString(prefs::kYSPServerAddress);
  if (server.length() > 0)
    return server;

  return std::string();
}

//ysp+ { AES DES and SMS4 crypt
std::string YSPLoginManager::GetCryptKey() {
  //return std::string();
  if (pc_info_) {
    base::DictionaryValue* dataDict = nullptr;
    if (pc_info_->GetDictionary("data", &dataDict)) {
      base::DictionaryValue* funcDict = nullptr;
      if (dataDict && dataDict->GetDictionary("cookieEncryptMethod", &funcDict)) {
        std::string cryptkey;
        std::string value, key;
        if (funcDict) {
          funcDict->GetString("value", &value);
          funcDict->GetString("key", &key);
          if (value == "noEncrypt")
            cryptkey = "1" + key;
          else
            cryptkey = value.c_str()[0] + key;
          return cryptkey;
        }
      }
    }
  }

  return std::string();
}//ysp+ }

 //YSP+ { cache encryption
bool YSPLoginManager::GetCacheCrypt() {
  //return std::string();
  if (pc_info_) {
    base::DictionaryValue* dataDict = nullptr;
    if (pc_info_->GetDictionary("data", &dataDict)) {
      base::DictionaryValue* funcDict = nullptr;
      if (dataDict && dataDict->GetDictionary("cacheEncryptMethod", &funcDict)) {
        if (funcDict) {
          std::string value;
          funcDict->GetString("value", &value);
          if (value == "1")
            return true;
        }
      }
    }
  }
  return false;
}
//ysp+ }

base::string16 YSPLoginManager::GetYSPUserName() {
  if (login_info_) {
    base::DictionaryValue* dataDict = nullptr;
    if (login_info_->GetDictionary("data", &dataDict)) {
      base::DictionaryValue* userDict = nullptr;
      if (dataDict && dataDict->GetDictionary("user", &userDict)) {
        base::string16 name;
        if (userDict && userDict->GetString("name", &name))
          return name;
      }
    }
  }

  return base::string16();
}

std::string YSPLoginManager::GetHeadImageUrl() {
  if (login_info_) {
    base::DictionaryValue* dataDict = nullptr;
    if (login_info_->GetDictionary("data", &dataDict)) {
      base::DictionaryValue* userDict = nullptr;
      if (dataDict && dataDict->GetDictionary("user", &userDict)) {
        std::string url;
        if (userDict && userDict->GetString("avatarPath", &url))
          return url;
      }
    }
  }

  return std::string();
}
void YSPLoginManager::OnApplicationFetcherResponseParse(std::unique_ptr<base::DictionaryValue>& response_data, bool from_local, bool auto_fetch)
{
  LOG(INFO) << "YSPLoginManager::OnApplicationFetcherResponseParse auto_fetch:" << auto_fetch;
  std::string folder = "application";
  std::string responseStatus = GetResponseStatusCode(response_data);
  applicationStatusCode_ = (responseStatus == "0");
  if (!from_local) {
    if (auto_login_ && !folder.empty()) {
      base::FilePath savePath = FilePathOfLoginCache(folder);
      //YSP+ { config cryptor
      std::string response_datastring;
      std::string enc_response;
      base::JSONWriter::Write(*response_data, &response_datastring);
      ConfigCryptor AES_crypted_;
      AES_crypted_.EncryptString(cryptorkey::key, response_datastring, &enc_response);
      if (-1 == (base::WriteFile(savePath, enc_response.data(), enc_response.length()))) {
        DLOG(INFO) << "fail write fetch resource cache data.";
      }
      //YSP+ } /*config cryptor*/
    }
  }
  application_info_.reset();
  application_info_.reset(response_data.release());
  if (auto_fetch) {
    NotifyConfigureUpdate("application", GetDataJSAsJSONString());
  }
}
void YSPLoginManager::OnStrategyFetcherResponseParse(std::unique_ptr<base::DictionaryValue>& response_data, bool from_local, bool auto_fetch)
{
  LOG(INFO) << "YSPLoginManager::OnStrategyFetcherResponseParse auto_fetch:" << auto_fetch;
  std::string folder = "confituration_strategy";
  std::string responseStatus = GetResponseStatusCode(response_data);
  strategyStatusCode_ = (responseStatus == "0");
  if (!from_local) {
    if (auto_login_ && !folder.empty()) {
      base::FilePath savePath = FilePathOfLoginCache(folder);
      //YSP+ { config cryptor
      std::string response_datastring;
      std::string enc_response;
      base::JSONWriter::Write(*response_data, &response_datastring);
      ConfigCryptor AES_crypted_;
      AES_crypted_.EncryptString(cryptorkey::key, response_datastring, &enc_response);
      if (-1 == (base::WriteFile(savePath, enc_response.data(), enc_response.length()))) {
        DLOG(INFO) << "fail write fetch resource cache data.";
      }
      //YSP+ } /*config cryptor*/
    }
  }
  strategy_info_.reset();
  strategy_info_.reset(response_data.release());
  if (auto_fetch) {
    NotifyConfigureUpdate("strategy", GetDataJSAsJSONString());
  }
}
void YSPLoginManager::OnTokenFetcherResponseParse(std::unique_ptr<base::DictionaryValue>& response_data, bool from_local, bool auto_fetch)
{
  LOG(INFO) << "YSPLoginManager::OnTokenFetcherResponseParse auto_fetch:" << auto_fetch;
  std::string folder = "sso_token";
  std::string responseStatus = GetResponseStatusCode(response_data);
  tokenStatusCode_ = (responseStatus == "0");
  if (!from_local) {
    if (auto_login_ && !folder.empty()) {
      base::FilePath savePath = FilePathOfLoginCache(folder);
      //YSP+ { config cryptor
      std::string response_datastring;
      std::string enc_response;
      base::JSONWriter::Write(*response_data, &response_datastring);
      ConfigCryptor AES_crypted_;
      AES_crypted_.EncryptString(cryptorkey::key, response_datastring, &enc_response);
      if (-1 == (base::WriteFile(savePath, enc_response.data(), enc_response.length()))) {
        DLOG(INFO) << "fail write fetch resource cache data.";
      }
      //YSP+ } /*config cryptor*/
    }
  }

  token_info_.reset();
  token_info_.reset(response_data.release());
}
void YSPLoginManager::OnSwaFetcherResponseParse(std::unique_ptr<base::DictionaryValue>& response_data, bool from_local, bool auto_fetch)
{
  LOG(INFO) << "YSPLoginManager::OnSwaFetcherResponseParse auto_fetch:" << auto_fetch;
  std::string folder = "swa";
  std::string responseStatus = GetResponseStatusCode(response_data);
  swaStatusCode_ = (responseStatus == "0");
  if (!from_local) {
    if (auto_login_ && !folder.empty()) {
      base::FilePath savePath = FilePathOfLoginCache(folder);
      //YSP+ { config cryptor
      std::string response_datastring;
      std::string enc_response;
      base::JSONWriter::Write(*response_data, &response_datastring);
      ConfigCryptor AES_crypted_;
      AES_crypted_.EncryptString(cryptorkey::key, response_datastring, &enc_response);
      if (-1 == (base::WriteFile(savePath, enc_response.data(), enc_response.length()))) {
        DLOG(INFO) << "fail write fetch resource cache data.";
      }
      //YSP+ } /*config cryptor*/
    }
  }

  swa_info_.reset();
  swa_info_.reset(response_data.release());
}
void YSPLoginManager::OnPcFetcherResponseParse(std::unique_ptr<base::DictionaryValue>& response_data, bool from_local, bool auto_fetch)
{
  LOG(INFO) << "YSPLoginManager::OnPcFetcherResponseParse auto_fetch:" << auto_fetch;
  std::string folder = "confituration_pc";
  std::string responseStatus = GetResponseStatusCode(response_data);
  pcStatusCode_ = (responseStatus == "0");
  if (!from_local) {
    if (auto_login_ && !folder.empty()) {
      base::FilePath savePath = FilePathOfLoginCache(folder);
      //YSP+ { config cryptor
      std::string response_datastring;
      std::string enc_response;
      base::JSONWriter::Write(*response_data, &response_datastring);
      ConfigCryptor AES_crypted_;
      AES_crypted_.EncryptString(cryptorkey::key, response_datastring, &enc_response);
      if (-1 == (base::WriteFile(savePath, enc_response.data(), enc_response.length()))) {
        DLOG(INFO) << "fail write fetch resource cache data.";
      }
      //YSP+ } /*config cryptor*/
    }
  }

  pc_info_.reset();
  pc_info_.reset(response_data.release());
  if (auto_fetch) {
    NotifyConfigureUpdate("pc", "");
  }
}
void YSPLoginManager::OnUserFetcherResponseParse(std::unique_ptr<base::DictionaryValue>& response_data, bool from_local, bool auto_fetch)
{
  LOG(INFO) << "YSPLoginManager::OnUserFetcherResponseParse auto_fetch:" << auto_fetch;
  std::string responseStatus = GetResponseStatusCode(response_data);
  loginStatusCode_ = (responseStatus == "0");
  std::string folder = "login";
  if (!from_local) {
    if (auto_login_ && !folder.empty()) {
      base::FilePath savePath = FilePathOfLoginCache(folder);
      //YSP+ { config cryptor
      std::string response_datastring;
      std::string enc_response;
      base::JSONWriter::Write(*response_data, &response_datastring);
      ConfigCryptor AES_crypted_;
      AES_crypted_.EncryptString(cryptorkey::key, response_datastring, &enc_response);
      if (-1 == (base::WriteFile(savePath, enc_response.data(), enc_response.length()))) {
        DLOG(INFO) << "fail write fetch resource cache data.";
      }
      //YSP+ } /*config cryptor*/
    }
  }

  base::DictionaryValue* userDict_new = nullptr;
  base::DictionaryValue* companyDict_new = nullptr;
  response_data->GetDictionary("data.user", &userDict_new);
  response_data->GetDictionary("data.company", &companyDict_new);

  if (login_info_ && userDict_new && companyDict_new) {
    login_info_->Set("data.user", std::make_unique<base::Value>(userDict_new));
    login_info_->Set("data.company", std::make_unique<base::Value>(companyDict_new));
  }

  response_data->Clear();
  if (auto_fetch) {
    NotifyConfigureUpdate("accountInfo", GetUserInfoAsJSONString());
  }
}
void YSPLoginManager::OnAutoTokenFetcherResponseParse(std::unique_ptr<base::DictionaryValue>& response_data, bool from_local, bool auto_fetch)
{
  LOG(INFO) << "YSPLoginManager::OnAutoTokenFetcherResponseParse auto_fetch:" << auto_fetch;
  std::string new_access_token = "";
  response_data->GetString("data.access_token", &new_access_token);
  SaveLastId(prefs::kYSPAccessToken, new_access_token);
  if (!sendRequestList_.empty()) {
    std::vector<std::string>::iterator iter = sendRequestList_.begin();
    for (; iter != sendRequestList_.end(); ++iter) {
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
      else if ((*iter).find(KGetUserPath) != std::string::npos)
        GetUserfetcher(false);
      else if ((*iter).find(kAutoGetConfigPath) != std::string::npos)
        GetAutoConfigfetcher();
      else if ((*iter).find(kLoginUploadPath) != std::string::npos)
        Restore();
      else if ((*iter).find(kSdpDevicePath) != std::string::npos)
        GetSdpDevicefetcher(false);
    }
    sendRequestList_.clear();
  }
}
void YSPLoginManager::OnSdpDeviceFetcherResponseParse(std::unique_ptr<base::DictionaryValue>& response_data, bool from_local, bool auto_fetch)
{
  LOG(INFO) << "YSPLoginManager::OnSdpDeviceFetcherResponseParse auto_fetch:" << auto_fetch;
  std::string responseStatus = GetResponseStatusCode(response_data);
  bool deviceStatusCode = (responseStatus == "0");
  base::ListValue* dataDict = nullptr;
  response_data->GetList("data", &dataDict);
  std::string result = "";
  if (dataDict) {
    PrefService* prefs = g_browser_process->local_state();
    std::string strDeviceId = GetRegMachineId();
    if (strDeviceId.empty())
      strDeviceId = prefs->GetString(prefs::kYSPDeviceID);
    if (strDeviceId.empty()) {
      strDeviceId = base::GenerateGUID();
      prefs->SetString(prefs::kYSPDeviceID, strDeviceId);
    }
    response_data->SetString("localDeviceId", strDeviceId);
    base::JSONWriter::Write(*(response_data.get()), &result);
    NotifyConfigureUpdate("getLoginDeviceInfo", result);
  }
  else {
    if (deviceStatusCode)
      GetSdpDevicefetcher();
    //通知页面删除设备成功
    base::JSONWriter::Write(*(response_data.get()), &result);
    NotifyConfigureUpdate("deleteLoginDevice", result);
  }
}

//YSP+ { Fetcher resource
std::string YSPLoginManager::GetUserId() {
  if (login_info_) {
    std::string userId;
    if (login_info_->GetString("data.user.id", &userId))
      return userId;
  }
  return std::string();
}

//获取策略版本
int YSPLoginManager::GetStrategyVersion() {
  int strategyVersion = 0;
  if (login_info_) {
    if (login_info_->GetInteger("data.strategy.version", &strategyVersion)) {
      return strategyVersion;
    }
  }
  return strategyVersion;
}

//获取策略ID
std::string YSPLoginManager::GetActivelId() {
  if (login_info_) {
    std::string activelId;
    if (login_info_->GetString("data.strategy.activeId", &activelId)) {
      if (activelId.empty() && auto_config_info_)
        auto_config_info_->GetString("data.activeStrategyId", &activelId);
      return activelId;
    }
  }
  return std::string("null");
}

//获取access_token
std::string YSPLoginManager::GetAccessTokenForLogin() {
  if (login_info_) {
    std::string accessToken;
    if (login_info_->GetString("data.token.access_token", &accessToken)) {
      //DLOG(INFO) << "login_info_ access token: ", accessToken;
      return accessToken;
    }
  }
  return std::string();
}

std::string YSPLoginManager::GetAccessToken() {
  //DLOG(INFO) << "prefs access token: " << prefs->GetString(prefs::kYSPAccessToken);
  return GetLastId(prefs::kYSPAccessToken);
}


//获取refresh_token
std::string YSPLoginManager::GetRefreshTokenForLogin() {
  if (login_info_) {
    std::string refreshToken;
    if (login_info_->GetString("data.token.refresh_token", &refreshToken)) {
      return refreshToken;
    }
  }
  return std::string();
}

std::string YSPLoginManager::GetRefreshToken() {
  return GetLastId(prefs::kYSPRefreshToken);
}

void YSPLoginManager::GetApplicationfetcher(bool auto_fetch)
{
  std::vector<std::string> header_list;
  std::string accessToken = GetAccessToken();
  //if (get_application_fetcher_ && get_application_fetcher_->IsLoading())
  //  return;
  header_list.push_back("userid: " + GetUserId());
  if (!accessToken.empty())
    header_list.push_back("access-token: " + accessToken);
  //获取用户应用
  {
    LOG(INFO) << "YSPLoginManager::GetApplicationfetcher auto_fetch:" << auto_fetch;
    if (!get_application_fetcher_) {
      get_application_fetcher_ =
        new YSPFetcherResource(this,
          g_browser_process->system_request_context());
    }
    std::string url = GetManageServer() + kGetApplicationPath + GetActivelId();
    if (get_application_fetcher_) {
      get_application_fetcher_->StarFetcherResource(
        net::URLFetcher::GET,
        url,
        header_list,
        "",
        auto_fetch);
    }
  }
  header_list.clear();
}

void YSPLoginManager::GetStrategyfetcher(bool auto_fetch)
{
  std::vector<std::string> header_list;
  std::string accessToken = GetAccessToken();
  //if (get_configuration_strategy_fetcher_ && get_configuration_strategy_fetcher_->IsLoading())
  //  return;

  header_list.push_back("userid: " + GetUserId());
  if (!accessToken.empty())
    header_list.push_back("access-token: " + accessToken);
  //获取用户功能配置
  {
    LOG(INFO) << "YSPLoginManager::GetStrategyfetcher auto_fetch:" << auto_fetch;
    if (!get_configuration_strategy_fetcher_) {
      get_configuration_strategy_fetcher_ =
        new YSPFetcherResource(this,
          g_browser_process->system_request_context());
    }
    std::string url = GetManageServer() + kGetConfigurationStrategyPath + GetUserId() + "/" + GetActivelId();
    if (get_configuration_strategy_fetcher_) {
      get_configuration_strategy_fetcher_->StarFetcherResource(
        net::URLFetcher::GET,
        url,
        header_list,
        "",
        auto_fetch);
    }
  }
  header_list.clear();
}

void YSPLoginManager::GetTokenfetcher(bool auto_fetch)
{
  std::vector<std::string> header_list;
  std::string accessToken = GetAccessToken();
  //if (get_sso_token_fetcher_ && get_sso_token_fetcher_->IsLoading())
  //  return;
  if (GetUserId().empty() || GetCompanyId().empty())
    return;
  header_list.push_back("userid: " + GetUserId());
  if (!accessToken.empty())
    header_list.push_back("access-token: " + accessToken);
  //获取sso的token
  {
    LOG(INFO) << "YSPLoginManager::GetTokenfetcher auto_fetch:" << auto_fetch;
    std::string post_data;
    net::AddMultipartValueForUpload("companyId", GetCompanyId(), kMultipartBoundary, "", &post_data);
    net::AddMultipartValueForUpload("username", account_, kMultipartBoundary, "", &post_data);
    net::AddMultipartValueForUpload("password", password_, kMultipartBoundary, "", &post_data);
    net::AddMultipartFinalDelimiterForUpload(kMultipartBoundary, &post_data);
    if (!get_sso_token_fetcher_) {
      get_sso_token_fetcher_ =
        new YSPFetcherResource(this,
          g_browser_process->system_request_context());
    }
    std::string url = GetManageServer() + kGetSsoTokenPath;
    if (get_sso_token_fetcher_) {
      get_sso_token_fetcher_->StarFetcherResource(
        net::URLFetcher::POST,
        url,
        header_list,
        post_data,
        auto_fetch);
    }
  }
  header_list.clear();
}

void YSPLoginManager::GetSwafetcher(bool auto_fetch)
{
  std::vector<std::string> header_list;
  std::string accessToken = GetAccessToken();
  //if (get_swa_fetcher_ && get_swa_fetcher_->IsLoading())
  //  return;
  header_list.push_back("userid: " + GetUserId());
  if (!accessToken.empty())
    header_list.push_back("access-token: " + accessToken);
  //获取SWA配置
  {
    LOG(INFO) << "YSPLoginManager::GetSwafetcher auto_fetch:" << auto_fetch;
    if (!get_swa_fetcher_) {
      get_swa_fetcher_ =
        new YSPFetcherResource(this,
          g_browser_process->system_request_context());
    }
    std::string url = GetManageServer() + kGetSWAPath + GetUserId() + "/" + GetActivelId();
    if (get_swa_fetcher_) {
      get_swa_fetcher_->StarFetcherResource(
        net::URLFetcher::GET,
        url,
        header_list,
        "",
        auto_fetch);
    }
  }
  header_list.clear();
}

void YSPLoginManager::GetPcfetcher(bool auto_fetch)
{
  std::vector<std::string> header_list;
  std::string accessToken = GetAccessToken();
  //if (get_configuration_pc_fetcher_ && get_configuration_pc_fetcher_->IsLoading())
  //  return;
  if (GetUserId().empty() || GetCompanyId().empty())
    return;
  header_list.push_back("userid: " + GetUserId());
  if (!accessToken.empty())
    header_list.push_back("access-token: " + accessToken);
  //获取PC配置
  {
    LOG(INFO) << "YSPLoginManager::GetPcfetcher auto_fetch:" << auto_fetch;
    if (!get_configuration_pc_fetcher_) {
      get_configuration_pc_fetcher_ =
        new YSPFetcherResource(this,
          g_browser_process->system_request_context());
    }
    std::string url = GetManageServer() + kGetConfigurationPCPath + GetCompanyId();
    if (get_configuration_pc_fetcher_) {
      get_configuration_pc_fetcher_->StarFetcherResource(
        net::URLFetcher::GET,
        url,
        header_list,
        "",
        auto_fetch);
    }
  }
  header_list.clear();
}

void YSPLoginManager::GetUserfetcher(bool auto_fetch)
{
  std::vector<std::string> header_list;
  std::string accessToken = GetAccessToken();
  //if (get_user_fetcher_ && get_user_fetcher_->IsLoading())
  //  return;

  //获取企业和用户信息
  if (!GetUserId().empty() && !GetCompanyId().empty())
  {
    header_list.push_back("userid: " + GetUserId());
    if (!accessToken.empty())
      header_list.push_back("access-token: " + accessToken);
    LOG(INFO) << "YSPLoginManager::GetUserfetcher auto_fetch:" << auto_fetch;
    if (!get_user_fetcher_) {
      get_user_fetcher_ =
        new YSPFetcherResource(this,
          g_browser_process->system_request_context());
    }
    std::string url = GetManageServer() + KGetUserPath + GetCompanyId() + "/" + GetUserId();
    if (get_user_fetcher_) {
      get_user_fetcher_->StarFetcherResource(
        net::URLFetcher::GET,
        url,
        header_list,
        "",
        auto_fetch);
    }
    header_list.clear();
  }
}

void YSPLoginManager::GetAuthTokenfetcher(std::string source_url, bool auto_fetch)
{
  if (!sendRequestList_.empty()) {
    int count = 0;
    std::vector<std::string>::iterator iter = sendRequestList_.begin();
    for (; iter != sendRequestList_.end(); ++iter) {
      if (*iter == source_url)
        count++;
    }
    if (count == 0)
      sendRequestList_.push_back(source_url);
  }
  else {
    sendRequestList_.push_back(source_url);
  }
  std::vector<std::string> header_list;
  std::string accessToken = GetAccessToken();
  if (get_auth_token_fetcher_ && get_auth_token_fetcher_->IsLoading())
      return;
  header_list.push_back("userid: " + GetUserId());

  //获取auth的token
  {
    LOG(INFO) << "YSPLoginManager::GetAuthTokenfetcher auto_fetch:" << auto_fetch;
    std::string post_data;
    net::AddMultipartValueForUpload("refresh_token", GetRefreshToken(), kMultipartBoundary, "", &post_data);
    net::AddMultipartValueForUpload("access_token", GetAccessToken(), kMultipartBoundary, "", &post_data);
    net::AddMultipartFinalDelimiterForUpload(kMultipartBoundary, &post_data);
    if (!get_auth_token_fetcher_) {
      get_auth_token_fetcher_ =
        new YSPFetcherResource(this,
          g_browser_process->system_request_context());
    }
    std::string url = GetManageServer() + kGetAuthTokenPath;
    if (get_auth_token_fetcher_) {
      get_auth_token_fetcher_->StarFetcherResource(
        net::URLFetcher::POST,
        url,
        header_list,
        post_data,
        auto_fetch);
    }
  }
  header_list.clear();
}

void YSPLoginManager::GetAutoConfigfetcher(bool auto_fetch)
{
  std::vector<std::string> header_list;
  std::string accessToken = GetAccessToken();

  //获取企业和用户信息
  if (!GetUserId().empty() && !GetCompanyId().empty())
  {
    header_list.push_back("userid: " + GetUserId());
    if (!accessToken.empty())
      header_list.push_back("access-token: " + accessToken);
    LOG(INFO) << "YSPLoginManager::GetAutoConfigfetcher.";
    int strategyVersion = YSPLoginManager::GetInstance()->GetStrategyVersion();
    char version_str[10] = "";
    sprintf(version_str, "%d", strategyVersion);
    if (!autoConfig_fetcher_) {
      autoConfig_fetcher_ =
        new YSPAutoConfigFetcher(g_browser_process->system_request_context());
    }
    std::string url = GetManageServer() + kAutoGetConfigPath + GetCompanyId() + "/1/" + GetUserId() + "/" + GetActivelId() + "/" + version_str;
    if (autoConfig_fetcher_) {
      autoConfig_fetcher_->StartGetConfigVersion(
        url,
        header_list,
    auto_fetch);
    }
    header_list.clear();
  }
}

void YSPLoginManager::GetSdpDevicefetcher(bool auto_fetch)
{
  std::vector<std::string> header_list;
  std::string accessToken = GetAccessToken();
  //if (get_sdp_device_fetcher_ && get_sdp_device_fetcher_->IsLoading())
  //  return;

  //获取用户登陆过的设备
  if (!GetUserId().empty())
  {
    header_list.push_back("userid: " + GetUserId());
    if (!accessToken.empty())
      header_list.push_back("access-token: " + accessToken);
    LOG(INFO) << "YSPLoginManager::GetSdpDevicefetcher auto_fetch:" << auto_fetch;
    if (!get_sdp_device_fetcher_) {
      get_sdp_device_fetcher_ =
        new YSPFetcherResource(this,
          g_browser_process->system_request_context());
    }
    std::string url = GetManageServer() + kSdpDevicePath + GetUserId();
    if (get_sdp_device_fetcher_) {
      get_sdp_device_fetcher_->StarFetcherResource(
        net::URLFetcher::GET,
        url,
        header_list,
        "",
        auto_fetch);
    }
    header_list.clear();
  }
}

//ysp+ { ysp single sign on
std::string YSPLoginManager::GetYSPSingleSignOnString()
{
	if (pc_info_) {
		base::DictionaryValue* dataDict = nullptr;
		if (pc_info_->GetDictionary("data", &dataDict)) {
			base::ListValue* domainDict = nullptr;
			if (dataDict && dataDict->GetList("ssoTokenUrls.list", &domainDict)) {
				std::string domainList_string = "";
				if (base::JSONWriter::Write(*domainDict, &domainList_string))
					return domainList_string;
			}
		}
	}
	return std::string();
}
//ysp+ }

void YSPLoginManager::PutSdpDevicefetcher(std::string deviceId, bool auto_fetch)
{
  std::vector<std::string> header_list;
  std::string accessToken = GetAccessToken();
  //if (put_sdp_device_fetcher_ && put_sdp_device_fetcher_->IsLoading())
  //  return;

  //删除用户登陆过的设备
  if (!GetUserId().empty())
  {
    header_list.push_back("userid: " + GetUserId());
    if (!accessToken.empty())
      header_list.push_back("access-token: " + accessToken);
    std::string post_data;
    net::AddMultipartValueForUpload("deviceId", deviceId, kMultipartBoundary, "", &post_data);
    net::AddMultipartFinalDelimiterForUpload(kMultipartBoundary, &post_data);
    LOG(INFO) << "YSPLoginManager::PutSdpDevicefetcher auto_fetch:" << auto_fetch;
    if (!put_sdp_device_fetcher_) {
      put_sdp_device_fetcher_ =
        new YSPFetcherResource(this,
          g_browser_process->system_request_context());
    }
    std::string url = GetManageServer() + kSdpDevicePath + GetUserId() + "/" + deviceId;
    if (put_sdp_device_fetcher_) {
      put_sdp_device_fetcher_->StarFetcherResource(
        net::URLFetcher::PUT,
        url,
        header_list,
        post_data,
        auto_fetch);
    }
    header_list.clear();
  }
}

std::string YSPLoginManager::GetResponseStatusCode(std::unique_ptr<base::DictionaryValue>& response_data) {
  std::string statusCode = "-1";
  if (response_data) {
    response_data->GetString("errCode", &statusCode);
  }
  return statusCode;
}

base::string16 YSPLoginManager::GetResponseMessage(std::unique_ptr<base::DictionaryValue>& response_data) {
  base::string16 message = base::string16();
  if (response_data) {
    response_data->GetString("message", &message);
  }
  DLOG(ERROR) << "GetResponseMessage message: " << message;
  return message;
}

base::string16 YSPLoginManager::GetResponseErrorMessage(const std::string errCode) {
  base::string16 message = base::string16();
  if (errCode == "E1001") {
    message = l10n_util::GetStringUTF16(IDS_YSP_ACCESS_PROHIBITION);
  }
  else if (errCode == "E1002") {
    message = l10n_util::GetStringUTF16(IDS_YSP_ACCESS_TOKEN_EXPIRED);
  }
  else if (errCode == "E1003") {
    message = l10n_util::GetStringUTF16(IDS_YSP_REFRESH_TOKEN_EXPIRED);
  }
  else if (errCode == "E2001") {
    message = l10n_util::GetStringUTF16(IDS_YSP_COMPANY_NOT_FOUND);
  }
  else if (errCode == "E3001") {
    message = l10n_util::GetStringUTF16(IDS_YSP_USER_NOT_FOUND);
  }
  else if (errCode == "E3002") {
    message = l10n_util::GetStringUTF16(IDS_YSP_WRONG_PASSWORD);
  }
  else if (errCode == "E3003") {
    message = l10n_util::GetStringUTF16(IDS_YSP_AD_USERS_FORBID_UPDATING);
  }
  else if (errCode == "E3004") {
    message = l10n_util::GetStringUTF16(IDS_YSP_USER_DISABLED);
  }
  else if (errCode == "E4001") {
    message = l10n_util::GetStringUTF16(IDS_YSP_DEVICE_DISABLED);
  }
  else if (errCode == "E9999") {
    message = l10n_util::GetStringUTF16(IDS_YSP_HANDLE_ERROR);
  }
  else if (errCode == "-1") {
    base::string16 errstr = l10n_util::GetStringUTF16(IDS_YSP_SERVER_ERROR_CODE);
    message = errstr + base::UTF8ToUTF16(errCode + " ") + l10n_util::GetStringUTF16(IDS_YSP_CONNENT_SERVICE_FAILED);
  }
  else if (errCode == "404" || errCode == "403" || errCode == "400" || errCode == "410") {
    base::string16 errstr = l10n_util::GetStringUTF16(IDS_YSP_SERVER_ERROR_CODE);
    message = errstr + base::UTF8ToUTF16(errCode + " ") + l10n_util::GetStringUTF16(IDS_YSP_SERVER_RESOURCE_ERROR);
  }
  else if (errCode == "502" || errCode == "500" || errCode == "501" || errCode == "503" || errCode == "504" || errCode == "505") {
    base::string16 errstr = l10n_util::GetStringUTF16(IDS_YSP_SERVER_ERROR_CODE);
    message = errstr + base::UTF8ToUTF16(errCode + " ") + l10n_util::GetStringUTF16(IDS_YSP_SERVER_INTERNAL_ERROR);
  }
  else {
    message = l10n_util::GetStringUTF16(IDS_YSP_UNKNOWN_ERROR);
  }
  LOG(ERROR) << "GetResponseErrorMessage message: " << message;
  return message;
}
//YSP+ } /*Fetcher resource*/
std::string YSPLoginManager::GetLoginStatusCode() {
  std::string statusCode = "-1";
  if (login_info_) {
    login_info_->GetString("errCode", &statusCode);
  }
  DLOG(INFO) << "YSPLoginManager::GetLoginStatusCode: " << statusCode;
  return statusCode;
}

void YSPLoginManager::SetLoginStatus(int status) {
  DLOG(INFO) << "YSPLoginManager::SetLoginStatus status:" << status;
  login_status_ = status;
  // TODO: to noify all observers that the status is changed.
  //std::vector<YSPLoginManagerObserver*>::iterator iter = observers_.begin();
  //for (; iter != observers_.end(); ++iter) {
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
    base::DictionaryValue* dataDict = nullptr;
    if (login_info_->GetDictionary("data", &dataDict)) {
      if (dataDict) {
        std::string jsonString;
        base::JSONWriter::Write(*dataDict, &jsonString);
        return jsonString;
      }
    }
  }

  return "";
}

std::string YSPLoginManager::GetDataJSAsJSONString() {
  if (application_info_) {
    base::ListValue* dataList = nullptr;
    if (application_info_->GetList("data", &dataList)) {
      if (dataList) {
        std::string jsonString;
        base::JSONWriter::Write(*dataList, &jsonString);
        return jsonString;
      }
    }
  }

  return "";
}
//ysp+ { show config
std::string YSPLoginManager::GetShowConfigAsJSONString() {
  std::string result = "";
  base::DictionaryValue configDict;
  if (login_info_) {
    base::DictionaryValue* tmpDict = nullptr;
    login_info_->GetDictionary("data", &tmpDict);
    if (tmpDict) {
      configDict.Set("login", std::make_unique<base::Value>(tmpDict));
    }
  }
  if (application_info_) {
    base::ListValue* tmpDict = nullptr;
    application_info_->GetList("data", &tmpDict);
    if (tmpDict) {
      configDict.Set("application", std::make_unique<base::Value>(tmpDict));
    }
  }
  if (strategy_info_) {
    base::DictionaryValue* tmpDict = nullptr;
    strategy_info_->GetDictionary("data", &tmpDict);
    if (tmpDict) {
      configDict.Set("strategy", std::make_unique<base::Value>(tmpDict));
    }
  }
  if (token_info_) {
    base::DictionaryValue* tmpDict = nullptr;
    token_info_->GetDictionary("data", &tmpDict);
    if (tmpDict) {
      configDict.Set("tocken", std::make_unique<base::Value>(tmpDict));
    }
  }
  if (swa_info_) {
    base::ListValue* tmpDict = nullptr;
    swa_info_->GetList("data", &tmpDict);
    if (tmpDict) {
      configDict.Set("swa", std::make_unique<base::Value>(tmpDict));
    }
  }
  if (pc_info_) {
    base::DictionaryValue* tmpDict = nullptr;
    pc_info_->GetDictionary("data", &tmpDict);
    if (tmpDict) {
      configDict.Set("pc", std::make_unique<base::Value>(tmpDict));
    }
  }
  base::JSONWriter::Write(configDict, &result);

  return result;
}
//ysp+ } /*show config*/
//TODO(matianzhi) ysp+{push server api}
void YSPLoginManager::SetPushData(const std::string value)
{
  if (!value.empty()) {
    std::unique_ptr<base::DictionaryValue> pushDataDict = nullptr;
    std::unique_ptr<base::Value> rootValue = base::JSONReader::Read(value);
    pushDataDict = base::DictionaryValue::From(std::move(rootValue));
    std::string eventID;
    pushDataDict->GetString("eventID", &eventID);
    if (eventID == "allowLogin") {
      std::string key = "CSkBQrCX7X7Tlsp38jZQj5uRD0w6MwUx"; //push data crypto key
      std::string dec_userInfo = "";
      std::string userInfo, userInfo_base64;
      pushDataDict.get()->GetString("data.identity", &userInfo_base64);
      ConfigCryptor AES_crypted_;
      base::Base64Decode(userInfo_base64, &userInfo);
      AES_crypted_.DecryptString(key, userInfo, &dec_userInfo);
      if (!dec_userInfo.empty()) {
        std::unique_ptr<base::Value> userInfoValue = base::JSONReader::Read(dec_userInfo);
        std::unique_ptr<base::DictionaryValue> userInfoDict = base::DictionaryValue::From(std::move(userInfoValue));
        std::string server, userName, password;
        userInfoDict->GetString("server", &server);
        userInfoDict->GetString("userName", &userName);
        userInfoDict->GetString("password", &password);
        if (!server.empty() && !userName.empty() && !password.empty()) {
          StartLogin(server, userName, password);
        }
        DLOG(INFO) << "user info: " << dec_userInfo;
      }
    }
    else if (eventID == "pushNotification") {
      GetAutoConfigfetcher();
    }
    else if (eventID == "pushMessage") {
      std::string type = "";
      if (pushDataDict && pushDataDict.get()->GetString("data.name", &type)) {
        if (type == "removeDevice") {
          std::string device_id = "";
          if (pushDataDict && pushDataDict.get()->GetString("data.content.deviceId", &device_id)) {
            std::string key = "deviceId";
            std::string local_device_id = GetValueForKey(key);
            if (local_device_id == device_id) {
              //clear user data
              NotifyConfigureUpdate("removeDevice", "");
              Logout();
            }
            else {
              GetSdpDevicefetcher();
            }
          }
        }
      }
    }
    else if (eventID == "reconnect") {
      GetAutoConfigfetcher();
    }
    else {
      DLOG(INFO) << "YSPLoginManager::SetPushData value: " << value;
    }
    UpdateLoginManagerSettings();
  }
}
//ysp+
bool YSPLoginManager::Restore() {
  if (!auto_login_ || cid_.empty() || account_.empty() || password_.empty())
      return false;
  if (login_info_) {
    double expiryDate = 0;
    login_info_->GetDouble("data.token.expiryDate", &expiryDate);
    if (expiryDate != 0) {
      if (base::Time::Now().ToJavaTime() >= expiryDate) {
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
  loginStatusCode_ = applicationStatusCode_ = strategyStatusCode_ =
    tokenStatusCode_ = swaStatusCode_ = pcStatusCode_ = false;
  // login online
  std::string dominName = "";
  std::string port = GURL(cid_).port();
  if (port.empty())
    dominName = GURL(cid_).host();
  else
    dominName = GURL(cid_).host() + ":" + GURL(cid_).port();
  if (!login_fetcher_) {
    login_fetcher_ =
      new YSPLoginFetcher(this,
        g_browser_process->system_request_context());
  }
  if (login_fetcher_) {
    std::string deviceInfo = GetDeviceInfo();
    login_fetcher_->StartLogin(
      GetManageServer(),
      dominName,
      account_,
      password_,
      header_list,
      deviceInfo);
  }

  return true;  // restored
}

void YSPLoginManager::StartLogin(
  const std::string& cid,
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
      (*iter)->OnLoginFailure(l10n_util::GetStringUTF16(IDS_YSP_LOGIN_ERROR_REQUEST_FAILED));
    }
    return;
  }
  PrefService* prefs = g_browser_process->local_state();
  prefs->SetString(prefs::kYSPServerAddress, cid);
  prefs->SetString(prefs::kYSPUserSetServerAddress, cid);
  loginStatusCode_ = applicationStatusCode_ = strategyStatusCode_ =
    tokenStatusCode_ = swaStatusCode_ = pcStatusCode_ = false;
  {
    DLOG(INFO) << "YSPLoginManager::StartLogin login online";
    // login online
    std::string dominName = "";
    std::string port = GURL(cid).port();
    if (port.empty())
      dominName = GURL(cid).host();
    else
      dominName = GURL(cid).host() + ":" + GURL(cid).port();
    if (!login_fetcher_) {
      login_fetcher_ =
        new YSPLoginFetcher(this,
          g_browser_process->system_request_context());
    }
    if (login_fetcher_) {
      std::string deviceInfo = GetDeviceInfo();
      login_fetcher_->StartLogin(
        GetManageServer(),
        dominName,
        account_,
        password,
        header_list,
        deviceInfo);
    }
  }
}

void YSPLoginManager::ClearCache() {
  base::FilePath filePath = FilePathOfLoginCache("login");
  if (base::PathExists(filePath)) {
    base::DeleteFile(filePath, false);
  }
  login_info_.reset();
  filePath = FilePathOfLoginCache("application");
  if (base::PathExists(filePath)) {
    base::DeleteFile(filePath, false);
  }
  application_info_.reset();
  filePath = FilePathOfLoginCache("confituration_pc");
  if (base::PathExists(filePath)) {
    base::DeleteFile(filePath, false);
  }
  pc_info_.reset();
  filePath = FilePathOfLoginCache("confituration_strategy");
  if (base::PathExists(filePath)) {
    base::DeleteFile(filePath, false);
  }
  strategy_info_.reset();
  filePath = FilePathOfLoginCache("sso_token");
  if (base::PathExists(filePath)) {
    base::DeleteFile(filePath, false);
  }
  token_info_.reset();
  filePath = FilePathOfLoginCache("swa");
  if (base::PathExists(filePath)) {
    base::DeleteFile(filePath, false);
  }
  swa_info_.reset();
  //YSP+ { SingleSignOn config
  singlesignon_info_.reset();
  content::BrowserThread::PostTask(
    content::BrowserThread::IO, FROM_HERE,
    base::Bind(&net::URLRequestHttpJob::setSSOConfigValue, ""));
  //YSP+ } /*SingleSignOn config*/
  loginStatusCode_ = applicationStatusCode_ = strategyStatusCode_ =
    tokenStatusCode_ = swaStatusCode_ = pcStatusCode_ = false;
}

void YSPLoginManager::Logout() {
  window_frame_color_ = -1;
  window_inactive_color_ = 0;
  should_parse_response_ = false;
  SaveLastId(prefs::kYSPLoginLastPWD, "");
  SaveLastId(prefs::kYSPAccessToken, "");
  SaveLastId(prefs::kYSPRefreshToken, "");
  password_.clear();
  SetLoginStatus(SATUS_LOGOUT); //ysp+ { auto get config }
  ntp_login_status_ = false;
  //set_auto_login(false); //Once user does logout manually, the auto_login becomes false.
  net::URLRequestHttpJob::clearHeader();
  std::vector<YSPLoginManagerObserver*>::iterator iter = observers_.begin();
  for (; iter != observers_.end(); ++iter) {
    (*iter)->OnLogout();
  }
  // clear local cache
  ClearCache();
}

//YSP+ { Resource Replace
// YSPReplaceFetcherDelegate:
void YSPLoginManager::OnReplaceRequestFailure(const std::string& error) {
  DLOG(INFO) << "YSPLoginManager::OnReplaceRequestFailure";
  LOG(INFO) << "Resource replace file download failure !";
  //std::vector<YSPLoginManagerObserver*>::iterator iter = observers_.begin();
  //for (; iter != observers_.end(); ++iter) {
  //  (*iter)->OnLoginRequestFailure();
  //}
}

void YSPLoginManager::OnReplaceResponseParseSuccess(const std::string& response_data, const std::string& path_url) {
  if (response_data.empty()) {
    DLOG(INFO) << "YSPLoginManager::OnReplaceResponseParseSuccess ignore response.";
    return;
  }
  DLOG(INFO) << "YSPLoginManager::OnReplaceResponseParseSuccess";
  OnReplaceResponseParseSuccessInternal(response_data, path_url, false);
}

void YSPLoginManager::OnReplaceResponseParseSuccessInternal(
  const std::string& response_data,
  const std::string& path_url,
  bool from_local) {
  DLOG(INFO) << "YSPLoginManager::OnReplaceResponseParseSuccessInternal from local ? : " << from_local;
  // save to local cache
  if (!from_local) {
    base::FilePath saveDirectoryPath = FileDirectoryPathOfReplaceCache(path_url);
    base::CreateDirectoryAndGetError(saveDirectoryPath, NULL);
    base::FilePath savePath = FilePathOfReplaceCache(path_url);
    DLOG(INFO) << "savePath:" << savePath.value();
    if (-1 == (base::WriteFile(savePath, response_data.c_str(), response_data.length()))) {
      DLOG(INFO) << "fail write cache data.";
    }
  }
}

#ifdef IE_REDCORE
void YSPLoginManager::SetIEUrlTrusted(const UrlTrusted & trust) {
  if (trust.trustUrl.empty() || trust.policy.empty())
    return;

  IInternetZoneManager *pZoneManager = NULL;
  HRESULT hResult = ::CoCreateInstance(CLSID_InternetZoneManager,
    NULL,
    CLSCTX_SERVER,
    IID_IInternetZoneManager,
    (void **)&pZoneManager);
  if (SUCCEEDED(hResult)) {
    ZONEATTRIBUTES pZoneAttributes;
    hResult = pZoneManager->GetZoneAttributes(URLZONE_TRUSTED, &pZoneAttributes);
    hResult = pZoneAttributes.dwFlags &= ~ZAFLAGS_REQUIRE_VERIFICATION;
    hResult = pZoneManager->SetZoneAttributes((DWORD)URLZONE_TRUSTED, &pZoneAttributes);
    std::map<std::wstring, std::wstring>::const_iterator policyIter = trust.policy.begin();
    for (; policyIter != trust.policy.end(); policyIter++) {
      DWORD dwPolicy = wcstol(policyIter->second.c_str(), NULL, 16);
      DWORD dwAction = wcstol(policyIter->first.c_str(), NULL, 16);
      hResult = pZoneManager->SetZoneActionPolicy(URLZONE_TRUSTED,
        dwAction,
        (BYTE *)&dwPolicy,
        sizeof(DWORD),
        URLZONEREG_DEFAULT);
    }
    pZoneManager->Release();
  }

  IInternetSecurityManager *pSecurityManager = NULL;
  hResult = CoCreateInstance(CLSID_InternetSecurityManager,
    NULL,
    CLSCTX_INPROC_SERVER,
    IID_IInternetSecurityManager,
    (void **)&pSecurityManager);

  if (SUCCEEDED(hResult)) {
    std::vector<std::wstring>::const_iterator urlIter = trust.trustUrl.begin();
    for (; urlIter != trust.trustUrl.end(); urlIter++) {
      hResult = pSecurityManager->SetZoneMapping(URLZONE_TRUSTED,
        (*urlIter).c_str(), SZM_CREATE);
    }
    pSecurityManager->Release();
  }
}

std::wstring YSPLoginManager::Utf8ToUTF16(std::string str) {
  std::wstring ret = L"";
  std::wstring_convert<std::codecvt_utf8_utf16<wchar_t>> converter;
  ret = converter.from_bytes(str);
  return ret;
}
#endif /*IE_REDCORE*/

void YSPLoginManager::OnReplaceResponseParseFailure(
  const std::string& error) {
  DLOG(INFO) << "YSPLoginManager::OnReplaceResponseParseFailure";
  LOG(INFO) << "Resource replace file download failure !";
}
//YSP+ } /*Resource Replace*/
//YSP+ { SingleSignOn config
void YSPLoginManager::OnSingleSignOnRequestFailure()
{
  DLOG(INFO) << "YSPLoginManager::OnSingleSignOnRequestFailure";
  content::BrowserThread::PostTask(
    content::BrowserThread::IO, FROM_HERE,
    base::Bind(&net::URLRequestHttpJob::setSSOConfigValue, ""));
}
void YSPLoginManager::OnSingleSignOnResponseParseSuccess(std::unique_ptr<base::DictionaryValue> response_data)
{
  if (!response_data) {
    DLOG(INFO) << "YSPLoginManager::OnSingleSignOnResponseParseSuccess ignore response.";
    return;
  }
  DLOG(INFO) << "YSPLoginManager::OnSingleSignOnResponseParseSuccess";
  OnSingleSignOnResponseParseSuccessInternal(response_data, false);
}
void YSPLoginManager::OnSingleSignOnResponseParseFailure(const std::string & error)
{
  DLOG(INFO) << "YSPLoginManager::OnSingleSignOnResponseParseFailure (" << error << ")";
  content::BrowserThread::PostTask(
    content::BrowserThread::IO, FROM_HERE,
    base::Bind(&net::URLRequestHttpJob::setSSOConfigValue, ""));
}
void YSPLoginManager::OnSingleSignOnResponseParseSuccessInternal(std::unique_ptr<base::DictionaryValue>& response_data, bool from_local)
{
  DLOG(INFO) << "YSPLoginManager::OnSingleSignOnResponseParseSuccessInternal from local ? : " << from_local;
  if (auto_login_) {
    singlesignon_info_.reset(response_data.release());
  }
  if (singlesignon_info_) {
    singlesignon_info_.get()->SetString("data.timestamp", base::Int64ToString(base::Time::Now().ToTimeT()));
  }
  std::string singlesignon_string = "";
  if (singlesignon_info_) {
    base::DictionaryValue* dataDict = nullptr;
    if (singlesignon_info_->GetDictionary("data", &dataDict)) {
      base::JSONWriter::Write(*dataDict, &singlesignon_string);
      content::BrowserThread::PostTask(
        content::BrowserThread::IO, FROM_HERE,
        base::Bind(&net::URLRequestHttpJob::setSSOConfigValue, singlesignon_string));
    }
  }
}
//YSP+ } /*SingleSignOn config*/
// YSPLoginFetcherDelegate:
void YSPLoginManager::OnLoginRequestFailure(const std::string& error) {
  LOG(INFO) << "YSPLoginManager::OnLoginRequestFailure";
  if (error != "-1" && error != "404" && error != "403" && error != "400" &&
    error != "410" && error != "502" && error != "500" && error != "501" &&
    error != "503" && error != "504" && error != "505") {
    SetLoginStatus(SATUS_LOGIN_REQUEST_FAIL);
    ntp_login_status_ = false;
  }
  base::string16 errorMessage = GetResponseErrorMessage(error);
  std::string errorMsg = base::UTF16ToUTF8(errorMessage);
  std::vector<YSPLoginManagerObserver*>::iterator iter = observers_.begin();
  for (; iter != observers_.end(); ++iter) {
    (*iter)->OnLoginRequestFailure(errorMsg);
  }
}

void YSPLoginManager::OnLoginResponseParseSuccess(std::unique_ptr<base::DictionaryValue> response_data)
{
  if (!should_parse_response_) {
    LOG(INFO) << "YSPLoginManager::OnLoginResponseParseSuccess ignore response.";
    return;
  }
  LOG(INFO) << "YSPLoginManager::OnLoginResponseParseSuccess";
  OnLoginResponseParseSuccessInternal(response_data, false);
}

void YSPLoginManager::OnLoginResponseParseSuccessInternal(std::unique_ptr<base::DictionaryValue>& response_data, bool from_local)
{
  SetLoginStatus(SATUS_LOGIN_FETCH_RECEIVED);
  DLOG(INFO) << "YSPLoginManager::OnLoginResponseParseSuccessInternal from local ? : " << from_local;
  // save to local cache
  if (!from_local) {
    if (auto_login_) {
      base::DictionaryValue* tokenDict = nullptr;
      if (login_info_) {
        login_info_->GetDictionary("data.token", &tokenDict);
        if (tokenDict && !tokenDict->empty())
          response_data->Set("data.token", std::make_unique<base::Value>(tokenDict->DeepCopy()));
      }
      base::FilePath savePath = FilePathOfLoginCache("login");
      //YSP+ { config cryptor
      std::string response_datastring;
      std::string enc_response;
      base::JSONWriter::Write(*response_data, &response_datastring);
      ConfigCryptor AES_crypted_;
      LOG(INFO) << " login response: " << response_datastring;
      AES_crypted_.EncryptString(cryptorkey::key, response_datastring, &enc_response);
      if (-1 == (base::WriteFile(savePath, enc_response.data(), enc_response.length()))) {
        DLOG(INFO) << "fail write login cache data.";
      }
      //YSP+ } /*config cryptor*/
    }
    login_info_.reset(response_data.release());
    DLOG(INFO) << "login_info_ access token: " << GetAccessTokenForLogin();
    if (!GetAccessTokenForLogin().empty() && !GetRefreshTokenForLogin().empty()) {
      SaveLastId(prefs::kYSPAccessToken, GetAccessTokenForLogin());
      SaveLastId(prefs::kYSPRefreshToken, GetRefreshTokenForLogin());
    }
  }
  std::string status = GetLoginStatusCode();
  loginStatusCode_ = (status == "0");
  if (loginStatusCode_) {
    SaveLastId(prefs::kYSPLoginLastCID, cid_);
    SaveLastId(prefs::kYSPLoginLastUID, account_);
    SaveLastId(prefs::kYSPLoginLastPWD, password_);

    SetLoginStatus(SATUS_VERIFY_ACCOUNT_SUCCESS);
    // should dispatch the satus to the login page.
    SetLoginStatus(SATUS_LOGIN_FETCH_RESOURCE_START);
    //YSP+ { Fetcher resource
    GetApplicationfetcher(false);
    GetStrategyfetcher(false);
    GetTokenfetcher(false);
    GetSwafetcher(false);
    GetPcfetcher(false);
    //YSP+ } /*Fetcher resource*/
  }
  else {
    if (status == "E1002")
      GetAuthTokenfetcher(GetManageServer() + kLoginUploadPath, false);
    else if (status == "E1003") {
      std::vector<YSPLoginManagerObserver*>::iterator iter = observers_.begin();
      for (; iter != observers_.end(); ++iter) {
        (*iter)->OnTokenStatusChanged("TokenExpired");
      }
    }
    else {
      if (status == "E3002") {
        std::vector<YSPLoginManagerObserver*>::iterator iter = observers_.begin();
        for (; iter != observers_.end(); ++iter) {
          (*iter)->OnTokenStatusChanged("failure");
        }
      }
    else if (status == "E4002") {
      Logout();
    }

      SetLoginStatus(SATUS_VERIFY_ACCOUNT_FAIL);
    ntp_login_status_ = false;
      NotifyFailure();
      ClearCache();
    }

    return;
  }

  //YSP+ { SingleSignOn config
  {
    if (singlesignon_status_) {
      GetSingleSignOnConfig();
      singlesignon_status_ = false;
    }
  }
  //YSP+ } /* SingleSignOn config */

}

void YSPLoginManager::OnLoginResponseParseFailure(const std::string& error)
{
  LOG(INFO) << "YSPLoginManager::OnLoginResponseParseFailure";
  SetLoginStatus(SATUS_LOGIN_RESPONSE_FAIL);
  ntp_login_status_ = false;
  base::string16 errorMessage = GetResponseErrorMessage(error);
  std::string errorMsg = base::UTF16ToUTF8(errorMessage);
  std::vector<YSPLoginManagerObserver*>::iterator iter = observers_.begin();
  for (; iter != observers_.end(); ++iter) {
    (*iter)->OnLoginResponseParseFailure(errorMsg);
  }
}
//YSP+ { Fetcher resource
void YSPLoginManager::OnFetcherResourceRequestFailure(const GURL & url, bool auto_fecth, const std::string& error)
{
  LOG(INFO) << "YSPLoginManager::OnFetcherResourceRequestFailure original url:" << url;
  if (error != "-1" && error != "404" && error != "403" && error != "400" &&
    error != "410" && error != "502" && error != "500" && error != "501" &&
    error != "503" && error != "504" && error != "505") {
    if (!auto_fecth) {
      SetLoginStatus(SATUS_LOGIN_FETCH_RESOURCE_REQUEST_FAIL);
      ntp_login_status_ = false;
    }
  }
  base::string16 errorMessage = GetResponseErrorMessage(error);
  std::string errorMsg = base::UTF16ToUTF8(errorMessage);
  std::vector<YSPLoginManagerObserver*>::iterator iter = observers_.begin();
  for (; iter != observers_.end(); ++iter) {
    (*iter)->OnLoginRequestFailure(errorMsg);
  }
}

void YSPLoginManager::OnFetcherResourceResponseParseSuccess(const GURL & url, std::unique_ptr<base::DictionaryValue> response_data, bool auto_fetch)
{
  if (!should_parse_response_) {
    LOG(INFO) << "YSPLoginManager::OnFetcherResourceResponseParseSuccess ignore response. original url:" << url;
    return;
  }
  LOG(INFO) << "YSPLoginManager::OnFetcherResourceResponseParseSuccess original url:" << url << " auto_fetch:" << auto_fetch;
  OnFetcherResourceResponseParseSuccessInternal(url, response_data, false, auto_fetch);
}

void YSPLoginManager::OnFetcherResourceResponseParseSuccessInternal(const GURL & url, std::unique_ptr<base::DictionaryValue>& response_data, bool from_local, bool auto_fetch)
{
  std::string responseStatus = GetResponseStatusCode(response_data);
  LOG(INFO) << "responseStatus: " << responseStatus;
  bool statusCode = (responseStatus == "0");
  if (!auto_fetch)
    SetLoginStatus(SATUS_LOGIN_FETCH_RESOURCE_RECEIVED);
  if (statusCode) {
    if (url.spec().find(kGetApplicationPath) != std::string::npos) {
      OnApplicationFetcherResponseParse(response_data, from_local, auto_fetch);
    }
    else if (url.spec().find(kGetConfigurationPCPath) != std::string::npos) {
      OnPcFetcherResponseParse(response_data, from_local, auto_fetch);
    }
    else if (url.spec().find(kGetConfigurationStrategyPath) != std::string::npos) {
      OnStrategyFetcherResponseParse(response_data, from_local, auto_fetch);
    }
    else if (url.spec().find(kGetSsoTokenPath) != std::string::npos) {
      OnTokenFetcherResponseParse(response_data, from_local, auto_fetch);
    }
    else if (url.spec().find(kGetSWAPath) != std::string::npos) {
      OnSwaFetcherResponseParse(response_data, from_local, auto_fetch);
    }
    else if (url.spec().find(KGetUserPath) != std::string::npos) {
      OnUserFetcherResponseParse(response_data, from_local, auto_fetch);
    }
    else if (url.spec().find(kGetAuthTokenPath) != std::string::npos) {
      OnAutoTokenFetcherResponseParse(response_data, from_local, auto_fetch);
    }
    else if (url.spec().find(kSdpDevicePath) != std::string::npos) {
      OnSdpDeviceFetcherResponseParse(response_data, from_local, auto_fetch);
    }
  LOG(INFO) << "loginStatusCode_: " << loginStatusCode_ << " applicationStatusCode_: " << applicationStatusCode_
    << " pcStatusCode_: " << pcStatusCode_ << " strategyStatusCode_: " << strategyStatusCode_
    << " tokenStatusCode_: " << tokenStatusCode_ << " swaStatusCode_: " << swaStatusCode_;
    if (loginStatusCode_ && login_info_ && application_info_ && pc_info_ &&
    strategy_info_ && token_info_ && swa_info_) {
      PrefService* prefs = g_browser_process->local_state();
      prefs->SetBoolean(prefs::kYSPFirstLogin, false);
      if (!auto_fetch)
        SetLoginStatus(SATUS_VERIFY_CONFIG_SUCCESS); //ysp+ { auto get config }
      ntp_login_status_ = true;

    if (!auto_fetch) {
      std::vector<YSPLoginManagerObserver*>::iterator iter = observers_.begin();
      for (; iter != observers_.end(); ++iter) {
        (*iter)->OnTokenStatusChanged("TokenAvailable");
      }
    }

    UpdateLoginManagerSettings();

      if (!auto_fetch) {
    base::string16 userName = GetYSPUserName();
    std::string headImageUrl = GetHeadImageUrl();
        std::vector<YSPLoginManagerObserver*>::iterator iter = observers_.begin();
        for (; iter != observers_.end(); ++iter) {
          (*iter)->OnLoginSuccess(userName, headImageUrl);
        }
      }
      //YSP+ { Resource Replace
      {
        DLOG(INFO) << "YSPLoginManager::OnLoginResponseParseSuccessInternal download online";
        base::DictionaryValue* rootDict = GetManagedResourceReplace();
        base::ListValue* resourceReplace = nullptr;
        if (rootDict && rootDict->GetList("resourceReplace", &resourceReplace)) {
          if (resourceReplace && !resourceReplace->empty()) {
            for (size_t i = 0; i < resourceReplace->GetSize(); ++i) {
              base::DictionaryValue* bmDict = nullptr;
              if (resourceReplace->GetDictionary(i, &bmDict)) {
                std::string source_url, dest_url;
                bmDict->GetString("sourceUrl", &source_url);
                bmDict->GetString("destUrl", &dest_url);
                DLOG(INFO) << "sourceUrl: " << source_url << " destUrl: " << dest_url;
                if (!dest_url.empty() && !source_url.empty()) {
                  YSPReplaceFetcher* replace_fetcher = new YSPReplaceFetcher(this, g_browser_process->system_request_context());
                  if (replace_fetcher)
                    replace_fetcher->StartGetReplace(dest_url, cid_, source_url);
                }
              }
            }
          }
        }
      }
      //YSP+ } /* Resource Replace */
    }
  }
  else {
    if (url.spec().find(kGetAuthTokenPath) != std::string::npos && (responseStatus == "E1003")) {
      std::vector<YSPLoginManagerObserver*>::iterator iter = observers_.begin();
      for (; iter != observers_.end(); ++iter) {
        (*iter)->OnTokenStatusChanged("TokenExpired");
      }
    }
    else if (responseStatus == "E1002") {
      GetAuthTokenfetcher(url.spec(), false);
    }
    else {
      if (!auto_fetch) {
        SetLoginStatus(SATUS_LOGIN_RESPONSE_FAIL); //ysp+ { auto get config }
    ntp_login_status_ = false;
        NotifyFailure();
      }
      ClearCache();
    }
  }
}

void YSPLoginManager::OnFetcherResourceResponseParseFailure(const GURL & url, bool auto_fecth, const std::string & error)
{
  LOG(INFO) << "YSPLoginManager::OnFetcherResourceResponseParseFailure original url:" << url;
  if (!auto_fecth) {
    SetLoginStatus(SATUS_LOGIN_FETCH_RESOURCE_RESPONSE_FAIL);
    ntp_login_status_ = false;
  }
  base::string16 errorMessage = GetResponseErrorMessage(error);
  std::string errorMsg = base::UTF16ToUTF8(errorMessage);
  std::vector<YSPLoginManagerObserver*>::iterator iter = observers_.begin();
  for (; iter != observers_.end(); ++iter) {
    (*iter)->OnLoginResponseParseFailure(errorMsg);
  }
}

void YSPLoginManager::OnAutoConfigParseSuccess(
    std::unique_ptr<base::DictionaryValue> response_data, bool auto_fetch) {
  if (response_data->empty())
    return;
  std::string autoConfigStatus = GetResponseStatusCode(response_data);
  if (autoConfigStatus == "E1002")
    GetAuthTokenfetcher(GetManageServer() + kAutoGetConfigPath, false);
  if (!auto_config_info_) {
    auto_config_info_.reset(response_data.release());
    return;
  }
  std::string activeId = "";
  std::string lastLoginTime = "";
  response_data->GetString("data.activeStrategyId", &activeId);
  if (!activeId.empty() && login_info_)
    login_info_->SetString("data.strategy.activeId", activeId);
  response_data->GetString("data.time", &lastLoginTime);
  if (!lastLoginTime.empty() && login_info_) {
    login_info_->SetString("data.user.lastLoginTime", lastLoginTime);
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
  base::ListValue* newList = nullptr;
  response_data->GetList("data.versionList", &newList);
  base::ListValue* oldList = nullptr;
  auto_config_info_->GetList("data.versionList", &oldList);
  if (newList && oldList) {
    for (size_t i = 0; i < oldList->GetSize(); i++) {
      base::DictionaryValue* newDict = nullptr;
      base::DictionaryValue* oldDict = nullptr;
      int type = 0, newVersion = 0, oldVersion = 0;
      oldList->GetDictionary(i, &oldDict);
      newList->GetDictionary(i, &newDict);
      if (oldDict && newDict) {
        oldDict->GetInteger("version", &oldVersion);
        newDict->GetInteger("version", &newVersion);
        if (oldVersion != newVersion) {
          oldDict->SetInteger("version", newVersion);
          oldDict->GetInteger("type", &type);
          if (type == 1) {
            if (login_info_)
              login_info_->SetInteger("data.strategy.version", newVersion);
            GetApplicationfetcher(auto_fetch);
            GetStrategyfetcher(auto_fetch);
      GetSwafetcher(auto_fetch);
          }
          else if (type == 2) {
            GetPcfetcher(auto_fetch);
          }
          else if (type == 3) {
            GetApplicationfetcher(auto_fetch);
      GetSwafetcher(auto_fetch);
          }
      else if (type == 5) {
        GetUserfetcher(auto_fetch);
        GetStrategyfetcher(auto_fetch);
        GetSwafetcher(auto_fetch);
      }
          else if (type == 6) {
            GetSwafetcher(auto_fetch);
          }
        }
      }
    }
  }
  auto_config_info_.reset(response_data.release());
}

//YSP+ } /*Fetcher resource*/
void YSPLoginManager::AddObserver(YSPLoginManagerObserver* o) {
  if (!o)
    return;

  std::vector<YSPLoginManagerObserver*>::iterator iter =
    std::find(observers_.begin(), observers_.end(), o);
  if (iter == observers_.end()) {
    observers_.push_back(o);
  }
}

void YSPLoginManager::RemoveObserver(YSPLoginManagerObserver* o) {
  std::vector<YSPLoginManagerObserver*>::iterator iter =
    std::find(observers_.begin(), observers_.end(), o);
  if (iter != observers_.end()) {
    observers_.erase(iter);
  }
}

std::string YSPLoginManager::generateUserInfoForSettings()
{
  std::string result = "";
  if (login_info_) {
    base::DictionaryValue* dataDict = nullptr;
    if (login_info_->GetDictionary("data", &dataDict)) {
      if (dataDict)
      {
        base::DictionaryValue retDict;
        std::string ret;
        if (dataDict->GetString("user.id", &ret)) {
          retDict.SetString("userId", ret);
        }
        if (dataDict->GetString("company.id", &ret)) {
          retDict.SetString("companyId", ret);
        }

        if (dataDict->GetString("user.name", &ret)) {
          retDict.SetString("name", ret);
        }

        if (dataDict->GetString("user.email", &ret)) {
          retDict.SetString("email", ret);
        }
        if (dataDict->GetString("user.mobile", &ret)) {
          retDict.SetString("mobile", ret);
        }
        if (dataDict->GetString("user.avatarPath", &ret)) {
          retDict.SetString("avatarPath", ret);
        }
    {
      base::string16 userName = GetYSPUserName();
      base::char16 headName = 0;
      size_t pos = 0;
      if (!userName.empty()) {
        headName = userName[0];
        if ((headName < L'a' || headName > L'z') && (headName < L'A' || headName > L'Z')) {
          pos = userName.length() > 2 ? userName.length() - 2 : 0;
          retDict.SetString("headName", base::string16(userName, pos));
        }
        else
          retDict.SetString("headName", base::string16(1, headName));
      }
      SkColor headColor = kAvatarBackground[(int)headName % kAvatarBackgroundSize];
      retDict.SetDouble("headColor", headColor);
    }
        {
          const std::string id("managerAddress");
          retDict.SetString(id, GetManageServer());
        }


        base::JSONWriter::Write(retDict, &result);
      }
    }
  }
  return result;
}

std::string YSPLoginManager::GetValueForKey(
  std::string& key) {
  std::string result = "";
  if (key.compare("loggingStatus") == 0) {
    result = ntp_login_status_ ? "100" : "-1";
  }
  else if (key.compare("autoLogin") == 0) {
    result = auto_login_ ? "1" : "0";
  }
  else if (key.compare("address") == 0) {
    result = GetManageServer();
  }
  else if (key.compare("domain") == 0) {
    result = cid_;
  }
  else if (key.compare("companyId") == 0) {
    result = GetCompanyId();
  }
  else if (key.compare("accountInfo") == 0) {
    result = GetUserInfoAsJSONString();
  }
  else if (key.compare("lastUuid") == 0) {
    result = account_; // 用户名
  }
  else if (key.compare("onlyid") == 0) {
    result = GetStringFromDictionary(login_info_.get(), "data.user.id"); // uuid
  }
  else if (key.compare("password") == 0) {
    result = password_;
  }
  else if (key.compare("appGroupList") == 0) {
    result = GetDataJSAsJSONString();
  }
  else if (key.compare("firstLogin") == 0) {
    PrefService* prefs = g_browser_process->local_state();
    bool firstLogin = prefs->GetBoolean(prefs::kYSPFirstLogin);
    result = (firstLogin || (password_.empty() && account_.empty())) ? "1" : "0";
  }
  //ysp+{push server api}
  else if (key.compare("deviceId") == 0) {
    PrefService* prefs = g_browser_process->local_state();
    std::string strDeviceId = GetRegMachineId();
    if (strDeviceId.empty())
      strDeviceId = prefs->GetString(prefs::kYSPDeviceID);
    if (strDeviceId.empty()) {
      strDeviceId = base::GenerateGUID();
      prefs->SetString(prefs::kYSPDeviceID, strDeviceId);
    }
    result = strDeviceId;
  }
  //ysp+
  //ysp+ { show config
  else if (key.compare("ShowConfig") == 0) {
    result = GetShowConfigAsJSONString();
  }
  else if (key.compare("managerAddress") == 0) {
    result = GetManageServer();
  }
  else if (key.compare("lastLoginInfo") == 0) {
    base::DictionaryValue dataDict;
    const std::string domain("domain");
    dataDict.SetString(domain, GetLastCID());
    const std::string userid("userid");
    dataDict.SetString(userid, GetLastUID());
    base::JSONWriter::Write(dataDict, &result);
  }
  //ysp+ } /*show config*/
  return result;
}
//YSP+ { passwords AD manager
std::string YSPLoginManager::GetUserInfoForKey(int key)
{
  std::string result = "";
  std::string loginName = "";
  std::string email = "";
  std::string mobile = "";
  if (login_info_) {
    login_info_->GetString("data.user.email", &email);
    login_info_->GetString("data.user.mobile", &mobile);
    login_info_->GetString("data.user.loginName", &loginName);
  }
  if (key == 1) {
    result = loginName;
  }
  else if (key == 2) {
    result = email;
  }
  else if (key == 3) {
    if (!email.empty()) {
      size_t offset = email.find('@', 0);
      if (offset != std::string::npos)
        result.assign(email, 0, offset);
    }
  }
  else if (key == 4) {
    result = mobile;
  }
  return result;
}
//YSP+ } /*passwords AD manager*/
//ysp+ { auto get config
bool YSPLoginManager::GetLoginStatus()
{
  return ntp_login_status_;
}

bool YSPLoginManager::GetAutoLoginStatus()
{
  return login_status_ > 0;
}

//ysp+ } /*auto get config*/
std::string YSPLoginManager::GetLoginInfo() {
  std::string result = "";
  base::DictionaryValue retDict;
  base::DictionaryValue dict;
  std::string ret;

  base::DictionaryValue* dataDict = nullptr;
  if (!login_info_ || GetManagedADProxyAuth() == nullptr)
    return std::string();
  if (login_info_->GetDictionary("data", &dataDict)) {
    base::DictionaryValue* userDict = nullptr;
    if (dataDict && dataDict->GetDictionary("user", &userDict)) {
      {
        const std::string id("password");
        dict.SetString(id, password_);
      }

      if (userDict->GetString("email", &ret))
      {
        const std::string id("email");
        dict.SetString(id, ret);
      }

      if (userDict->GetString("id", &ret))
      {
        const std::string id("id");
        dict.SetString(id, ret);
      }

      if (userDict->GetString("mobile", &ret))
      {
        const std::string id("mobile");
        dict.SetString(id, ret);
      }

      if (userDict->GetString("loginName", &ret)) {
        const std::string id("loginName");
        dict.SetString(id, ret);
      }
      // FIXME(halton):
      base::DictionaryValue* uDict = dict.DeepCopy();
      const std::string id("user");
      retDict.Set(id, std::make_unique<base::Value>(uDict));
    }
  }

  {
    // FIXME(halton):
    const std::string id("swaList");
    base::ListValue* swaList = GetManagedADProxyAuth()->DeepCopy();
    retDict.Set(id, std::make_unique<base::Value>(swaList));
  }

  base::JSONWriter::Write(retDict, &result);
  return result;
}

//YSP+ { app version
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
//YSP+ } /*app version*/

std::string YSPLoginManager::GetDeviceInfo() {
  std::string strMac = GetLocalMacAdd();
  PrefService* prefs = g_browser_process->local_state();
  std::string strDeviceId = GetRegMachineId();
  if (strDeviceId.empty())
    strDeviceId = prefs->GetString(prefs::kYSPDeviceID);
  if (strDeviceId.empty()) {
    strDeviceId = base::GenerateGUID();
    prefs->SetString(prefs::kYSPDeviceID, strDeviceId);
  }
  std::string BoardInfo = "";
  GetBoardInfo(&BoardInfo);
  DLOG(INFO) << "YSPLoginManager::GetDeviceInfo, device id: " << strDeviceId;

  base::DictionaryValue dic;
  dic.SetString("systemVersion", GetOsVersionStr());
  dic.SetString("appVersion", BuildYSPBrowserVersionString());
  dic.SetString("deviceId", strDeviceId);
  dic.SetString("model", BoardInfo);
  dic.SetString("architecture", /*IsWow64()*/true ? "64" : "32");
  dic.SetString("platform", "1");

  std::string strDeviceInfo;
  base::JSONWriter::Write(dic, &strDeviceInfo);

  return strDeviceInfo;
}

bool YSPLoginManager::HasManagedBookmarks() {
  return (GetManagedBookmarks() != nullptr);
}

base::DictionaryValue* YSPLoginManager::GetManagedBookmarks() {
  if (login_info_) {
    base::DictionaryValue* dataDict = nullptr;
    if (login_info_->GetDictionary("data", &dataDict)) {
      base::DictionaryValue* dict = nullptr;
      if (dataDict && dataDict->GetDictionary("managedBookmarks.roots.bookmark_bar", &dict)) {
        return dict;
      }
    }
  }

  return nullptr;
}
//------------------------------------------
std::string YSPLoginManager::GetUserAgent() {
  std::string userAgent;
  if (login_info_) {
    base::DictionaryValue* dataDict = nullptr;
    if (login_info_->GetDictionary("data", &dataDict)) {
      if (dataDict)
        dataDict->GetString("userAgent", &userAgent);
    }
  }

  return userAgent;
}

//ysp+ { auto get config---------------------------------------
std::string YSPLoginManager::GetAutoConfigUrl() {
  if (login_info_) {
    base::DictionaryValue* dataDict = nullptr;
    if (login_info_->GetDictionary("data", &dataDict)) {
      base::DictionaryValue* funcDict = nullptr;
      if (dataDict && dataDict->GetDictionary("functionControl", &funcDict)) {
        std::string autoConfigUrl;
        if (funcDict && funcDict->GetString("ConfigVersionUrl", &autoConfigUrl))
          return autoConfigUrl;
      }
    }
  }

  return std::string();
}

int YSPLoginManager::GetAutoConfigDelta() {
  return GetFunctionControlIntWithDefault("IntervalTime", 60);
}
//ysp+ } /*auto get config*/

base::DictionaryValue* YSPLoginManager::GetAutoUpgrade() {
  if (pc_info_) {
    base::DictionaryValue* dataDict = nullptr;
    if (pc_info_->GetDictionary("data", &dataDict)) {
      base::DictionaryValue* dict = nullptr;
      if (dataDict && dataDict->GetDictionary("autoUpgrade", &dict)) {
        return dict;
      }
    }
  }

  return nullptr;
}

//ysp+ { URL Blacklist And Whitelist
base::DictionaryValue* YSPLoginManager::GetWebsiteListEnabled() {
  if (strategy_info_) {
    base::DictionaryValue* dataDict = nullptr;
    if (strategy_info_->GetDictionary("data", &dataDict)) {
      base::DictionaryValue* dict = nullptr;
      if (dataDict && dataDict->GetDictionary("isWebsiteListEnabled", &dict)) {
        return dict;
      }
    }
  }

  return nullptr;
}
//ysp+ } //URL Blacklist And Whitelist
//ysp+ { Resource Replace
base::DictionaryValue* YSPLoginManager::GetManagedResourceReplace() {
  if (pc_info_) {
    base::DictionaryValue* dataDict = nullptr;
    if (pc_info_->GetDictionary("data", &dataDict)) {
      base::DictionaryValue* dict = nullptr;
      if (dataDict && dataDict->GetDictionary("resourceOverload", &dict)) {
        return dict;
      }
    }
  }

  return nullptr;
}
//ysp + } //Resource Replace
//ysp+ { Kernel switching
base::DictionaryValue* YSPLoginManager::GetManagedKernels() {
  if (pc_info_) {
    base::DictionaryValue* dataDict = nullptr;
    if (pc_info_->GetDictionary("data", &dataDict)) {
      base::DictionaryValue* dict = nullptr;
      if (dataDict && dataDict->GetDictionary("siteCompatibility", &dict)) {
        return dict;
      }
    }
  }

  return nullptr;
}

std::string YSPLoginManager::GetDefaultCore() {
  std::string defaultCore;
  if (pc_info_) {
    base::DictionaryValue* dataDict = nullptr;
    if (pc_info_->GetDictionary("data", &dataDict)) {
      base::DictionaryValue* kernelDict = nullptr;
      if (dataDict && dataDict->GetDictionary("siteCompatibility", &kernelDict)) {
        if (kernelDict)
          kernelDict->GetString("defaultCore", &defaultCore);
      }
    }
  }

  return defaultCore;
}
//ysp+ } //Kernel switching

//YSP+ { sangfor GM ssl
base::DictionaryValue* YSPLoginManager::GetManagedGMStream() {
  if (pc_info_) {
    base::DictionaryValue* dataDict = nullptr;
    if (pc_info_->GetDictionary("data", &dataDict)) {
      if (dataDict) {
        return dataDict;
      }
    }
  }

  return nullptr;
}
//YSP+ } //sangfor GM ssl

//YSP+ { SingleSignOn config
void YSPLoginManager::GetSingleSignOnConfig()
{
  DLOG(INFO) << "YSPLoginManager::GetSingleSignOnConfig SingleSignOn config online";
  if (login_info_) {
    if (GetUserId().empty() || GetCompanyId().empty())
      return;
    std::string post_data;
    net::AddMultipartValueForUpload("companyId", GetCompanyId(), kMultipartBoundary, "", &post_data);
    net::AddMultipartValueForUpload("username", account_, kMultipartBoundary, "", &post_data);
    net::AddMultipartValueForUpload("password", password_, kMultipartBoundary, "", &post_data);
    net::AddMultipartFinalDelimiterForUpload(kMultipartBoundary, &post_data);
    std::vector<std::string> header_list;
    header_list.push_back("userId: " + GetUserId());
    YSPFetcherResource* singlesignon_fetcher_ = new YSPFetcherResource(this, g_browser_process->system_request_context());
    if (singlesignon_fetcher_) {
      singlesignon_fetcher_->StarFetcherResource(
        net::URLFetcher::POST,
        GetManageServer() + kGetSsoTokenPath,
        header_list,
        post_data,
        false);
    }
  }
}

base::DictionaryValue* YSPLoginManager::GetManagedSingleSignOnConfig() {
  if (token_info_) {
    base::DictionaryValue* dataDict = nullptr;
    if (token_info_->GetDictionary("data", &dataDict)) {
      return dataDict;
    }
  }

  return nullptr;
}
//YSP+ } /*SingleSignOn config*/

//ysp+ { passwords AD manager
base::ListValue* YSPLoginManager::GetManagedADProxyAuth() {
  if (swa_info_ && !swa_info_->empty()) {
    base::ListValue* dataDict = nullptr;
    if (swa_info_->GetList("data", &dataDict)) {
      if (dataDict && !dataDict->empty()) {
        return dataDict;
      }
    }
  }

  return nullptr;
}
//ysp + } /*passwords AD manager*/

//ysp+ { AES DES and SMS4 crypt
std::string YSPLoginManager::GetEncryptionAndKey() {
  return GetCryptKey();
}
//ysp+ } //AES DES and SMS4 crypt

//YSP+ { cache encryption
bool YSPLoginManager::GetCacheEncryption() {
  return GetCacheCrypt();
}
//YSP+ }

//ysp+ { crypto http header
std::string YSPLoginManager::GetCryptoHeaderKey() {
  if (pc_info_) {
    base::DictionaryValue* dataDict = nullptr;
    if (pc_info_->GetDictionary("data", &dataDict)) {
      base::DictionaryValue* funcDict = nullptr;
      if (dataDict && dataDict->GetDictionary("httpHeadersCertify", &funcDict)) {
        std::string cryptkey;
        if (funcDict && funcDict->GetString("setting.User-Agent", &cryptkey))
          return cryptkey;
      }
    }
  }

  return std::string();
}

std::string YSPLoginManager::GetUserAgentTypes()
{
  if (pc_info_) {
    base::DictionaryValue* dataDict = nullptr;
    if (pc_info_->GetDictionary("data", &dataDict)) {
      base::DictionaryValue* funcDict = nullptr;
      if (dataDict && dataDict->GetDictionary("httpHeadersCertify", &funcDict)) {
        std::string uaTypes;
        if (funcDict && funcDict->GetString("setting.UAtypes", &uaTypes))
          return uaTypes;
      }
    }
  }

  return std::string();
}
//ysp+ } /*crypto http header*/
//YSP+ { lock screen time
int YSPLoginManager::GetLockScreenTime()
{
  if (pc_info_) {
    base::DictionaryValue* dataDict = nullptr;
    if (pc_info_->GetDictionary("data", &dataDict)) {
      base::DictionaryValue* funcDict = nullptr;
      if (dataDict && dataDict->GetDictionary("lockScreenTime", &funcDict)) {
        int lockScreenTime = 0;
        if (funcDict && funcDict->GetInteger("value", &lockScreenTime))
          return lockScreenTime;
      }
    }
  }

  return 0;
}
//ysp+ } /*lock screen time*/
//YSP+ { window popup
base::DictionaryValue* YSPLoginManager::GetPopupSetting() {
  if (pc_info_) {
    base::DictionaryValue* dataDict = nullptr;
    if (pc_info_->GetDictionary("data", &dataDict)) {
      base::DictionaryValue* dict = nullptr;
      if (dataDict && dataDict->GetDictionary("popUpBlockerSetting", &dict)) {
        return dict;
      }
    }
  }

  return nullptr;
}
//YSP+ } /*window popup*/

//TODO (matianzhi): YSP+ { startup and home pages
base::ListValue * YSPLoginManager::GetStartupPages()
{
  if (pc_info_) {
    base::ListValue* dictList = nullptr;
    if (pc_info_->GetList("data.startPageSetting.list", &dictList)) {
      return dictList;
    }
  }
  return nullptr;
}

bool YSPLoginManager::isStartupPages() {
  bool defalutMainPage = false;
  if (pc_info_) {
    int status = 1;
    pc_info_->GetInteger("data.startPageSetting.value", &status);
    if (status == 2)
      defalutMainPage = true;
  }
  return defalutMainPage;
}
//YSP+ } /*startup and home pages*/

//YSP+ { doc online preview
base::DictionaryValue * YSPLoginManager::GetPreviewDocOnline()
{
  if (pc_info_) {
    base::DictionaryValue* dataDict = nullptr;
    if (pc_info_->GetDictionary("data", &dataDict)) {
      base::DictionaryValue* docDict = nullptr;
      if (dataDict && dataDict->GetDictionary("docOnlinePreview.setting", &docDict)) {
        return docDict;
      }
    }
  }
  return nullptr;
}

bool YSPLoginManager::GetPreviewDocOnlineEnable() {
  bool status = false;
  if (pc_info_) {
    base::DictionaryValue* dataDict = nullptr;
    if (pc_info_->GetDictionary("data", &dataDict)) {
      if (dataDict->GetBoolean("docOnlinePreview.value", &status)) {
        return status;
      }
    }
  }
  return status;
}
//YSP+ } /*doc online preview*/

//ysp+ { private DNS
base::DictionaryValue * YSPLoginManager::GetPrivateDNS()
{
  if (pc_info_) {
    base::DictionaryValue* dataDict = nullptr;
    if (pc_info_->GetDictionary("data", &dataDict)) {
      base::DictionaryValue* docDict = nullptr;
      if (dataDict && dataDict->GetDictionary("privateDNS", &docDict)) {
        return docDict;
      }
    }
  }
  return nullptr;
}
//ysp+ }

void YSPLoginManager::AddHeaders() {
  if (!pc_info_)
    return;

  base::DictionaryValue* dataDict = nullptr;
  if (pc_info_->GetDictionary("data", &dataDict)) {
    base::DictionaryValue* headerDict = nullptr;
    if (dataDict && dataDict->GetDictionary("httpHeadersCertify.setting", &headerDict)) {
      if (headerDict) {
        base::DictionaryValue::Iterator iter(*headerDict);
        for (; !iter.IsAtEnd(); iter.Advance()) {
          if (iter.value().is_string()) {
            std::string value;
            iter.value().GetAsString(&value);
            net::URLRequestHttpJob::addGlobalHeader(iter.key(), value);
          }
        }
      }
    }
  }
}

bool YSPLoginManager::GetFunctionControlBool(std::string key, bool Default) {
  bool functionEnabled = Default;
  if (strategy_info_ && !key.empty()) {
    base::DictionaryValue* dataDict = nullptr;
    if (strategy_info_->GetDictionary("data", &dataDict)) {
      base::DictionaryValue* dict = nullptr;
      if (dataDict && dataDict->GetDictionary(key, &dict)) {
        if (dict) {
          dict->GetBoolean("status", &functionEnabled);
        }
      }
    }
  }

  return !functionEnabled;
}

int YSPLoginManager::GetFunctionControlInt(std::string key) {
  return GetFunctionControlIntWithDefault(key, 0);
}

int YSPLoginManager::GetFunctionControlIntWithDefault(
  std::string key, int Default) {
  int value = Default;
  if (strategy_info_ && !key.empty()) {
    base::DictionaryValue* dataDict = nullptr;
    if (strategy_info_->GetDictionary("data", &dataDict)) {
      if (dataDict) {
        dataDict->GetInteger(key, &value);
      }
    }
  }

  return value;
}

double YSPLoginManager::GetFunctionControlDoubleWithDefault(
  std::string key, double Default) {
  double value = Default;
  if (strategy_info_ && !key.empty()) {
    base::DictionaryValue* dataDict = nullptr;
    if (strategy_info_->GetDictionary("data", &dataDict)) {
      if (dataDict) {
        dataDict->GetDouble(key, &value);
      }
    }
  }

  return value;
}
//----------------------------------------
base::string16 YSPLoginManager::GetFunctionControlString(std::string key) {
  base::string16 value;
  if (strategy_info_ && !key.empty()) {
    base::DictionaryValue* dataDict = nullptr;
    if (strategy_info_->GetDictionary("data", &dataDict)) {
      base::DictionaryValue* dict = nullptr;
      if (dataDict && dataDict->GetDictionary("functionControl", &dict)) {
        if (dict) {
          dict->GetString(key, &value);
        }
      }
    }
  }

  return value;
}

std::vector<base::string16> YSPLoginManager::GetFunctionControlListString(std::string key) {
  std::vector<base::string16> vec;
  if (strategy_info_ && !key.empty()) {
    base::DictionaryValue* dataDict = nullptr;
    if (strategy_info_->GetDictionary("data", &dataDict)) {
      base::ListValue* list = nullptr;
      if (dataDict && dataDict->GetList(key, &list)) {
        if (list) {
          for (size_t i = 0; i < list->GetSize(); i++) {
            std::string listKey = "", str = "";
            list->GetString(i, &listKey);
            if (login_info_ && !listKey.empty()) {
              str = listKey;
              if (listKey == "?timestamp?")
                login_info_->GetString("data.user.lastLoginTime", &str);
              if (listKey == "?deviceId?")
                str = GetRegMachineId();
              if (!str.empty())
                vec.push_back(base::UTF8ToUTF16(str));
            }
          }
        }
      }
    }
  }

  return vec;
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
  window_frame_color_ = GetFunctionControlIntWithDefault("windowFrameColor", -1);
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
  uint32_t color = (uint32_t)GetFunctionControlDoubleWithDefault("isWatermarkEnabled.watermarkColor", 0);
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

void YSPLoginManager::SaveLastId(const std::string& key, const std::string& value) {
  if (key.empty())
    return;

  PrefService* prefs = g_browser_process->local_state();
  if (value.empty())
    prefs->SetString(key, "");
  else
  {
    std::string base64String;
    base::Base64Encode(value, &base64String);
    prefs->SetString(key, base64String);
  }
}

std::string YSPLoginManager::GetLastId(const std::string& key) {
  PrefService* prefs = g_browser_process->local_state();
  std::string base64String = prefs->GetString(key);
  if (base64String.empty())
    return base64String;

  std::string decodedString;
  base::Base64Decode(base64String, &decodedString);
  return decodedString;
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
    base::DictionaryValue* dataDict = nullptr;
    if (strategy_info_->GetDictionary("data", &dataDict)) {
      dataDict->GetBoolean("isWebsiteListEnabled.status", &status);
    }
  }
  return status;
}

std::string YSPLoginManager::GetWebsiteListType() {
  std::string type = "";
  if (strategy_info_) {
    base::DictionaryValue* dataDict = nullptr;
    if (strategy_info_->GetDictionary("data", &dataDict)) {
      dataDict->GetString("isWebsiteListEnabled.type", &type);
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

  if (!(url.SchemeIs(url::kAboutScheme) ||
    url.SchemeIs(url::kBlobScheme) ||
    url.SchemeIs(url::kContentScheme) ||
    url.SchemeIs(url::kDataScheme) ||
    url.SchemeIs(url::kFileScheme) ||
    url.SchemeIs(url::kFileSystemScheme) ||
    url.SchemeIs(url::kFtpScheme) ||
    url.SchemeIs(url::kGopherScheme) ||
    url.SchemeIs(url::kHttpScheme) ||
    url.SchemeIs(url::kHttpsScheme) ||
    url.SchemeIs(url::kJavaScriptScheme) ||
    url.SchemeIs(url::kMailToScheme) ||
    url.SchemeIs(url::kWsScheme) ||
    url.SchemeIs(url::kWssScheme))) {
    return true;
  }
  return false;
  //if(URLInBacklist(url))
  //  return false;
  //
  //return URLInWhitelist(url);
}

// ManagedSettingsProvider:
base::Value* YSPLoginManager::GetManagedValue(const std::string& path) {
  //if(path.compare("browser.show_home_button") == 0) {
  //  DLOG(INFO) << "YSPLoginManager::GetManagedValue!!!!";
  //  return new base::FundamentalValue(false);
  //}
  if (login_info_) {
    base::DictionaryValue* dataDict = nullptr;
    if (login_info_->GetDictionary("data", &dataDict)) {
      base::DictionaryValue* dict = nullptr;
      if (dataDict && dataDict->GetDictionary("managedSettings", &dict)) {
        base::Value* value = nullptr;
        if (dict) {
          //DLOG(INFO) << "YSPLoginManager::GetManagedValue: " << path;
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

bool YSPLoginManager::URLInPolicylist(const std::string& policy_path, const GURL& url) {
  if (login_info_) {
    base::DictionaryValue* dataDict = nullptr;
    if (login_info_->GetDictionary("data", &dataDict)) {
      base::ListValue* list = nullptr;
      if (dataDict && dataDict->GetList(policy_path, &list)) {
        if (list == nullptr || list->empty())
          return false;

        const base::Value urlValue(url.host());
        bool in = (list->Find(urlValue) != list->end());
        //DLOG(INFO) << "YSPLoginManager::URLInPolicylist host: "
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

bool YSPLoginManager::GetDownloadFileAllowed(const base::FilePath& filePath) {
  if (filePath.empty())
    return true;
  if (FileTypeInBlacklist(filePath))
    return false;

  return true;
}

bool YSPLoginManager::FileTypeInBlacklist(const base::FilePath& filePath) {
  return FileTypeInPolicylist("isFileBlackListEnabled.blacklistList", filePath);
}

bool YSPLoginManager::FileTypeInPolicylist(
  const std::string& policy_path, const base::FilePath& filePath) {
  if (strategy_info_) {
    base::DictionaryValue* dataDict = nullptr;
    if (strategy_info_->GetDictionary("data", &dataDict)) {
      base::ListValue* list = nullptr;
      if (dataDict && dataDict->GetList(policy_path, &list)) {
        if (list && !list->empty()) {
          for (size_t i = 0; i < list->GetSize(); i++) {
            base::FilePath::StringType key;
            list->GetString(i, &key);
            base::FilePath::StringType key_ext = FILE_PATH_LITERAL(".") + key;
            if (filePath.MatchesExtension(key_ext))
              return true;
          }
        }
      }
    }
  }
  return false;
}

bool YSPLoginManager::EmptyPolicyList(const std::string& policy_path) {
  if (login_info_) {
    base::DictionaryValue* dataDict = nullptr;
    if (login_info_->GetDictionary("data", &dataDict)) {
      bool status = true;
      dataDict->GetBoolean("isWebsiteListEnabled.status", &status);
      base::ListValue* list = nullptr;
      if (dataDict && dataDict->GetList(policy_path, &list)) {
        return (status || list == nullptr || list->empty());
      }
    }
  }
  return true;
}

#if defined(OS_WIN)
typedef bool(__stdcall *InternetSetOpt)(_In_ HINTERNET, _In_ DWORD, _In_ LPVOID, _In_ DWORD);
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
  MyInternetSetOpt = (InternetSetOpt)GetProcAddress(wininet_module, "InternetSetOptionW");
  if (MyInternetSetOpt != NULL) {
    proxy_tmp = MyInternetSetOpt(NULL, INTERNET_OPTION_SETTINGS_CHANGED, NULL, 0);
    if (!proxy_tmp)
      DLOG(INFO) << "UpdateProxySettings InternetSetOptionW is failure!";
  }
  //INTERNET_PER_CONN_OPTION option;
  //if (!InternetSetOption(NULL, INTERNET_OPTION_SETTINGS_CHANGED, NULL, 0)) {
  // LOG(INFO) << "InternetSetOption failure !";
  //}
}

// return download target file path
base::string16 YSPLoginManager::GetPACFileURL() {
  return GetFunctionControlString("proxyAutoConfigFile");
}

void YSPLoginManager::UpdatePACSettings(base::FilePath& pac_file) {
  DLOG(INFO) << "YSPLoginManager::UpdatePACSettings: '" << pac_file.value() << "'";
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
  MyInternetSetOpt = (InternetSetOpt)GetProcAddress(wininet_module, "InternetSetOptionW");
  if (MyInternetSetOpt != NULL) {
    pac_tmp = MyInternetSetOpt(NULL, INTERNET_OPTION_SETTINGS_CHANGED, NULL, 0);
    if (!pac_tmp)
      DLOG(INFO) << "UpdatePACSettings InternetSetOptionW is failure!";
  }
  //if (!InternetSetOption(NULL, INTERNET_OPTION_SETTINGS_CHANGED, NULL, 0)) {
  // LOG(INFO) << "InternetSetOption failure !";
  //}
}
#endif //OS_WIN

#ifdef IE_REDCORE

std::vector<ActivexDownloadInfo> YSPLoginManager::GetActivexDownloadInfo() {
  std::vector<ActivexDownloadInfo> vec;
  if (pc_info_.get() == NULL)
    return vec;
  base::DictionaryValue* dataDict = NULL;
  if (pc_info_->GetDictionary("data", &dataDict) == false)
    return vec;
  base::ListValue* downloadList = NULL;
  if (dataDict && dataDict->GetList("distributedSoftware", &downloadList)) {
    size_t i = 0;
    for (; i < downloadList->GetSize(); i++) {
      base::DictionaryValue* temp = NULL;
      if (downloadList->GetDictionary(i, &temp) == false)
        continue;
      ActivexDownloadInfo info;
      temp->GetString("fileName", &info.fileName);
      temp->GetString("displayName", &info.displayName);
      temp->GetString("downLoadUrl", &info.downloadUrl);
      temp->GetString("md5", &info.md5);
      vec.push_back(info);
    }
  }
  return vec;
}

UrlTrusted YSPLoginManager::GetUrlTrusted()
{
  UrlTrusted trust;
  if (pc_info_.get() == NULL)
    return trust;
  base::DictionaryValue* dataDict = NULL;
  if (pc_info_->GetDictionary("data", &dataDict) == false)
    return trust;
  base::DictionaryValue* urlTrustedValue = NULL;
  if (dataDict && dataDict->GetDictionary("trustedSiteLevel", &urlTrustedValue)) {
    base::ListValue* urlArry = NULL;
    if (urlTrustedValue->GetList("url", &urlArry) == false)
      return trust;
    base::string16 urlTemp = base::string16();
    size_t i = 0;
    for (; i < urlArry->GetSize(); i++) {
      urlArry->GetString(i, &urlTemp);
      trust.trustUrl.push_back(urlTemp);
    }
    base::DictionaryValue* policyValue = NULL;
    if (urlTrustedValue->GetDictionary("policy", &policyValue) == false)
      return trust;
    base::DictionaryValue::Iterator iter(*policyValue);
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

bool YSPLoginManager::GetUserAgent(std::string * uaTypes, std::string * uaString)
{
    bool ret = false;
    if (pc_info_) {
        base::DictionaryValue* dataDict = nullptr;
        if (pc_info_->GetDictionary("data", &dataDict)) {
            base::DictionaryValue* funcDict = nullptr;
            if (dataDict && dataDict->GetDictionary("httpHeadersCertify", &funcDict)) {
                std::string cryptkey;
                if (funcDict && uaTypes) {
                    ret = funcDict->GetString("setting.UAtypes", uaTypes);
                    if (!ret)
                        return ret;
                }
                if (funcDict && uaString)
                    ret = funcDict->GetString("setting.User-Agent", uaString);
            }
        }
    }

  return ret;
}
#endif /*IE_REDCORE*/

void YSPLoginManager::ReportURLLoading(const GURL& url) {
  if (!login_info_ || !GetUSReportEnabled())
    return;

  if (url.SchemeIs("redcore"))
    return;

  // init report fetcher
  if (!report_fetcher_) {
    report_fetcher_ =
        new YSPUSReportFetcher(g_browser_process->system_request_context());
  }

  if (!report_fetcher_)
    return;

  std::string strDeviceId = GetRegMachineId();
  if (strDeviceId.empty()) {
    PrefService* prefs = g_browser_process->local_state();
    strDeviceId = prefs->GetString(prefs::kYSPDeviceID);
  }
  report_fetcher_->StartReport(
      GetManageServer(),
      GetCompanyId(),
      GetString16FromDictionary(login_info_.get(), "data.company.name"),
      GetStringFromDictionary(login_info_.get(), "data.user.departmentId"),
      GetString16FromDictionary(login_info_.get(), "data.user.department"),
      GetStringFromDictionary(login_info_.get(), "data.user.id"),
      GetYSPUserName(),
      strDeviceId,
      url.spec());
}

bool YSPLoginManager::GetUSReportEnabled() {
  return GetFunctionControlBool("enableStatistic", true);
}

void YSPLoginManager::set_auto_login(bool a) {
  PrefService* prefs = g_browser_process->local_state();
  prefs->SetBoolean(prefs::kYSPAutoLogin, a);
  auto_login_ = a;
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
	double timeDiff = 0;
	PrefService* prefs = g_browser_process->local_state();
	timeDiff = prefs->GetDouble(prefs::kYSPTimeDifference);
	return timeDiff;
}

void YSPLoginManager::UpdateLoginManagerSettings()
{
	//ysp+ { crypto http header
	std::string cryptHeaderKey = GetCryptoHeaderKey();
	std::string uaTypes = GetUserAgentTypes();
	if (uaTypes == "3") {
		YSPCryptoHeader::GetInstance()->Init((uaTypes + cryptHeaderKey));
		YSPCryptoHeader::GetInstance()->SetTimeDiff(GetTimeDifference());
	}
	//ysp+ } /*crypto http header*/

	AddHeaders();
	LoadWindowFrameColors();
#ifdef IE_REDCORE
	UpdateProxySettings();
	UrlTrusted trust = YSPLoginManager::GetInstance()->GetUrlTrusted();
	SetIEUrlTrusted(trust);
#endif
}


#endif //REDCORE
