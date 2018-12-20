// Copyright 2018 The Redcore (Beijing) Technology Co.,Ltd. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "content/common/IE/web_browser_ie.h"

#include <ShlObj.h>
#include <WinUser.h>
#include <Windows.h>
#include <Wininet.h>
#include <Ws2def.h>
#include <Ws2tcpip.h>
#include <io.h>
#include <sddl.h>
#include <shellapi.h>
#include <winternl.h>
#include <algorithm>
#include <iostream>
#include <list>
#include <map>
#include <set>
#include <sstream>
#include <stack>
#include <string>
#include <vector>

#include "base/json/json_reader.h"
#include "base/json/json_writer.h"
#include "base/strings/utf_string_conversions.h"
#include "base/values.h"
#include "base/win/win_util.h"
#include "base/win/windows_version.h"
#include "content/common/IE/devtools_host_ie.h"
#include "content/common/IE/event_handler_ie.h"
#include "content/common/IE/http_monitor_ie.h"
#include "content/common/IE/xpath_parse_ie.h"
#include "net/dns/host_resolver_impl.h"
#include "third_party/blink/public/platform/web_input_event.h"
#include "third_party/minhook/include/MinHook.h"
#include "url/gurl.h"

#define WM_IE_MOUSEACTIVATE WM_USER + 5255
#define WM_COOKIEUPDATED WM_USER + 5801
#define WM_QUERYDNS WM_USER + 5802
// #define GWL_WNDPROC (-4) just for compiling
#define GWL_WNDPROC (-4)

namespace ie {

typedef struct _OBJECT_BASIC_INFORMATION {
  ULONG attributes;
  ACCESS_MASK granted_access;
  ULONG handle_count;
  ULONG pointer_count;
  ULONG paged_pool_usage;
  ULONG non_paged_pool_usage;
  ULONG reserved[3];
  ULONG name_information_length;
  ULONG type_information_length;
  ULONG security_descriptor_length;
  LARGE_INTEGER create_time;
} OBJECT_BASIC_INFORMATION, *POBJECT_BASIC_INFORMATION;

typedef struct __PUBLIC_OBJECT_TYPE_INFORMATION {
  UNICODE_STRING type_name;
  ULONG reserved[22];
} PUBLIC_OBJECT_TYPE_INFORMATION, *PPUBLIC_OBJECT_TYPE_INFORMATION;

typedef struct _OBJECT_NAME_INFORMATION {
  UNICODE_STRING object_name;
} OBJECT_NAME_INFORMATION, *POBJECT_NAME_INFORMATION;

static std::wstring ie_files_array[] = {
    L"advpack.dll",  L"ieframe.dll",  L"iepeers.dll",  L"ieproxy.dll",
    L"iertutil.dll", L"ieui.dll",     L"imgutil.dll",  L"jscript.dll",
    L"jsproxy.dll",  L"mshtml.dll",   L"mshtmled.dll", L"mshtmler.dll",
    L"msrating.dll", L"pngfilt.dll",  L"urlmon.dll",   L"wininet.dll",
    L"xmllite.dll",  L"normaliz.dll", L"dxtrans.dll",  L"dxtmsft.dll",
    L"shlwapi.dll",  L"jsproxy.dll"};
static std::wstring ie_mui_files_array[] = {
    L"8shtmled.dll.mui", L"8srating.dll.mui", L"8shtml.dll.mui",
    L"8eframe.dll.mui",  L"8ertutil.dll.mui", L"8bscript.dll.mui",
    L"8eui.dll.mui",     L"8netcpl.cpl.mui",  L"8ininet.dll.mui",
    L"8shtmler.dll.mui", L"8rlmon.dll.mui",   L"8epeers.dll.mui"};
static std::vector<std::wstring> ie_core_files =
    std::vector<std::wstring>(ie_files_array, ie_files_array + 22);
static std::vector<std::wstring> ie_mui_files =
    std::vector<std::wstring>(ie_mui_files_array, ie_mui_files_array + 12);

typedef HMODULE(WINAPI* LOADLIBRARYEXW)(LPCTSTR, HANDLE, DWORD);
static LOADLIBRARYEXW s_load_library_exw = NULL;

typedef DWORD(__stdcall* fMyLdrLoadDll)(PWCHAR,
                                        ULONG,
                                        UNICODE_STRING*,
                                        PHANDLE);
fMyLdrLoadDll fpLdrLoadDll = NULL;

typedef DWORD(__stdcall* fMyNtCreateFile)(PHANDLE,
                                          ACCESS_MASK,
                                          POBJECT_ATTRIBUTES,
                                          PIO_STATUS_BLOCK,
                                          PLARGE_INTEGER,
                                          ULONG,
                                          ULONG,
                                          ULONG,
                                          ULONG,
                                          PVOID,
                                          ULONG);
fMyNtCreateFile fpNtCreateFile = NULL;

typedef NTSTATUS(WINAPI* NTOPENFILE)(PHANDLE,
                                     ACCESS_MASK,
                                     POBJECT_ATTRIBUTES,
                                     PIO_STATUS_BLOCK,
                                     ULONG,
                                     ULONG);
NTOPENFILE fpNtOpenFile = NULL;

typedef LONG(__stdcall* REGOPENKEYEX)(HKEY, LPCTSTR, DWORD, REGSAM, PHKEY);
REGOPENKEYEX fpRegOpenKeyEX = NULL;

typedef NTSTATUS(__stdcall* NTOPENKEYEX)(IN HANDLE KeyHandle,
                                         IN ULONG desired_access,
                                         IN POBJECT_ATTRIBUTES
                                             object_attributes,
                                         _In_ ULONG OpenOptions);
NTOPENKEYEX fpNtOpenKeyEx = NULL;

static std::wstring kSidString = L"";

static std::wstring kIeFileString = L"";

typedef BOOL(
    WINAPI* HTTPQUERYINFOW)(HINTERNET, DWORD, LPVOID, LPDWORD, LPDWORD);
static HTTPQUERYINFOW fpHttpQueryInfoW = NULL;

typedef BOOL(
    WINAPI* HTTPQUERYINFOA)(HINTERNET, DWORD, LPVOID, LPDWORD, LPDWORD);
static HTTPQUERYINFOA fpHttpQueryInfoA = NULL;

typedef BOOL(WINAPI* INTERNETQUERYOPTION)(HINTERNET, DWORD, LPVOID, LPDWORD);
static INTERNETQUERYOPTION fpInternetQueryOption = NULL;

static bool set_cookie_self = false;
static CRITICAL_SECTION critical_section = CRITICAL_SECTION();

typedef BOOL(
    WINAPI* INTERNETSETCOOKIEEXW)(LPCTSTR, LPCTSTR, LPCTSTR, DWORD, DWORD_PTR);
static INTERNETSETCOOKIEEXW fpInternetSetCookie = NULL;

typedef NTSTATUS(__stdcall* QUERYOBJECT)(HANDLE,
                                         OBJECT_INFORMATION_CLASS,
                                         PVOID,
                                         ULONG,
                                         PULONG);
static QUERYOBJECT fpQueryObject = NULL;

typedef HRESULT(__stdcall* COGETCLASSOBJECT)(REFCLSID,
                                             DWORD,
                                             COSERVERINFO*,
                                             REFIID,
                                             LPVOID*);
static COGETCLASSOBJECT fpCoGetClassObject = NULL;

typedef HRESULT(
    __stdcall* LOADREGTYPELIB)(REFGUID, WORD, WORD, LCID, ITypeLib**);
static LOADREGTYPELIB fpLoadRegTypeLib = NULL;

typedef void(__stdcall* FREEADDRINFOEXW)(PADDRINFOEX);
FREEADDRINFOEXW fpFreeAddrInfoExW = NULL;
typedef int(__stdcall* GETADDRINFOEXW)(PCTSTR,
                                       PCTSTR,
                                       DWORD,
                                       LPGUID,
                                       const ADDRINFOEX*,
                                       PADDRINFOEX*,
                                       timeval*,
                                       LPOVERLAPPED,
                                       LPLOOKUPSERVICE_COMPLETION_ROUTINE,
                                       LPHANDLE);
GETADDRINFOEXW fpGetAddrInfoExW = NULL;

typedef void(__stdcall* FREEADDRINFOW)(PADDRINFOW);
FREEADDRINFOW fpFreeAddrInfoW = NULL;
typedef int(__stdcall* GETADDRINFOW)(PCWSTR,
                                     PCWSTR,
                                     const ADDRINFOW*,
                                     PADDRINFOW*);
GETADDRINFOW fpGetAddrInfoW = NULL;

static bool cookie_hook_flag = false;

WNDPROC WebBrowser::old_window_proc_ = NULL;
WNDPROC WebBrowser::old_control_proc_ = NULL;
HHOOK WebBrowser::next_hook_ = NULL;
WebBrowser* WebBrowser::self_ = NULL;
IOleInPlaceActiveObject* WebBrowser::ole_in_place_active_object_ = NULL;
static bool s_is_flash_activex_hook = false;

static std::map<PADDRINFOEXW, bool> addr_infoex_map_;
static std::map<PADDRINFOW, bool> addr_infor_map_;
CRITICAL_SECTION addr_info_section_;

// xp下设置cookie时，可能还未能够完成hook
// api，所以如果还未hook就把cookie记录在cookieBuffer中，等到hook完成时再设置
static std::vector<std::pair<std::wstring, std::wstring>> cookie_buffer =
    std::vector<std::pair<std::wstring, std::wstring>>();

std::wstring GetIECorePath() {
  wchar_t path[MAX_PATH + 1] = {0};
  GetModuleFileName(NULL, path, MAX_PATH);
  wchar_t driver[_MAX_DRIVE + 1] = {0};
  wchar_t dir[_MAX_DIR + 1] = {0};
  wchar_t fname[_MAX_FNAME + 1] = {0};
  wchar_t ext[_MAX_EXT + 1] = {0};
  ::_wsplitpath_s(path, driver, dir, fname, ext);

  return std::wstring(driver) + dir + L"ie8core";
}

std::pair<std::wstring, std::vector<std::wstring>> SpliteCookie(
    const std::wstring& host,
    const std::wstring& header) {
  std::pair<std::wstring, std::vector<std::wstring>> url_cookies;
  size_t pos_left = 0, pos_right = 0;
  std::vector<std::wstring> cookie;
  std::wstring lowercase = header;
  std::transform(lowercase.begin(), lowercase.end(), lowercase.begin(),
                 ::tolower);
  while (pos_left != std::wstring::npos) {
    pos_left = lowercase.find(L"set-cookie: ", pos_right);
    if (pos_left == std::wstring::npos)
      break;
    pos_left += wcslen(L"set-cookie: ");
    pos_right = lowercase.find(L"\r\n", pos_left);
    if (pos_right == std::wstring::npos)
      break;
    std::wstring temp = header.substr(pos_left, pos_right - pos_left);
    cookie.push_back(temp);
  }

  std::wstring url = L"";
  // TODO: is_resure should be get from host
  bool is_secure = false;
  if (is_secure)
    url = L"https://" + host + L"/";
  else
    url = L"http://" + host + L"/";

  url_cookies = std::make_pair(url, cookie);
  return url_cookies;
}

std::wstring CookieToJson(
    const std::pair<std::wstring, std::vector<std::wstring>>& cookie) {
  base::DictionaryValue root_dict;
  root_dict.SetString("url", cookie.first);
  base::ListValue* list = new base::ListValue;
  list->AppendStrings(cookie.second);
  root_dict.Set("cookies", std::move(std::unique_ptr<base::ListValue>(list)));
  std::string buffer = "";
  base::JSONWriter::Write(root_dict, &buffer);
  std::wstring json = base::UTF8ToUTF16(buffer);
  return json;
}

BOOL WINAPI DetourHttpQueryInfoW(HINTERNET request,
                                 DWORD info_level,
                                 LPVOID buffer,
                                 LPDWORD buffer_length,
                                 LPDWORD index) {
  if (fpHttpQueryInfoW == NULL)
    return FALSE;

  BOOL return_value =
      fpHttpQueryInfoW(request, info_level, buffer, buffer_length, index);
  if (return_value == FALSE)
    return return_value;

  // info_level == HTTP_QUERY_STATUS_CODE
  if (info_level != HTTP_QUERY_RAW_HEADERS_CRLF) {
    // http status返回302时或304时
    // 也有页面会设置cookie，而IE可能并不会调用HTTP_QUERY_RAW_HEADERS_CRLF，导致没能拦截cookie，所以获取所有response
    DWORD length = 0;
    LPVOID buff = NULL;
    if (fpHttpQueryInfoW(request, HTTP_QUERY_RAW_HEADERS_CRLF, (LPVOID)buff,
                         &length, NULL) == FALSE &&
        GetLastError() == ERROR_INSUFFICIENT_BUFFER) {
      buff = new byte[length + 1];
      length = sizeof(byte) * (length + 1);
      memset(buff, 0, length);
      DetourHttpQueryInfoW(request, HTTP_QUERY_RAW_HEADERS_CRLF, buff, &length,
                           NULL);
      delete[] buff;
    }
  } else if (info_level == HTTP_QUERY_RAW_HEADERS_CRLF) {
    std::wstring buff = static_cast<wchar_t*>(buffer);
    std::wstring lowercase = buff;
    std::transform(lowercase.begin(), lowercase.end(), lowercase.begin(),
                   ::tolower);
    size_t pos = lowercase.find(L"set-cookie:");
    if (pos == std::wstring::npos)
      return return_value;

    DWORD host_size = 10240;
    wchar_t* host = new wchar_t[host_size];
    memset(host, 0, sizeof(wchar_t) * host_size);
    if (fpHttpQueryInfoW(request,
                         HTTP_QUERY_FLAG_REQUEST_HEADERS | HTTP_QUERY_HOST,
                         host, &host_size, NULL)) {
      std::pair<std::wstring, std::vector<std::wstring>> temp_pair =
          SpliteCookie(host, buff);
      std::wstring json_string = CookieToJson(temp_pair);
      if (WebBrowser::GetWebBrowser()) {
        HWND hwnd = WebBrowser::GetWebBrowser()->GetControlWindow();
        LRESULT ret = 0;
        std::wstring* post_string = new std::wstring;
        *post_string = json_string;
        ret = PostMessage(hwnd, WM_COOKIEUPDATED, (WPARAM)post_string, NULL);
      }
    }
    delete[] host;
  }

  return return_value;
}

BOOL WINAPI DetourHttpQueryInfoA(HINTERNET request,
                                 DWORD info_level,
                                 LPVOID buffer,
                                 LPDWORD buffer_length,
                                 LPDWORD index) {
  if (fpHttpQueryInfoA == NULL)
    return FALSE;

  BOOL return_value =
      fpHttpQueryInfoA(request, info_level, buffer, buffer_length, index);
  if (return_value == FALSE)
    return return_value;

  // info_level == HTTP_QUERY_STATUS_CODE
  if (info_level != HTTP_QUERY_RAW_HEADERS_CRLF) {
    // http status返回302时或304时
    // 也有页面会设置cookie，而IE可能并不会调用HTTP_QUERY_RAW_HEADERS_CRLF
    // 导致没能拦截cookie，所以获取所有response
    DWORD length = 0;
    LPVOID buff = NULL;
    if (fpHttpQueryInfoA(request, HTTP_QUERY_RAW_HEADERS_CRLF, (LPVOID)buff,
                         &length, NULL) == FALSE &&
        GetLastError() == ERROR_INSUFFICIENT_BUFFER) {
      buff = new byte[length + 1];
      length = sizeof(byte) * (length + 1);
      memset(buff, 0, length);
      DetourHttpQueryInfoA(request, HTTP_QUERY_RAW_HEADERS_CRLF, buff, &length,
                           NULL);
      delete[] buff;
    }
  } else if (info_level == HTTP_QUERY_RAW_HEADERS_CRLF) {
    std::wstring buff = base::ASCIIToUTF16(static_cast<char*>(buffer));
    std::wstring lowercase = buff;
    std::transform(lowercase.begin(), lowercase.end(), lowercase.begin(),
                   ::tolower);
    size_t pos = lowercase.find(L"set-cookie:");
    if (pos == std::wstring::npos)
      return return_value;

    DWORD host_asc_size = 10240;
    char* host_asc = new char[host_asc_size];
    memset(host_asc, 0, sizeof(char) * host_asc_size);
    if (fpHttpQueryInfoA(request,
                         HTTP_QUERY_FLAG_REQUEST_HEADERS | HTTP_QUERY_HOST,
                         host_asc, &host_asc_size, NULL)) {
      std::wstring host = base::ASCIIToUTF16(host_asc);
      std::pair<std::wstring, std::vector<std::wstring>> temp_pair =
          SpliteCookie(host, buff);
      std::wstring json_string = CookieToJson(temp_pair);
      if (WebBrowser::GetWebBrowser()) {
        HWND hwnd = WebBrowser::GetWebBrowser()->GetControlWindow();
        LRESULT ret = 0;
        std::wstring* post_string = new std::wstring;
        *post_string = json_string;
        ret = PostMessage(hwnd, WM_COOKIEUPDATED, (WPARAM)post_string, NULL);
      }
    }
    delete[] host_asc;
  }
  return return_value;
}

DWORD WINAPI DetourInternetSetCookieExW(LPCTSTR url,
                                        LPCTSTR cookie_name,
                                        LPCTSTR cookie_data,
                                        DWORD flags,
                                        DWORD_PTR reserved) {
  if (fpInternetSetCookie == NULL) {
    cookie_buffer.push_back(std::make_pair(url, cookie_data));
    return FALSE;
  }
  std::wstring temp = cookie_data;
  DWORD ret =
      fpInternetSetCookie(url, cookie_name, temp.c_str(), flags, reserved);
  EnterCriticalSection(&critical_section);
  if ((ret == COOKIE_STATE_ACCEPT || ret == COOKIE_STATE_LEASH ||
       ret == COOKIE_STATE_DOWNGRADE) &&
      set_cookie_self == false) {
    std::wstring cookie_source = cookie_data;
    std::transform(temp.begin(), temp.end(), temp.begin(), ::tolower);
    if ((flags & INTERNET_COOKIE_HTTPONLY) == INTERNET_COOKIE_HTTPONLY) {
      size_t pos = temp.find(L"httponly");
      if (pos == temp.npos)
        cookie_source = cookie_source + L";  HTTPONLY";
    }
    std::vector<std::wstring> data;
    data.push_back(cookie_source);
    GURL gurl(url);
    std::pair<std::wstring, std::vector<std::wstring>> cookies =
        make_pair(base::ASCIIToUTF16(gurl.spec()), data);

    std::wstring json = CookieToJson(cookies);
    if (WebBrowser::GetWebBrowser()) {
      HWND hwnd = WebBrowser::GetWebBrowser()->GetControlWindow();
      std::wstring* post_string = new std::wstring;
      *post_string = json;
      PostMessage(hwnd, WM_COOKIEUPDATED, (WPARAM)post_string, NULL);
    }
  }

  LeaveCriticalSection(&critical_section);
  return ret;
}

bool EnableCookieHook(const LPCTSTR win_inet_path) {
  if (cookie_hook_flag == true)
    return true;

  // hook http response中设置的cookie,IE6版本要hook HttpQueryInfoA函数
  // 其他版本IE hook HttpQueryInfoW
  int system_ie_version = base::win::GetSystemIEVersion();
  if (system_ie_version == 6) {
    // hook http response中设置的cookie
    if (MH_CreateHookApi(win_inet_path, "HttpQueryInfoA", &DetourHttpQueryInfoA,
                         (LPVOID*)&fpHttpQueryInfoA) != MH_OK)
      return false;
  } else if (system_ie_version > 6) {
    // hook http response中设置的cookie
    if (MH_CreateHookApi(win_inet_path, "HttpQueryInfoW", &DetourHttpQueryInfoW,
                         (LPVOID*)&fpHttpQueryInfoW) != MH_OK)
      return false;
  }

  // hook 通过js设置的cookie
  if (MH_CreateHookApi(win_inet_path, "InternetSetCookieExW",
                       &DetourInternetSetCookieExW,
                       (LPVOID*)&fpInternetSetCookie) != MH_OK)
    return false;

  if (MH_EnableHook(MH_ALL_HOOKS) != MH_OK)
    return false;

  cookie_hook_flag = true;
  if (cookie_buffer.empty() == false) {
    std::vector<std::pair<std::wstring, std::wstring>>::iterator iter =
        cookie_buffer.begin();
    for (; iter != cookie_buffer.end(); iter++) {
      DetourInternetSetCookieExW(iter->first.c_str(), NULL,
                                 iter->second.c_str(), 0, NULL);
    }
    cookie_buffer.clear();
  }

  if (fpInternetQueryOption == NULL) {
    HMODULE hmod = GetModuleHandle(win_inet_path);
    fpInternetQueryOption =
        (INTERNETQUERYOPTION)GetProcAddress(hmod, "InternetQueryOptionW");
  }

  return true;
}

bool DisableCookieHook() {
  return MH_DisableHook(MH_ALL_HOOKS) == MH_OK;
}

HMODULE WINAPI DetourLoadLibraryExW(LPCTSTR name,
                                    HANDLE file_handle,
                                    DWORD flags) {
  if (s_load_library_exw == NULL)
    return NULL;

  std::wstring path = name;
  std::transform(path.begin(), path.end(), path.begin(), ::tolower);

  bool need_cookie_hook = false;
  if (path == L"wininet.dll")
    need_cookie_hook = true;

  std::wstring ie_path = kIeFileString;
  std::vector<std::wstring>::iterator iter = ie_core_files.begin();
  for (; iter != ie_core_files.end(); iter++) {
    int pos = path.find(*iter);
    if (pos >= 0) {
      std::wstring temp = *iter;
      if (temp != L"ieproxy.dll" && temp != L"normaliz.dll")
        temp = temp.replace(0, 1, 1, L'8');

      path = ie_path + L"\\" + temp;
      if (_waccess(path.c_str(), 0) == -1)
        path = name;

      if (flags == 0)
        flags = LOAD_WITH_ALTERED_SEARCH_PATH;
      break;
    }
  }

  HMODULE module_handle = s_load_library_exw(path.c_str(), file_handle, flags);

  if (module_handle && need_cookie_hook)
    EnableCookieHook(path.c_str());

  if (module_handle == 0)
    GetLastError();

  return module_handle;
}

std::wstring GetFlashAxPath() {
  std::wstring file_path = L"";
  wchar_t full_path[MAX_PATH] = {0};
  HMODULE child_dll_handle = LoadLibrary(L"chrome_child.dll");
  if (child_dll_handle == NULL) {
    child_dll_handle = LoadLibrary(L"chrome.dll");
    if (child_dll_handle == NULL)
      return file_path;
  }

  GetModuleFileName(child_dll_handle, full_path, MAX_PATH);
  FreeLibrary(child_dll_handle);
  wchar_t driver[_MAX_DRIVE + 1] = {0};
  wchar_t dir[_MAX_DIR + 1] = {0};
  wchar_t fname[_MAX_FNAME + 1] = {0};
  wchar_t ext[_MAX_EXT + 1] = {0};
  ::_wsplitpath_s(full_path, driver, dir, fname, ext);
  std::wstring path_temp = std::wstring(driver) + dir;

  if (base::win::GetVersion() < base::win::VERSION_WIN8)
    path_temp = path_temp + L"FlashAx\\flash_win7.ocx";
  else
    path_temp = path_temp + L"FlashAx\\flash_win8.ocx";

  if (_waccess(path_temp.c_str(), 0) == -1)
    return file_path;

  file_path = path_temp;
  return file_path;
}

DWORD WINAPI DetourLdrLoadDll(PWCHAR path_to_file,
                              ULONG flags,
                              UNICODE_STRING* module_file_name,
                              PHANDLE module_handle) {
  std::wstring path = module_file_name->Buffer;
  std::transform(path.begin(), path.end(), path.begin(), ::tolower);

  bool need_cookie_hook = false;

  if (path == L"wininet.dll")
    need_cookie_hook = true;

  std::wstring ie_path = kIeFileString;
  std::vector<std::wstring>::iterator iter = ie_core_files.begin();
  for (; iter != ie_core_files.end(); iter++) {
    int pos = path.find(*iter);
    if (pos >= 0) {
      std::wstring temp = *iter;
      if (temp != L"ieproxy.dll" &&
          temp != L"normaliz.dll" /*&& temp!=L"iepeers.dll"*/) {
        temp = temp.replace(0, 1, 1, L'8');
      }
      path = ie_path + L"\\" + temp;
      if (_waccess(path.c_str(), 0) == -1) {
        path = module_file_name->Buffer;
      }
      break;
    }
  }

  UNICODE_STRING name;
  name.Buffer = (wchar_t*)path.c_str();
  name.Length = path.size() * sizeof(wchar_t);
  name.MaximumLength = 512 * sizeof(wchar_t);
  DWORD handle = fpLdrLoadDll(path_to_file, flags, &name, module_handle);
  if (handle == 0 && need_cookie_hook)
    EnableCookieHook(path.c_str());

  if (handle != 0)
    GetLastError();

  return handle;
}

std::wstring GetSidString() {
  if (kSidString.empty()) {
    DWORD process_id = GetCurrentProcessId();
    HANDLE process_handle = OpenProcess(PROCESS_ALL_ACCESS, FALSE, process_id);
    HANDLE token = 0;
    OpenProcessToken(process_handle, TOKEN_QUERY, &token);
    DWORD length = 0;
    GetTokenInformation(token, TokenUser, NULL, length, &length);
    PTOKEN_USER token_user;
    token_user = (PTOKEN_USER)GlobalAlloc(GPTR, length);
    GetTokenInformation(token, TokenUser, token_user, length, &length);
    wchar_t* buff = NULL;
    ConvertSidToStringSid(token_user->User.Sid, &buff);
    kSidString = buff;
    LocalFree(buff);
    GlobalFree(token_user);
  }

  return kSidString;
}

std::wstring QueryHkeyString(HKEY hkey) {
  std::wstring hkey_string = L"";
  if (fpQueryObject == NULL) {
    HMODULE mod_test = GetModuleHandle(L"ntdll.dll");
    fpQueryObject = (QUERYOBJECT)GetProcAddress(mod_test, "NtQueryObject");
  }

  // comment unused code by webb
  // OBJECT_NAME_INFORMATION info = { 0 };
  unsigned long handle = 0;
  NTSTATUS stat = 0;
  stat =
      fpQueryObject(hkey, (OBJECT_INFORMATION_CLASS)1, NULL, handle, &handle);
  // STATUS_INFO_LENGTH_MISMATCH
  if (stat != 0xC0000004)
    return hkey_string;

  POBJECT_NAME_INFORMATION information =
      (POBJECT_NAME_INFORMATION)GlobalAlloc(GPTR, handle);
  if (information == NULL)
    return hkey_string;

  fpQueryObject(hkey, (OBJECT_INFORMATION_CLASS)1, information, handle,
                &handle);
  hkey_string = information->object_name.Buffer;
  GlobalFree(information);
  return hkey_string;
}

LONG WINAPI DetourRegOpenKeyEx(HKEY hkey,
                               LPCTSTR sub_key,
                               DWORD options,
                               REGSAM sam_desired,
                               PHKEY result) {
  if (fpRegOpenKeyEX == NULL)
    return 1;

  long ret = 0;

  static int log = 0;  // 1;
  bool read_sys = false;
  bool read_hook = false;

  ret = fpRegOpenKeyEX(hkey, sub_key, options, sam_desired, result);

  std::wstring register_string = L"";
  if (*result && ret == ERROR_SUCCESS) {
    read_sys = true;
    register_string = QueryHkeyString(*result);
  } else {
    read_sys = false;
    if (hkey == HKEY_CLASSES_ROOT) {
      if (base::win::GetVersion() == base::win::VERSION_XP)
        register_string =
            std::wstring(L"\\REGISTRY\\MACHINE\\SOFTWARE\\Classes\\") + sub_key;
      else
        register_string =
            std::wstring(
                L"\\REGISTRY\\MACHINE\\SOFTWARE\\Classes\\Wow6432Node\\") +
            sub_key;
    } else if (hkey == HKEY_LOCAL_MACHINE) {
      register_string = std::wstring(L"\\REGISTRY\\MACHINE\\") + sub_key;
    } else {
      register_string = QueryHkeyString(hkey);
      register_string = register_string + L"\\" + sub_key;
    }
  }

  std::wstring interface_key =
      L"\\REGISTRY\\MACHINE\\SOFTWARE\\Classes\\Wow6432Node\\Interface\\";
  size_t interface_position = register_string.find(interface_key);
  if (interface_position != register_string.npos &&
      register_string.size() > interface_key.size()) {
    int length = interface_position + interface_key.size();
    std::wstring sub_key_tmp = register_string.substr(length);
    sub_key_tmp =
        L"Software\\AllMobilize\\Redcore\\IE8\\Interface\\" + sub_key_tmp;
    HKEY result_key = 0;
    if (fpRegOpenKeyEX(HKEY_CURRENT_USER, sub_key_tmp.c_str(), 0, KEY_READ,
                       &result_key) == ERROR_SUCCESS) {
      read_hook = true;
      if (*result)
        RegCloseKey(*result);

      *result = result_key;
      ret = ERROR_SUCCESS;
    }
  }

  std::wstring interface_key1 =
      L"\\REGISTRY\\MACHINE\\SOFTWARE\\Classes\\Interface\\";
  interface_position = register_string.find(interface_key1);
  if (interface_position != register_string.npos &&
      register_string.size() > interface_key1.size()) {
    int length = interface_position + interface_key1.size();
    std::wstring sub_key_tmp = register_string.substr(length);
    sub_key_tmp =
        L"Software\\AllMobilize\\Redcore\\IE8\\Interface\\" + sub_key_tmp;
    HKEY result_key = 0;
    if (fpRegOpenKeyEX(HKEY_CURRENT_USER, sub_key_tmp.c_str(), 0, KEY_READ,
                       &result_key) == ERROR_SUCCESS) {
      read_hook = true;
      if (*result)
        RegCloseKey(*result);
      *result = result_key;
      ret = ERROR_SUCCESS;
    }
  }

  std::wstring key =
      L"\\REGISTRY\\MACHINE\\SOFTWARE\\Classes\\Wow6432Node\\CLSID\\";
  size_t pos = register_string.find(key);
  if (pos != register_string.npos && register_string.size() > key.size()) {
    int length = pos + key.size();
    std::wstring sub_key_tmp = register_string.substr(length);
    sub_key_tmp = L"Software\\AllMobilize\\Redcore\\IE8\\CLSID\\" + sub_key_tmp;
    HKEY result_key = 0;
    if (fpRegOpenKeyEX(HKEY_CURRENT_USER, sub_key_tmp.c_str(), 0, KEY_READ,
                       &result_key) == ERROR_SUCCESS) {
      read_hook = true;
      if (*result)
        RegCloseKey(*result);
      *result = result_key;
      ret = ERROR_SUCCESS;
      // log = 0;
    }
  }

  std::wstring key2 = L"\\REGISTRY\\MACHINE\\SOFTWARE\\Classes\\CLSID\\";
  pos = register_string.find(key2);
  if (pos != register_string.npos && register_string.size() > key2.size()) {
    int length = pos + key2.size();
    std::wstring sub_key_tmp = register_string.substr(length);
    sub_key_tmp = L"Software\\AllMobilize\\Redcore\\IE8\\CLSID\\" + sub_key_tmp;
    HKEY result_key = 0;
    if (fpRegOpenKeyEX(HKEY_CURRENT_USER, sub_key_tmp.c_str(), 0, KEY_READ,
                       &result_key) == ERROR_SUCCESS) {
      read_hook = true;
      if (*result)
        RegCloseKey(*result);
      *result = result_key;
      ret = ERROR_SUCCESS;
    }
  }

  if (log == 1) {
    // FILE* file = _wfopen(L"C:\\reg.txt", L"ab+");
    FILE* file = _wfopen(L"e:\\reg.txt", L"ab+");
    if (file) {
      fwrite(register_string.c_str(), 1,
             register_string.size() * sizeof(wchar_t), file);

      std::wstring s1 = L"  read_sys : ";
      s1 = s1 + (read_sys ? L"true" : L"false");
      fwrite(s1.c_str(), 1, s1.size() * sizeof(wchar_t), file);

      std::wstring s2 = L"  read_hook : ";
      s2 = s2 + (read_hook ? L"true" : L"false");
      fwrite(s2.c_str(), 1, s2.size() * sizeof(wchar_t), file);

      fwrite(L"\r\n", 1, 4, file);
      fclose(file);
    }
  }

  return ret;
}

DWORD WINAPI DetourNtCreateFile(PHANDLE file_handle,
                                ACCESS_MASK desired_access,
                                POBJECT_ATTRIBUTES object_attributes,
                                _Out_ PIO_STATUS_BLOCK io_status_block,
                                _In_opt_ PLARGE_INTEGER allocation_size,
                                _In_ ULONG file_attributes,
                                _In_ ULONG share_access,
                                _In_ ULONG create_disposition,
                                _In_ ULONG create_options,
                                _In_ PVOID ea_buffer,
                                _In_ ULONG ea_length) {
  OBJECT_ATTRIBUTES obj = *object_attributes;
  UNICODE_STRING uni_string = {0};
  std::wstring path = obj.ObjectName->Buffer;
  std::transform(path.begin(), path.end(), path.begin(), ::tolower);
  OBJECT_ATTRIBUTES object = *object_attributes;
  std::wstring temp = kIeFileString;
  wchar_t* buff = NULL;

  std::vector<std::wstring>::iterator ie_core_iter = ie_core_files.begin();
  for (; ie_core_iter != ie_core_files.end(); ie_core_iter++) {
    size_t pos = path.find(*ie_core_iter);
    if (pos != path.npos) {
      std::wstring name = *ie_core_iter;
      if (name != L"ieproxy.dll" && name != L"normaliz.dll") {
        name = name.replace(0, 1, 1, L'8');
      }
      path = kIeFileString + L"\\" + name;
      if (_waccess(path.c_str(), 0) == -1) {
        object = *object_attributes;
      } else {
        path = L"\\??\\" + path;
        buff = new wchar_t[path.size() + 1];
        memset(buff, 0, (path.size() + 1) * sizeof(wchar_t));
        memcpy(buff, path.c_str(), path.size() * sizeof(wchar_t));
        uni_string.Buffer = buff;
        uni_string.Length = path.size() * sizeof(wchar_t);
        uni_string.MaximumLength = uni_string.Length + sizeof(wchar_t);
        object.ObjectName = &uni_string;
      }
      break;
    }
  }

  if (path.find(L"mshtml.tlb") != path.npos) {
    temp = temp + L"\\8shtml.tlb";
    temp = L"\\??\\" + temp;
    buff = new wchar_t[temp.size() + 1];
    memset(buff, 0, (temp.size() + 1) * sizeof(wchar_t));
    memcpy(buff, temp.c_str(), temp.size() * sizeof(wchar_t));
    uni_string.Buffer = buff;
    uni_string.Length = temp.size() * sizeof(wchar_t);
    uni_string.MaximumLength = uni_string.Length + sizeof(wchar_t);
    object.ObjectName = &uni_string;
  }

  if (path.find(L"iepeers.dll") != path.npos) {
    temp = temp + L"\\8epeers.dll";
    temp = L"\\??\\" + temp;
    buff = new wchar_t[temp.size() + 1];
    memset(buff, 0, (temp.size() + 1) * sizeof(wchar_t));
    memcpy(buff, temp.c_str(), temp.size() * sizeof(wchar_t));
    uni_string.Buffer = buff;
    uni_string.Length = temp.size() * sizeof(wchar_t);
    uni_string.MaximumLength = uni_string.Length + sizeof(wchar_t);
    object.ObjectName = &uni_string;
  }

  std::vector<std::wstring>::iterator iter = ie_mui_files.begin();
  for (; iter != ie_mui_files.end(); iter++) {
    int pos = path.find(*iter);
    if (pos >= 0) {
      temp = temp + L"\\" + *iter;
      if (_waccess(temp.c_str(), 0) == -1) {
        object = *object_attributes;
      } else {
        temp = L"\\??\\" + temp;
        buff = new wchar_t[temp.size() + 1];
        memset(buff, 0, (temp.size() + 1) * sizeof(wchar_t));
        memcpy(buff, temp.c_str(), temp.size() * sizeof(wchar_t));
        uni_string.Buffer = buff;
        uni_string.Length = temp.size() * sizeof(wchar_t);
        uni_string.MaximumLength = uni_string.Length + sizeof(wchar_t);
        object.ObjectName = &uni_string;
      }
      break;
    }
  }
  DWORD handle =
      fpNtCreateFile(file_handle, desired_access, &object, io_status_block,
                     allocation_size, file_attributes, share_access,
                     create_disposition, create_options, ea_buffer, ea_length);
  if (buff) {
    delete[] buff;
    buff = NULL;
  }

  if (handle != 0)
    GetLastError();

  return handle;
}

HRESULT WINAPI DetourLoadRegTypeLib(REFGUID guid,
                                    WORD version_major,
                                    WORD version_minor,
                                    LCID lcid,
                                    ITypeLib** type_lib) {
  HRESULT result;

  std::wstring flash_class_id = L"{D27CDB6B-AE6D-11CF-96B8-444553540000}";
  LPOLESTR buff = NULL;
  std::wstring class_id_string = L"";
  if (SUCCEEDED(StringFromCLSID(guid, &buff)) && buff) {
    class_id_string = buff;
    CoTaskMemFree(buff);
  }

  if (flash_class_id == class_id_string) {
    std::wstring path_string = GetFlashAxPath();
    if (path_string.empty() == false) {
      LPCOLESTR path = path_string.c_str();
      result = LoadTypeLib(path, type_lib);
      return result;
    }
  }

  result = fpLoadRegTypeLib(guid, version_major, version_minor, lcid, type_lib);
  return result;
}

HRESULT WINAPI DetourCoGetClassObject(_In_ REFCLSID class_id,
                                      _In_ DWORD class_context,
                                      _In_opt_ COSERVERINFO* server_info,
                                      _In_ REFIID riid,
                                      _Out_ LPVOID* ppv) {
  HRESULT result;

  std::wstring flash_class_id = L"{D27CDB6E-AE6D-11CF-96B8-444553540000}";
  LPOLESTR buff = NULL;
  std::wstring class_id_string = L"";
  if (SUCCEEDED(StringFromCLSID(class_id, &buff)) && buff) {
    class_id_string = buff;
    CoTaskMemFree(buff);
  }

  if (flash_class_id == class_id_string) {
    std::wstring path = GetFlashAxPath();
    HMODULE hmod = LoadLibrary(path.c_str());
    if (hmod) {
      typedef HRESULT(__stdcall * DLLGETCLASSOBJECT)(REFCLSID, REFIID,
                                                     LPVOID * ppv);
      DLLGETCLASSOBJECT fpDllGetClassObj = NULL;
      fpDllGetClassObj =
          (DLLGETCLASSOBJECT)GetProcAddress(hmod, "DllGetClassObject");
      if (fpDllGetClassObj) {
        result = fpDllGetClassObj(class_id, riid, ppv);
        return result;
      }
    }
  }

  result = fpCoGetClassObject(class_id, class_context, server_info, riid, ppv);
  return result;
}

bool EnableLoadLibraryHook() {
  kIeFileString = GetIECorePath();
  ::SetDllDirectory(kIeFileString.c_str());

  if (MH_CreateHookApi(L"Kernel32", "LoadLibraryExW", &DetourLoadLibraryExW,
                       (LPVOID*)&s_load_library_exw) != MH_OK)
    return false;

  if (MH_CreateHookApi(L"ntdll.dll", "LdrLoadDll", &DetourLdrLoadDll,
                       (LPVOID*)&fpLdrLoadDll) != MH_OK)
    return false;

  if (MH_CreateHookApi(L"ntdll.dll", "NtCreateFile", &DetourNtCreateFile,
                       (LPVOID*)&fpNtCreateFile) != MH_OK)
    return false;

  base::win::Version windows_version = base::win::GetVersion();
  if (windows_version == base::win::VERSION_WIN8 ||
      windows_version == base::win::VERSION_WIN8_1) {
    if (MH_CreateHookApi(L"kernelbase.dll", "RegOpenKeyExW",
                         &DetourRegOpenKeyEx,
                         (LPVOID*)&fpRegOpenKeyEX) != MH_OK)
      return false;
  } else if (windows_version == base::win::VERSION_XP) {
    if (MH_CreateHookApi(L"advapi32.dll", "RegOpenKeyExW", &DetourRegOpenKeyEx,
                         (LPVOID*)&fpRegOpenKeyEX) != MH_OK)
      return false;
  } else {
    if (MH_CreateHookApi(L"kernel32.dll", "RegOpenKeyExW", &DetourRegOpenKeyEx,
                         (LPVOID*)&fpRegOpenKeyEX) != MH_OK)
      return false;
  }

  if (MH_EnableHook(MH_ALL_HOOKS) != MH_OK)
    return false;

  return true;
}

bool EnableFlashHook() {
  if (MH_CreateHookApi(L"Ole32.dll", "CoGetClassObject",
                       &DetourCoGetClassObject,
                       (LPVOID*)&fpCoGetClassObject) != MH_OK)
    return false;

  if (MH_CreateHookApi(L"OleAut32.dll", "LoadRegTypeLib", &DetourLoadRegTypeLib,
                       (LPVOID*)&fpLoadRegTypeLib) != MH_OK)
    return false;

  if (MH_EnableHook(MH_ALL_HOOKS) != MH_OK)
    return false;

  return true;
}

bool DisableLoadLibraryHook() {
  if (MH_DisableHook(&LoadLibraryExW) != MH_OK)
    return false;

  if (MH_RemoveHook(&LoadLibraryExW) != MH_OK)
    return false;

  cookie_hook_flag = false;
  return true;
}

void WINAPI DetourFreeAddrInfoExW(PADDRINFOEX info) {
  EnterCriticalSection(&addr_info_section_);
  auto mapIterator = addr_infoex_map_.find(info);
  if (fpFreeAddrInfoExW == NULL || mapIterator == addr_infoex_map_.end()) {
    LeaveCriticalSection(&addr_info_section_);
    return;
  }
  bool isCreateByHook = mapIterator->second;
  addr_infoex_map_.erase(mapIterator);
  LeaveCriticalSection(&addr_info_section_);

  int verNum = base::win::OSInfo::GetInstance()->version();
  if (!isCreateByHook) {
    fpFreeAddrInfoExW(info);
    return;
  }

  if (verNum == base::win::VERSION_XP || verNum == base::win::VERSION_WIN7 ||
      verNum == base::win::VERSION_WIN10 ||
      verNum == base::win::VERSION_WIN10_TH2) {
    PADDRINFOEXW pcur = info;

    while (pcur != NULL) {
      PADDRINFOEXW pnext = pcur->ai_next;
      if (pcur->ai_canonname) {
        delete[] pcur->ai_canonname;
      }
      if (pcur->ai_addr)
        delete pcur->ai_addr;
      delete pcur;
      pcur = pnext;
    }
  }
}

int WINAPI
DetourGetAddrInfoExW(PCTSTR pName,
                     PCTSTR pServiceName,
                     DWORD dwNameSpace,
                     LPGUID lpNspId,
                     const ADDRINFOEX* pHints,
                     PADDRINFOEX* ppResult,
                     timeval* timeout,
                     LPOVERLAPPED lpOverlapped,
                     LPLOOKUPSERVICE_COMPLETION_ROUTINE lpCompletionRoutine,
                     LPHANDLE lpNameHandle) {
  int ret = fpGetAddrInfoExW(pName, pServiceName, dwNameSpace, lpNspId, pHints,
                             ppResult, timeout, lpOverlapped,
                             lpCompletionRoutine, lpNameHandle);

  if (!pName)
    return ret;

  int verNum = base::win::OSInfo::GetInstance()->version();
  if (verNum == base::win::VERSION_XP || verNum == base::win::VERSION_WIN7 ||
      verNum == base::win::VERSION_WIN10 ||
      verNum == base::win::VERSION_WIN10_TH2) {
    HWND hwnd = WebBrowser::GetWebBrowser()->GetControlWindow();
    PrivateDnsIp dnsIp;
    dnsIp.host = pName;
    dnsIp.ip_list.clear();
    HANDLE QueryDnsEvent = CreateEvent(NULL, FALSE, FALSE, NULL);
    ResetEvent(QueryDnsEvent);
    PostMessage(hwnd, WM_QUERYDNS, (WPARAM)QueryDnsEvent, (LPARAM)&dnsIp);
    WaitForSingleObject(QueryDnsEvent, INFINITE);
    CloseHandle(QueryDnsEvent);
    std::list<std::wstring> ip_list = dnsIp.ip_list;
    if (ip_list.empty() == false) {
      std::wstring canonname = pName;
      if (ret == 0 && (*ppResult)->ai_canonname)
        canonname = (*ppResult)->ai_canonname;
      ADDRINFOEX* lastAddr = NULL;

      for (auto wsIp : ip_list) {
        ADDRINFOEX* addr = new ADDRINFOEX;
        wchar_t* canonname1 = new wchar_t[wcslen(wsIp.c_str()) + 1];
        memset(canonname1, 0, sizeof(wchar_t) * (wcslen(wsIp.c_str()) + 1));
        wcscpy(canonname1, wsIp.c_str());
        addr->ai_canonname = canonname1;
        addr->ai_family = AF_INET;
        addr->ai_protocol = IPPROTO_TCP;
        addr->ai_flags = 0;
        addr->ai_socktype = SOCK_STREAM;
        sockaddr_in* temp = new sockaddr_in;
        memset(temp, 0, sizeof(sockaddr_in));
        temp->sin_family = AF_INET;
        temp->sin_addr.s_addr = inet_addr(base::UTF16ToASCII(wsIp).c_str());
        temp->sin_port = htons(0);
        addr->ai_addr = (sockaddr*)temp;
        addr->ai_next = lastAddr;
        addr->ai_blob = 0;
        addr->ai_provider = 0;
        addr->ai_bloblen = 0;
        addr->ai_addrlen = sizeof(sockaddr);

        lastAddr = addr;
      }
      if (fpFreeAddrInfoExW && ret == 0)
        fpFreeAddrInfoExW(*ppResult);
      *ppResult = lastAddr;

      EnterCriticalSection(&addr_info_section_);
      addr_infoex_map_[*ppResult] = true;
      LeaveCriticalSection(&addr_info_section_);
      return 0;
    }
  }

  if (ret == 0 && *ppResult) {
    EnterCriticalSection(&addr_info_section_);
    addr_infoex_map_[*ppResult] = false;
    LeaveCriticalSection(&addr_info_section_);
  }

  return ret;
}

void WINAPI DetourFreeAddrInfoW(ADDRINFOW* info) {
  EnterCriticalSection(&addr_info_section_);
  auto mapIterator = addr_infor_map_.find(info);
  if (fpFreeAddrInfoW == NULL || mapIterator == addr_infor_map_.end()) {
    LeaveCriticalSection(&addr_info_section_);
    return;
  }

  bool isCreateByHook = mapIterator->second;
  addr_infor_map_.erase(mapIterator);
  LeaveCriticalSection(&addr_info_section_);

  if (!isCreateByHook) {
    fpFreeAddrInfoW(info);
    return;
  }

  int verNum = base::win::OSInfo::GetInstance()->version();
  if (verNum == base::win::VERSION_XP || verNum == base::win::VERSION_WIN7 ||
      verNum == base::win::VERSION_WIN10 ||
      verNum == base::win::VERSION_WIN10_TH2) {
    PADDRINFOW pcur = info;
    while (pcur != NULL) {
      PADDRINFOW pnext = pcur->ai_next;
      if (pcur->ai_canonname)
        delete[] pcur->ai_canonname;
      if (pcur->ai_addr)
        delete pcur->ai_addr;
      delete pcur;
      pcur = pnext;
    }
  }
}

int WINAPI DetourGetAddrInfoW(PCWSTR pNodeName,
                              PCWSTR pServiceName,
                              const ADDRINFOW* pHints,
                              PADDRINFOW* ppResult) {
  int ret = fpGetAddrInfoW(pNodeName, pServiceName, pHints, ppResult);

  if (!pNodeName)
    return ret;

  int verNum = base::win::OSInfo::GetInstance()->version();
  if (verNum == base::win::VERSION_XP || verNum == base::win::VERSION_WIN7 ||
      verNum == base::win::VERSION_WIN10 ||
      verNum == base::win::VERSION_WIN10_TH2) {
    PrivateDnsIp dnsIp;
    dnsIp.host = pNodeName;
    dnsIp.ip_list.clear();

    HWND hwnd = WebBrowser::GetWebBrowser()->GetControlWindow();
    HANDLE QueryDnsEvent = CreateEvent(NULL, FALSE, FALSE, NULL);
    ResetEvent(QueryDnsEvent);
    PostMessage(hwnd, WM_QUERYDNS, (WPARAM)QueryDnsEvent, (LPARAM)&dnsIp);
    WaitForSingleObject(QueryDnsEvent, INFINITE);
    CloseHandle(QueryDnsEvent);

    std::list<std::wstring> ip_list = dnsIp.ip_list;

    if (ip_list.empty() == false) {
      std::wstring canonname = pNodeName;
      if (ret == 0)
        canonname = (*ppResult)->ai_canonname;
      ADDRINFOW* lastAddr = NULL;
      std::list<std::wstring>::iterator iter = ip_list.begin();
      for (; iter != ip_list.end(); iter++) {
        ADDRINFOW* addr = new ADDRINFOW;
        int wlen = wcslen(canonname.c_str()) + 1;
        wchar_t* canonname1 = new wchar_t[wlen];
        memset(canonname1, 0, sizeof(wchar_t) * wlen);
        wcscpy(canonname1, canonname.c_str());
        addr->ai_canonname = canonname1;  // canonname1;
        addr->ai_family = AF_INET;
        addr->ai_protocol = IPPROTO_TCP;
        addr->ai_flags = 0;
        addr->ai_socktype = SOCK_STREAM;
        sockaddr_in* temp = new sockaddr_in;
        memset(temp, 0, sizeof(sockaddr_in));
        temp->sin_family = AF_INET;
        temp->sin_addr.s_addr = inet_addr(base::UTF16ToASCII(*iter).c_str());
        temp->sin_port = htons(0);
        addr->ai_addr = (sockaddr*)temp;
        addr->ai_next = lastAddr;
        addr->ai_addrlen = sizeof(sockaddr);

        lastAddr = addr;
      }
      if (fpFreeAddrInfoW && ret == 0)
        fpFreeAddrInfoW(*ppResult);
      *ppResult = lastAddr;

      EnterCriticalSection(&addr_info_section_);
      addr_infor_map_[*ppResult] = true;
      LeaveCriticalSection(&addr_info_section_);
      return 0;
    }
  }
  if (ret == 0 && *ppResult) {
    EnterCriticalSection(&addr_info_section_);
    addr_infor_map_[*ppResult] = false;
    LeaveCriticalSection(&addr_info_section_);
  }
  return ret;
}

WebBrowser::WebBrowser(HWND parent_handle,
                       EventHandler* delegate,
                       int browser_emu,
                       bool is_new)
    : com_ref_count_(0),
      web_browser2_(NULL),
      event_handler_(NULL),
      doc_host_hander_(NULL),
      server_provider_(NULL),
      dev_tools_host_(NULL),
      connection_point_(NULL),
      cookie_(0),
      parent_handle_(0),
      control_handle_(0),
      ole_in_place_object_(NULL),
      ole_object_(NULL),
      delegate_(NULL),
      ie_browser_handle_(NULL),
      module_handle_(NULL),
      win_inet_handle_(NULL),
      url_mon_handle_(NULL),
      fpUrlSetSessionOption(NULL),
      fpUrlGetSessionOption(NULL),
      is_downloading_(false),
      browser_emulation_((ie::Emulation)browser_emu),
      old_ua_(L""),
      print_enabled_(true),
      mouse_right_button_enabled_(true),
      cut_copy_enabled_(true),
      save_file_enabled_(true),
      login_info_frame_xpath_(L""),
      login_info_name_xpath_(L""),
      login_info_password_xpath_(L""),
      login_info_button_xpath_(L""),
      login_info_name_(L""),
      login_info_password_(L""),
      login_info_url_(L"") {
  InitializeCriticalSectionAndSpinCount(&critical_section, 0x00004000);
  MH_Initialize();

  ::SetRect(&rect_object_, -1, -1, 0, 0);
  // parent_handle_ = _hWndParent;
  // 为了解决Chrome49中无法创建IE窗口的问题，首先让IE以桌面为父窗口建立IE窗口，
  // 然后在DISPID_TITLECHANGE事件中，再将父窗口设成chrome中的窗口
  parent_handle_ = NULL;

  delegate_ = delegate;

  self_ = this;

  if (CreateBrowser(is_new) == FALSE)
    return;

  event_handler_->SetChildHWND(GetControlWindow());
  event_handler_->SetParentHwnd(parent_handle);
  parent_handle_ = parent_handle;
  // ShowWindow(GetControlWindow(), SW_SHOW);
  ShowWindow(GetControlWindow(), SW_HIDE);

  old_control_proc_ = (WNDPROC)SetWindowLong(GetControlWindow(), GWL_WNDPROC,
                                             (long)SubClassControlWndProc);
  dev_tools_host_ = new DevToolsHost(web_browser2_);
}

WebBrowser::~WebBrowser() {
  if (dev_tools_host_)
    delete dev_tools_host_;

  if (old_control_proc_)
    SetWindowLong(GetControlWindow(), GWL_WNDPROC, (long)old_control_proc_);

  if (ole_in_place_object_) {
    ole_in_place_object_->InPlaceDeactivate();
    ole_in_place_object_->Release();
  }

  if (connection_point_ && event_handler_ && cookie_) {
    connection_point_->Unadvise(cookie_);
    connection_point_->Release();
    delete event_handler_;
  }

  if (web_browser2_) {
    web_browser2_->Stop();
    web_browser2_->ExecWB(OLECMDID_CLOSE, OLECMDEXECOPT_DONTPROMPTUSER, 0, 0);
    web_browser2_->Release();
    if (ole_object_) {
      ole_object_->DoVerb(OLEIVERB_HIDE, NULL, this, 0, parent_handle_, NULL);
      ole_object_->Close(OLECLOSE_NOSAVE);
      OleSetContainedObject(ole_object_, FALSE);
      ole_object_->SetClientSite(NULL);
      CoDisconnectObject(ole_object_, 0);
      ole_object_->Release();
    }
  }

  if (doc_host_hander_)
    delete doc_host_hander_;

  if (server_provider_)
    delete server_provider_;

  if (module_handle_)
    ::FreeLibrary(module_handle_);

  DisableLoadLibraryHook();
  DisableCookieHook();
  MH_Uninitialize();

  if (win_inet_handle_) {
    FreeLibrary(win_inet_handle_);
    win_inet_handle_ = NULL;
  }

  if (url_mon_handle_) {
    FreeLibrary(url_mon_handle_);
    url_mon_handle_ = NULL;
    fpUrlSetSessionOption = NULL;
  }

  DeleteCriticalSection(&critical_section);
}

WebBrowser* WebBrowser::GetWebBrowser() {
  return self_;
}

bool WebBrowser::CreateBrowser(bool is_new) {
  if (server_provider_ == NULL)
    server_provider_ = new ServiceProvider(delegate_);

  s_is_flash_activex_hook = true;
  base::win::Version version = base::win::VERSION_WIN8;
  //不使用自带IE8内核
  if (version >= base::win::VERSION_XP && version < base::win::VERSION_WIN8 &&
      (int)browser_emulation_ <= (int)ie::EMULATION8) {
    EnableLoadLibraryHook();
    module_handle_ = ::LoadLibraryExW(L"ieframe.dll", NULL, NULL);
  }

  if (s_is_flash_activex_hook)
    EnableFlashHook();

  int sysIEVer = base::win::GetSystemIEVersion();
  if (sysIEVer >= 10 && sysIEVer <= 11) {
    // TODO(qidi.ma): disable private dns functionality, should be opened before product release
    // if (MH_CreateHookApi(L"ws2_32.dll", "GetAddrInfoExW", &DetourGetAddrInfoExW,
    //                      (LPVOID*)&fpGetAddrInfoExW) != MH_OK)
    //   return false;
    // if (MH_CreateHookApi(L"ws2_32.dll", "FreeAddrInfoExW",
    //                      &DetourFreeAddrInfoExW,
    //                      (LPVOID*)&fpFreeAddrInfoExW) != MH_OK)
    //   return false;
    // if (MH_EnableHook(MH_ALL_HOOKS) != MH_OK)
    //   return false;
    // } else if (sysIEVer >= 6 && sysIEVer <= 9) {
    //   if (MH_CreateHookApi(L"ws2_32.dll", "GetAddrInfoW", &DetourGetAddrInfoW,
    //                        (LPVOID*)&fpGetAddrInfoW) != MH_OK)
    //     return false;
    //   if (MH_CreateHookApi(L"ws2_32.dll", "FreeAddrInfoW", &DetourFreeAddrInfoW,
    //                        (LPVOID*)&fpFreeAddrInfoW) != MH_OK)
    //     return false;
    if (MH_EnableHook(MH_ALL_HOOKS) != MH_OK)
      return false;
  }

  // XP下直接加载Wininet.dll会失败，这里也无法hook，所以XP下是在
  // LoadLibraryExW的hook函数中进行的Wininet.dll hook
  win_inet_handle_ = ::LoadLibraryExW(L"wininet.dll", NULL, NULL);
  if (win_inet_handle_) {
    EnableCookieHook(L"wininet.dll");
  }

  HRESULT handle_result = E_FAIL;
  // module_handle_ = ::LoadLibraryExW(L"ieframe.dll", NULL, NULL);
  if (module_handle_) {
    typedef HRESULT(__stdcall * HtmlDllGetClassObj)(REFCLSID, REFIID, LPVOID);
    HtmlDllGetClassObj class_object = (HtmlDllGetClassObj)::GetProcAddress(
        module_handle_, "DllGetClassObject");
    if (class_object) {
      IClassFactory* class_factory = NULL;
      handle_result =
          class_object(CLSID_WebBrowser, IID_IClassFactory, &class_factory);

      if (class_factory)
        handle_result = class_factory->CreateInstance(NULL, IID_IOleObject,
                                                      (void**)&ole_object_);
    }
  } else {
    handle_result =
        ::OleCreate(CLSID_WebBrowser, IID_IOleObject, OLERENDER_DRAW, 0, this,
                    this, (void**)&ole_object_);
  }

  if (FAILED(handle_result))
    return FALSE;

  handle_result = ole_object_->SetClientSite(this);
  handle_result = OleSetContainedObject(ole_object_, TRUE);

  RECT pos_rect;
  ::SetRect(&pos_rect, 0, 0, 0, 0);
  handle_result = ole_object_->DoVerb(OLEIVERB_INPLACEACTIVATE, NULL, this, -1,
                                      parent_handle_, &pos_rect);
  if (FAILED(handle_result))
    return FALSE;

  handle_result = ole_object_->QueryInterface(&web_browser2_);
  if (FAILED(handle_result))
    return FALSE;

  server_provider_->SetWebBrowser(web_browser2_);

  if (doc_host_hander_ == NULL)
    doc_host_hander_ = new DocHostUIHandler(delegate_, this);

  web_browser2_->put_RegisterAsBrowser(VARIANT_TRUE);
  web_browser2_->put_RegisterAsDropTarget(VARIANT_TRUE);
  web_browser2_->put_Visible(VARIANT_FALSE);
  IConnectionPointContainer* container = NULL;
  handle_result = web_browser2_->QueryInterface(IID_IConnectionPointContainer,
                                                (void**)&container);
  if (FAILED(handle_result))
    return FALSE;

  // 事件监听
  handle_result = container->FindConnectionPoint(DIID_DWebBrowserEvents2,
                                                 &connection_point_);
  event_handler_ = new Event(delegate_, doc_host_hander_);
  event_handler_->SetWebBrowser(web_browser2_);
  handle_result = connection_point_->Advise(event_handler_, &cookie_);
  container->Release();

  HttpMonitorIe::Init();
  // //IE8下会造成访问有些页面IE内核死锁，取消http请求过程拦截。

  // DomainAuthenticate接口在不是通过DISPID_NEWWINDOW3建立的ie时，
  // 必须要先加载一个页面后才能够初始化，应该是IE的bug
  if (is_new == false)
    web_browser2_->Navigate(L"https://www.redcore.cn", NULL, NULL, NULL, NULL);

  return TRUE;
}

RECT WebBrowser::PixelToHiMetric(const RECT& rect) {
  static bool s_initialized = false;
  static int s_pixels_per_inch_x, s_pixels_per_inch_y;
  if (!s_initialized) {
    HDC hdc = ::GetDC(0);
    s_pixels_per_inch_x = ::GetDeviceCaps(hdc, LOGPIXELSX);
    s_pixels_per_inch_y = ::GetDeviceCaps(hdc, LOGPIXELSY);
    ::ReleaseDC(0, hdc);
    s_initialized = true;
  }

  RECT rc;
  rc.left = MulDiv(2540, rect.left, s_pixels_per_inch_x);
  rc.top = MulDiv(2540, rect.top, s_pixels_per_inch_y);
  rc.right = MulDiv(2540, rect.right, s_pixels_per_inch_x);
  rc.bottom = MulDiv(2540, rect.bottom, s_pixels_per_inch_y);
  return rc;
}

bool WebBrowser::DownLoadFile(const std::wstring& url,
                              const int status,
                              const VARIANT& var) {
  bool ret = false;
  if (delegate_)
    ret = delegate_->OnDownLoadFile(url, status, var);

  if (status == (int)BEGINDOWNLOAD)
    is_downloading_ = true;

  return ret;
}

void WebBrowser::UpdateCookie(const std::wstring& json) {
  if (delegate_)
    delegate_->OnUpdateCookie(json);
}

void WebBrowser::FinishDownload(bool is_cancel) {
  if (server_provider_ == NULL || is_downloading_ == false)
    return;

  if (is_cancel) {
    IDownloadManager* download_manager = NULL;
    server_provider_->QueryService(SID_SDownloadManager, IID_IDownloadManager,
                                   (void**)&download_manager);
    if (download_manager == NULL)
      return;

    DownLoadManager* download_manager2 =
        static_cast<DownLoadManager*>(download_manager);
    if (download_manager2 == NULL)
      return;

    download_manager2->CancelDownload();
    is_downloading_ = false;
    download_manager->Release();
  }

  is_downloading_ = false;
  MaybeCloseOnFinishDownload();
}

void WebBrowser::SetFunctionControl(const std::wstring& json) {
  std::string temp = base::UTF16ToUTF8(json);

  std::unique_ptr<base::Value> root = base::JSONReader().ReadToValue(temp);
  if (root.get() == NULL || root->is_dict() == false)
    return;

  base::DictionaryValue* root_dict = NULL;
  base::DictionaryValue* function_control = NULL;
  root->GetAsDictionary(&root_dict);
  root_dict->GetDictionary("ieFunctionControl", &function_control);
  if (function_control == NULL)
    return;

  function_control->GetBoolean("mouseRightButtonEnabled",
                               &mouse_right_button_enabled_);
  function_control->GetBoolean("cutCopyEnabled", &cut_copy_enabled_);
  function_control->GetBoolean("printEnabled", &print_enabled_);
  function_control->GetBoolean("saveFileEnabled", &save_file_enabled_);
  function_control->GetString("userAgentString", &ua_add_string_);
  AddUAString(ua_add_string_);

  if (doc_host_hander_) {
    doc_host_hander_->EnableCutCopy(cut_copy_enabled_);
    doc_host_hander_->EnableMouseRightButton(mouse_right_button_enabled_);
    doc_host_hander_->EnablePrint(print_enabled_);
    doc_host_hander_->EnableSaveFile(save_file_enabled_);
  }
  base::DictionaryValue* private_dns = NULL;
  if (root_dict->GetDictionary("privateDNS", &private_dns)) {
    std::string buff = "";
    base::JSONWriter::Write(*private_dns, &buff);
    net::HostResolverImpl::SetPrivateDnsValue(buff);
  }
}

bool WebBrowser::AutoLoginToSite(const std::wstring& frame_xpath,
                                 const std::wstring& uname_xpath,
                                 const std::wstring& uname_value,
                                 const std::wstring& password_xpath,
                                 const std::wstring& password_value,
                                 const std::wstring& login_button_xpath) {
  std::wstring auto_login_id =
      uname_xpath + L"-" + password_xpath + L"-" + login_button_xpath;
  auto it = auto_login_times_.find(auto_login_id);
  if (it != auto_login_times_.end()) {
    DWORD last_auto_login_time = it->second;
    if (GetTickCount() - last_auto_login_time < 3000) {
      LOG(WARNING) << "Time diff is " << GetTickCount() - last_auto_login_time
                   << ", don't auto login to site.";
      return false;
    }
  }

  LOG(WARNING) << "Do auto login to site.";

  CComPtr<IHTMLDOMNode> start_document_dom = GetRootDomNode();
  if (frame_xpath.empty() == false)
    start_document_dom = GetSubFrameDocDomNode(start_document_dom, frame_xpath);
  if (start_document_dom == NULL)
    return false;

  CComPtr<IHTMLDOMNode> username_input =
      GetDomNodeFromXPath(start_document_dom, uname_xpath);
  if (username_input == NULL)
    return false;

  CComPtr<IHTMLDOMNode> password_input =
      GetDomNodeFromXPath(start_document_dom, password_xpath);
  if (password_input == NULL)
    return false;

  CComPtr<IHTMLDOMNode> login_button =
      GetDomNodeFromXPath(start_document_dom, login_button_xpath);
  if (login_button == NULL)
    return false;

  CComPtr<IHTMLElement> element = NULL;
  username_input->QueryInterface(IID_IHTMLElement, (void**)&element);
  if (element)
    element->setAttribute(L"value", variant_t(uname_value.c_str()));

  password_input->QueryInterface(IID_IHTMLElement, (void**)&element);
  if (element)
    element->setAttribute(L"value", variant_t(password_value.c_str()));

  login_button->QueryInterface(IID_IHTMLElement, (void**)&element);
  if (element)
    element->click();

  auto_login_times_[auto_login_id] = GetTickCount();

  return true;
}

void WebBrowser::NotifyCommand(const std::wstring& command) {
  if (event_handler_ == NULL)
    return;

  event_handler_->NotifyRefresh(command == L"refresh");
  ClearListenLoginXpath();
}

void WebBrowser::TryGetLoginInfo(CComPtr<IHTMLElement> click_element,
                                 bool enter_key_pressed) {
  if (login_info_name_xpath_.empty() || login_info_password_xpath_.empty() ||
      login_info_button_xpath_.empty())
    return;

  // CComPtr<IHTMLDOMNode> login_button =
  // GetDomNodeFromXPath(L"//*[@id=\"tableheight\"]/tbody/tr[3]/td/table/tbody/tr/td[2]/table/tbody/tr[1]/td[3]/img");
  // //oa login
  CComPtr<IHTMLDOMNode> start_document_dom = GetRootDomNode();
  if (login_info_frame_xpath_.empty() == false)
    start_document_dom =
        GetSubFrameDocDomNode(start_document_dom, login_info_frame_xpath_);
  if (start_document_dom == NULL)
    return;

  CComPtr<IHTMLDOMNode> login_button =
      GetDomNodeFromXPath(start_document_dom, login_info_button_xpath_);
  CComPtr<IHTMLDOMNode> username =
      GetDomNodeFromXPath(start_document_dom, login_info_name_xpath_);
  CComPtr<IHTMLDOMNode> password =
      GetDomNodeFromXPath(start_document_dom, login_info_password_xpath_);

  if (login_button == NULL || username == NULL || password == NULL)
    return;

  CComPtr<IHTMLUniqueName> unique_name_login;
  login_button->QueryInterface(IID_IHTMLUniqueName, (void**)&unique_name_login);
  long num_login = 0;
  unique_name_login->get_uniqueNumber(&num_login);

  CComPtr<IHTMLUniqueName> unique_name_password;
  password->QueryInterface(IID_IHTMLUniqueName, (void**)&unique_name_password);
  long number_password = 0;
  unique_name_password->get_uniqueNumber(&number_password);

  long num_click = -1;
  if (click_element) {
    CComPtr<IHTMLUniqueName> unique_name_click;
    click_element->QueryInterface(IID_IHTMLUniqueName,
                                  (void**)&unique_name_click);
    unique_name_click->get_uniqueNumber(&num_click);
  }

  if (num_login == num_click || enter_key_pressed) {
    CComPtr<IHTMLElement> name_element;
    username->QueryInterface(IID_IHTMLElement, (void**)&name_element);
    variant_t name = L"";
    name_element->getAttribute(L"value", 0, name.GetAddress());

    CComPtr<IHTMLElement> password_element;
    password->QueryInterface(IID_IHTMLElement, (void**)&password_element);
    variant_t pwd = L"";
    password_element->getAttribute(L"value", 0, pwd.GetAddress());

    if (name.vt != VT_BSTR || name.bstrVal == NULL || pwd.vt != VT_BSTR ||
        pwd.bstrVal == NULL)
      return;

    login_info_name_ = name.bstrVal;
    login_info_password_ = pwd.bstrVal;
  }
}

void WebBrowser::SetListenLoginXPath(const std::wstring& url,
                                     const std::wstring& frame_xpath,
                                     const std::wstring& name_xpath,
                                     const std::wstring& password_xpath,
                                     const std::wstring& login_button_xpath) {
  login_info_url_ = url;
  login_info_frame_xpath_ = frame_xpath;
  login_info_name_xpath_ = name_xpath;
  login_info_password_xpath_ = password_xpath;
  login_info_button_xpath_ = login_button_xpath;

  if (login_info_frame_xpath_.empty() == false) {
    CComPtr<IHTMLDOMNode> document_dom = GetRootDomNode();
    document_dom = GetSubFrameDocDomNode(document_dom, login_info_frame_xpath_);
    if (document_dom) {
      CComPtr<IHTMLElement> element;
      document_dom->QueryInterface(IID_IHTMLElement, (void**)&element);
      if (element == NULL)
        return;

      CComPtr<IDispatch> dispatch;
      element->get_document(&dispatch);
      if (dispatch == NULL)
        return;

      CComPtr<IHTMLDocument2> document2;
      dispatch->QueryInterface(IID_IHTMLDocument2, (void**)&document2);
      if (document2 && event_handler_)
        event_handler_->ListenDocmentEvent(document2);
    }
  }
}

void WebBrowser::ConfirmLoginSuccess() {
  if (login_info_name_.empty() || login_info_password_.empty() ||
      login_info_name_xpath_.empty() || login_info_password_xpath_.empty() ||
      login_info_button_xpath_.empty() || delegate_ == NULL)
    return;

  CComPtr<IHTMLDOMNode> start_document_dom = GetRootDomNode();
  if (login_info_frame_xpath_.empty() == false)
    start_document_dom =
        GetSubFrameDocDomNode(start_document_dom, login_info_frame_xpath_);

  CComPtr<IHTMLDOMNode> login_button =
      GetDomNodeFromXPath(start_document_dom, login_info_button_xpath_);
  if (login_button)
    return;
  CComPtr<IHTMLDOMNode> username =
      GetDomNodeFromXPath(start_document_dom, login_info_name_xpath_);
  if (username)
    return;
  CComPtr<IHTMLDOMNode> password =
      GetDomNodeFromXPath(start_document_dom, login_info_password_xpath_);
  if (password)
    return;

  delegate_->OnGetLoginInfo(login_info_url_, login_info_name_,
                            login_info_password_);
  ClearListenLoginXpath();
}

bool WebBrowser::IsDevToolsOpened() {
  if (dev_tools_host_ == NULL)
    return false;

  return dev_tools_host_->IsOpened();
}

void WebBrowser::ShowDevTools(bool show) {
  if (dev_tools_host_ == NULL)
    return;

  if (base::win::GetVersion() >= base::win::VERSION_WIN10) {
    wchar_t system_dir[_MAX_PATH + 1] = {0};
    if (SHGetSpecialFolderPath(NULL, system_dir, CSIDL_SYSTEMX86, FALSE) ==
        TRUE) {
      std::wstring path = std::wstring(system_dir) + L"\\F12";
      ShellExecute(NULL, L"open", (path + L"\\F12Chooser.exe").c_str(), L"",
                   path.c_str(), SW_SHOW);
      return;
    }
  }

  if (show)
    dev_tools_host_->ShowDevTools();
  else
    dev_tools_host_->CloseDevTools();
}

void WebBrowser::SetRect(const RECT& rect) {
  rect_object_ = rect;

  if (ole_object_) {
    RECT metric_rect = PixelToHiMetric(rect_object_);
    SIZEL size;
    size.cx = metric_rect.right - metric_rect.left;
    size.cy = metric_rect.bottom - metric_rect.top;
    ole_object_->SetExtent(DVASPECT_CONTENT, &size);
  }

  if (ole_in_place_object_ != 0) {
    ole_in_place_object_->SetObjectRects(&rect_object_, &rect_object_);
  }
}

// ----- Control methods -----

void WebBrowser::GoBack() {
  this->web_browser2_->GoBack();
}

void WebBrowser::GoForward() {
  this->web_browser2_->GoForward();
}

void WebBrowser::Refresh() {
  this->web_browser2_->Refresh();
}

void WebBrowser::Navigate(const std::wstring& url) {
  variant_t flags;  // (0x02u); //navNoHistory
  // this->web_browser2_->Navigate(url, &flags, 0, 0, 0);

  variant_t url2(url.c_str());

  // variant_t user_agent(L"User-Agent: Mozilla/5.0 (Windows NT 6.1; WOW64)
  // AppleWebKit/537.36 (KHTML, like Gecko) Chrome/68.0.2840.9999
  // Safari/537.36");
  web_browser2_->Navigate2(&url2, &flags, 0, 0, 0);
}

HRESULT WebBrowser::ExcuteJavaScript(std::wstring& function_name,
                                     std::vector<variant_t>& params,
                                     variant_t* result) {
  if (event_handler_)
    return event_handler_->ExcuteJs(function_name, params, result);

  return E_FAIL;
}

HRESULT WebBrowser::ExcuteJavaScript(std::wstring& code, variant_t* result) {
  if (event_handler_)
    return event_handler_->ExcuteJs(code, result);

  return E_FAIL;
}

int WebBrowser::GetDocumentMode() {
  if (event_handler_)
    return event_handler_->GetDocMode();

  return 0;
}

bool WebBrowser::SetDocumentMode(DocumentMode mode) {
  if (event_handler_)
    return event_handler_->SetDocMode(mode);

  return false;
}

bool WebBrowser::SetBrowserCookie(const std::wstring& json, const int flag) {
  std::string temp = base::UTF16ToUTF8(json);

  std::unique_ptr<base::Value> root = base::JSONReader().ReadToValue(temp);
  if (root.get() == NULL || root->is_dict() == false) {
    return false;
  }

  base::DictionaryValue* root_dict = NULL;
  root->GetAsDictionary(&root_dict);
  base::ListValue* cookies = NULL;
  root_dict->GetList("allCookies", &cookies);
  base::ListValue::iterator iter = cookies->begin();
  for (; iter != cookies->end(); iter++) {
    base::DictionaryValue* cookie = NULL;
    (*iter).GetAsDictionary(&cookie);
    if (cookie == NULL)
      continue;

    std::wstring url = L"";
    cookie->GetString("url", &url);
    std::wstring buff = L"";
    cookie->GetString("cookie", &buff);
    int option_flag = 0;
    size_t find_pos = buff.find(L"HTTPONLY");
    if (find_pos != std::wstring::npos)
      option_flag = option_flag | INTERNET_COOKIE_HTTPONLY;

    find_pos = buff.find(L"; INTERNET_COOKIE_THIRD_PARTY");
    if (find_pos != std::wstring::npos) {
      option_flag = option_flag | INTERNET_COOKIE_THIRD_PARTY;
      buff = buff.substr(
          0, buff.size() - wcslen(L"; INTERNET_COOKIE_THIRD_PARTY"));
    }
    option_flag = option_flag | INTERNET_COOKIE_EVALUATE_P3P;
    std::wstring p3p =
        L"CP=ALL IND DSP COR ADM CONo CUR CUSo IVAo IVDo PSA PSD TAI TELo OUR "
        L"SAMo CNT COM INT NAV ONL PHY PRE PUR UNI";

    EnterCriticalSection(&critical_section);
    set_cookie_self = true;
    DWORD ret =
        DetourInternetSetCookieExW(url.c_str(), NULL, buff.c_str(), option_flag,
                                   (DWORD_PTR)(LPCTSTR)p3p.c_str());
    if (ret != COOKIE_STATE_ACCEPT && ret != COOKIE_STATE_LEASH &&
        ret != COOKIE_STATE_DOWNGRADE)
      LOG(WARNING) << "SetCookie Failed! \r\n url:" << url.c_str()
                   << "\r\n cookies:" << buff.c_str();

    if (ret == COOKIE_STATE_LEASH)
      LOG(WARNING) << "SetCookie Succed! But return value is "
                      "COOKIE_STATE_LEASH \r\n url:"
                   << url.c_str() << "\r\n cookies:" << buff.c_str();

    if (ret == COOKIE_STATE_DOWNGRADE)
      LOG(WARNING) << "SetCookie Succed! But return value is "
                      "COOKIE_STATE_DOWNGRADE \r\n url:"
                   << url.c_str() << "\r\n cookies:" << buff.c_str();

    set_cookie_self = false;
    LeaveCriticalSection(&critical_section);
  }
  return true;
}

std::vector<std::wstring> WebBrowser::SpliteCookieKeyVal(
    const std::wstring& cookie) {
  std::vector<std::wstring> vector;
  std::wistringstream string_stream(cookie);
  wchar_t buff[10240] = {0};
  while (string_stream.getline(buff, 10240, L';')) {
    vector.push_back(buff);
    memset(buff, 0, sizeof(wchar_t) * 10240);
  }
  return vector;
}

HRESULT WebBrowser::SetUserAgent(const std::wstring& user_agent) {
  if (fpUrlSetSessionOption == NULL) {
    url_mon_handle_ = LoadLibraryExW(L"urlmon.dll", NULL, NULL);
    if (url_mon_handle_ == NULL)
      return E_FAIL;

    fpUrlSetSessionOption = (UrlSetSessionOptionFun)GetProcAddress(
        url_mon_handle_, "UrlMkSetSessionOption");
  }

  if (fpUrlSetSessionOption == NULL || user_agent.empty())
    return E_FAIL;

  std::string buffer = base::UTF16ToASCII(user_agent);
  HRESULT handle_result = fpUrlSetSessionOption(
      URLMON_OPTION_USERAGENT, (LPVOID)buffer.c_str(), buffer.size(), 0);
  return handle_result;
}

LRESULT WebBrowser::SubClassControlWndProc(HWND window_handle,
                                           UINT message,
                                           WPARAM w_param,
                                           LPARAM l_param) {
  if (message == WM_COOKIEUPDATED && w_param) {
    std::wstring* json_string = (std::wstring*)w_param;
    WebBrowser::GetWebBrowser()->UpdateCookie(*json_string);
    delete json_string;
    return TRUE;
  } else if (message == WM_DESTROY) {
    if (WebBrowser::GetWebBrowser())
      WebBrowser::GetWebBrowser()->event_handler_->ResetDocHostUIHandler();
    LRESULT ret = CallWindowProc(old_control_proc_, window_handle, message,
                                 w_param, l_param);
    if (WebBrowser::GetWebBrowser())
      WebBrowser::GetWebBrowser()->delegate_->OnBrowserClosing();

    return ret;
  }

  return CallWindowProc(old_control_proc_, window_handle, message, w_param,
                        l_param);
}

bool WebBrowser::PreTranslateMsg(LPMSG message) {
  if (message == NULL)
    return false;

  if (message->message == WM_MOUSEWHEEL) {
    int fw_keys = GET_KEYSTATE_WPARAM(message->wParam);
    if (fw_keys == MK_CONTROL) {
      int x = GET_X_LPARAM(message->lParam);
      int y = GET_Y_LPARAM(message->lParam);
      int delta = GET_WHEEL_DELTA_WPARAM(message->wParam);
      int modifiers = 1024;
      // kControllkey by Webb
      modifiers = modifiers | blink::WebInputEvent::kControlKey;
      if (WebBrowser::GetWebBrowser()) {
        WebBrowser::GetWebBrowser()
            ->delegate_->OnRequestAcceleratorFromMouseWheel(delta, x, y,
                                                            modifiers);
        return true;
      }
    }
  } else if (message->message == WM_KEYDOWN || message->message == WM_KEYUP) {
    bool translate = true;
    WPARAM w_param = message->wParam;

    if (w_param == VK_BACK
        //|| w_param == VK_F5
        || w_param == VK_ESCAPE) {
      translate = false;
      // if (w_param == VK_F5 && self_)
      //{
      //  HWND hMainHwnd = GetMainFrameHwnd(self_->parent_handle_);
      //  PostMessage(hMainHwnd, message, w_param, l_param);
      //  return true;
      //}
    } else if ((w_param == VK_F12 || w_param == VK_F11 || w_param == VK_F5 ||
                w_param == 'I') &&
               message->message == WM_KEYDOWN) {
      bool hit = true;
      int modifiers = 1024;
      if (w_param == 'I') {
        short state_ctrl = GetAsyncKeyState(VK_CONTROL);
        short state_shift = GetAsyncKeyState(VK_SHIFT);
        if ((state_ctrl & 0x8000) == 0 || (state_shift & 0x8000) == 0) {
          hit = false;
        } else
          modifiers = modifiers | blink::WebInputEvent::kControlKey |
                      blink::WebInputEvent::kShiftKey;
      }
      if (hit && WebBrowser::GetWebBrowser()) {
        WebBrowser::GetWebBrowser()
            ->delegate_->OnRequestAcceleratorFromKeyboard(w_param, modifiers);
        return true;
      }
    } else if ((w_param == VK_ADD || w_param == VK_SUBTRACT) &&
               message->message == WM_KEYDOWN) {
      int modifiers = 1024;
      short state = GetAsyncKeyState(VK_CONTROL);
      if (state & 0x8000) {
        modifiers = modifiers | blink::WebInputEvent::kControlKey;
        if (WebBrowser::GetWebBrowser()) {
          WebBrowser::GetWebBrowser()
              ->delegate_->OnRequestAcceleratorFromKeyboard(w_param, modifiers);
          return true;
        }
      }
    } else if (w_param == 'C' || w_param == 'X') {
      // 屏蔽剪切复制快捷键
      short state = GetAsyncKeyState(VK_CONTROL);
      if ((state & 0x8000) && self_ && self_->cut_copy_enabled_ == false) {
        return true;
      }
    } else if (w_param == 'P') {
      // 屏蔽打印快捷键
      short state = GetAsyncKeyState(VK_CONTROL);
      if ((state & 0x8000) && self_ && self_->print_enabled_ == false) {
        return true;
      }
    } else if (w_param == 'O') {
      // 屏蔽打开文件快捷键
      short state = GetAsyncKeyState(VK_CONTROL);
      if (state & 0x8000)
        return true;
    }
    if (ole_in_place_active_object_ && translate) {
      HRESULT handle_result =
          ole_in_place_active_object_->TranslateAccelerator(message);
      if (handle_result == S_OK) {
        if (w_param == VK_RETURN && WebBrowser::GetWebBrowser())
          WebBrowser::GetWebBrowser()->delegate_->OnClickElement(NULL, true);
        return true;
      }
    }
  }

  return false;
}

LRESULT CALLBACK WebBrowser::MessageProc(int code,
                                         WPARAM w_param,
                                         LPARAM l_param) {
  if (code < 0)
    return CallNextHookEx(next_hook_, code, w_param, l_param);

  LPMSG message = (LPMSG)l_param;
  if (PreTranslateMsg(message))
    return TRUE;

  return CallNextHookEx(next_hook_, code, w_param, l_param);
}

bool WebBrowser::EnableSubClass(HWND window_handle) {
  if (web_browser2_ == NULL || IsWindow(window_handle) == FALSE)
    return false;

  web_browser2_->QueryInterface(IID_IOleInPlaceActiveObject,
                                (void**)&ole_in_place_active_object_);
  if (ole_in_place_active_object_ == NULL)
    return false;

  next_hook_ = SetWindowsHook(WH_MSGFILTER, MessageProc);

  ie_browser_handle_ = window_handle;
  old_window_proc_ =
      (WNDPROC)SetWindowLong(window_handle, GWL_WNDPROC, (long)SubClassWndProc);
  return true;
}

void WebBrowser::DisableSubClass(HWND window_handle) {
  if (IsWindow(window_handle) == FALSE || old_window_proc_ == NULL ||
      next_hook_ == NULL)
    return;

  SetWindowLong(window_handle, GWL_WNDPROC, (long)old_window_proc_);
  UnhookWindowsHookEx(next_hook_);
  self_ = NULL;

  if (ole_in_place_active_object_)
    ole_in_place_active_object_->Release();
}

static BOOL parentIsForegroundWindow(HWND hwnd) {
  HWND hForeground = ::GetForegroundWindow();
  HWND hParentWnd = ::GetParent(hwnd);

  while (hParentWnd) {
    if (hForeground == hParentWnd)
      return TRUE;

    hParentWnd = ::GetParent(hParentWnd);
  }

  return FALSE;
}

void WebBrowser::MaybeCloseOnFinishDownload() {
  if (is_downloading_ == true || web_browser2_ == NULL)
    return;

  IHTMLDocument2* document = NULL;
  web_browser2_->get_Document((IDispatch**)&document);
  if (document == NULL)
    return;

  IHTMLElement* body = NULL;
  document->get_body(&body);
  if (body == NULL)
    return;

  BSTR buff = NULL;
  body->get_innerHTML(&buff);
  body->Release();
  document->Release();
  bstr_t html(buff, false);
  if (html.length() == 0 && event_handler_) {
    delegate_->OnBrowserClosing();
    //::PostQuitMessage(0);
  }
}

LRESULT WebBrowser::SubClassWndProc(HWND window_handle,
                                    UINT message,
                                    WPARAM w_param,
                                    LPARAM l_param) {
  if (message == WM_INITMENUPOPUP)
    return 0;

  if (message == WM_MOUSEACTIVATE) {
    if (!parentIsForegroundWindow(window_handle)) {
      HWND hwnd = GetParent(window_handle);
      hwnd = GetParent(hwnd);
      hwnd = GetParent(hwnd);
      hwnd = GetParent(hwnd);
      ::PostMessage(hwnd, WM_IE_MOUSEACTIVATE, NULL, NULL);
    }
  }

  return CallWindowProc(old_window_proc_, window_handle, message, w_param,
                        l_param);
}

HWND WebBrowser::GetMainFrameHwnd(HWND window_handle) {
  if (IsWindow(window_handle) == FALSE)
    return NULL;

  HWND hMainFrameWnd = NULL;
  while (window_handle) {
    window_handle = GetParent(window_handle);
    if (window_handle && IsWindow(window_handle))
      hMainFrameWnd = window_handle;
  }

  GetLastError();
  return hMainFrameWnd;
}

void WebBrowser::AddUAString(const std::wstring& user_agent) {
  if (old_ua_.empty()) {
    if (fpUrlGetSessionOption == NULL) {
      url_mon_handle_ = LoadLibraryExW(L"urlmon.dll", NULL, NULL);
      if (url_mon_handle_ == NULL)
        return;
      fpUrlGetSessionOption = (UrlMkGetSessionOptionFun)GetProcAddress(
          url_mon_handle_, "UrlMkGetSessionOption");
    }
    if (fpUrlGetSessionOption == NULL || user_agent.empty())
      return;

    HRESULT handle_result = S_OK;
    DWORD length = 0;
    handle_result =
        UrlMkGetSessionOption(URLMON_OPTION_USERAGENT, NULL, 0, &length, 0);
    if (length <= 0)
      return;

    int size = length + 1;
    char* ua_buffer = new char[size];
    memset(ua_buffer, 0, sizeof(size));
    length = 0;
    handle_result = UrlMkGetSessionOption(URLMON_OPTION_USERAGENT, ua_buffer,
                                          size - 1, &length, 0);
    if (length == 0) {
      delete[] ua_buffer;
      return;
    }
    old_ua_ = base::ASCIIToUTF16(std::string(ua_buffer));
    delete[] ua_buffer;
  }

  std::wstring ua = old_ua_;
  ua = ua + L" " + user_agent;
  SetUserAgent(ua);
}

CComPtr<IHTMLDOMNode> WebBrowser::GetRootDomNode() {
  CComPtr<IHTMLDOMNode> rood_dom_node = NULL;
  if (web_browser2_ == NULL)
    return rood_dom_node;

  CComPtr<IDispatch> doc_dispatch = NULL;
  web_browser2_->get_Document(&doc_dispatch);
  if (doc_dispatch == NULL)
    return rood_dom_node;

  CComPtr<IHTMLDocument> html_document = NULL;
  doc_dispatch->QueryInterface(IID_IHTMLDocument, (void**)&html_document);
  if (html_document == NULL)
    return rood_dom_node;

  CComPtr<IHTMLDocument3> html_document3;
  html_document->QueryInterface(IID_IHTMLDocument3, (void**)&html_document3);
  if (html_document3 == NULL)
    return rood_dom_node;

  CComPtr<IHTMLElement> root_element;
  html_document3->get_documentElement(&root_element);
  if (root_element == NULL)
    return rood_dom_node;

  root_element->QueryInterface(IID_IHTMLDOMNode, (void**)&rood_dom_node);
  return rood_dom_node;
}

CComPtr<IHTMLDOMNode> WebBrowser::GetSubFrameDocDomNode(
    CComPtr<IHTMLDOMNode> parent_document_dom,
    const std::wstring& frame_xpath) {
  CComPtr<IHTMLDOMNode> sub_document_dom_node = NULL;

  if (frame_xpath.empty() || parent_document_dom == NULL)
    return sub_document_dom_node;

  CComPtr<IHTMLDOMNode> frame_node =
      GetDomNodeFromXPath(parent_document_dom, frame_xpath);
  if (frame_node == NULL)
    return sub_document_dom_node;

  CComPtr<IHTMLElement> element;
  frame_node->QueryInterface(IID_IHTMLElement, (void**)&element);
  if (element == NULL)
    return sub_document_dom_node;

  CComPtr<IHTMLIFrameElement> iframe_element;
  CComPtr<IHTMLFrameElement> frame_element;
  element->QueryInterface(IID_IHTMLIFrameElement, (void**)&iframe_element);

  if (iframe_element == NULL)
    element->QueryInterface(IID_IHTMLFrameElement, (void**)&frame_element);

  if (iframe_element == NULL && frame_element == NULL)
    return sub_document_dom_node;

  CComPtr<IHTMLFrameBase2> pFrameBase2;
  if (iframe_element)
    iframe_element->QueryInterface(IID_IHTMLFrameBase2, (void**)&pFrameBase2);
  else if (frame_element)
    frame_element->QueryInterface(IID_IHTMLFrameBase2, (void**)&pFrameBase2);

  if (pFrameBase2 == NULL)
    return sub_document_dom_node;

  CComPtr<IHTMLWindow2> window2;
  pFrameBase2->get_contentWindow(&window2);
  if (window2 == NULL)
    return sub_document_dom_node;

  CComPtr<IHTMLDocument2> document2;
  window2->get_document(&document2);
  if (document2 == NULL)
    return sub_document_dom_node;

  CComPtr<IHTMLElement> body;
  document2->get_body(&body);
  bstr_t html;
  if (body)
    body->get_innerHTML(html.GetAddress());

  CComPtr<IHTMLDocument3> html_document3;
  document2->QueryInterface(IID_IHTMLDocument3, (void**)&html_document3);
  if (html_document3 == NULL)
    return sub_document_dom_node;

  CComPtr<IHTMLElement> sub_document_element;
  html_document3->get_documentElement(&sub_document_element);
  if (sub_document_element == NULL)
    return sub_document_dom_node;

  sub_document_element->QueryInterface(IID_IHTMLDOMNode,
                                       (void**)&sub_document_dom_node);
  return sub_document_dom_node;
}

CComPtr<IHTMLDOMNode> WebBrowser::GetDomNodeFromXPath(
    CComPtr<IHTMLDOMNode> document,
    const std::wstring& xpath) {
  CComPtr<IHTMLDOMNode> find_dom_node = NULL;
  if (web_browser2_ == NULL || xpath.empty() || document == NULL)
    return find_dom_node;

  ie::XpathParse parse;
  parse.Parse(xpath);
  parse.BeginMatch();

  //  FILE* logFile = NULL;
  //  logFile = _wfopen(L"F:\\html.txt", L"wb+");

  std::stack<CComPtr<IHTMLDOMNode>> dom_stack;
  dom_stack.push(document);
  while (dom_stack.empty() == false) {
    CComPtr<IHTMLDOMNode> current_node = dom_stack.top();
    dom_stack.pop();
    CComBSTR node_name;
    current_node->get_nodeName(&node_name);
    // fwrite(node_name, 1, node_name.Length() * sizeof(wchar_t), logFile);
    // fwrite(L"\r\n", 1, 4, logFile);

    ie::MatchResult match_return = parse.StepMatch(current_node);
    if (match_return == ie::RESULT_NOT_MATCH)
      continue;
    if (match_return == ie::RESULT_ALL_NODE_MATCH) {
      find_dom_node = current_node;
      break;
      // CComPtr<IHTMLElement> element = NULL;
      // current_node->QueryInterface(IID_IHTMLElement, (void**)&element);
      // if (element)
      //{
      //  element->setAttribute(L"value", variant_t(L"chjyTest"));
      //  break;
      //}
    }

    VARIANT_BOOL is_has_child = VARIANT_FALSE;
    current_node->hasChildNodes(&is_has_child);
    if (is_has_child == VARIANT_TRUE) {
      CComPtr<IHTMLDOMNode> child_node;
      current_node->get_lastChild(&child_node);

      if (child_node == NULL)
        return find_dom_node;

      long type = 0;
      child_node->get_nodeType(&type);
      if (type == 1) {
        dom_stack.push(child_node);
        child_node->get_nodeName(&node_name);
      }

      CComPtr<IHTMLDOMNode> sibling_node = child_node;
      while (sibling_node != NULL) {
        sibling_node->get_previousSibling(&sibling_node);
        if (sibling_node == NULL)
          break;

        type = 0;
        sibling_node->get_nodeType(&type);
        if (type == 1) {
          dom_stack.push(sibling_node);
          sibling_node->get_nodeName(&node_name);
        }
      }
    }
  }
  //  fclose(logFile);
  return find_dom_node;
}

void WebBrowser::ClearListenLoginXpath() {
  login_info_frame_xpath_ = L"";
  login_info_url_ = L"";
  login_info_name_xpath_ = L"";
  login_info_password_xpath_ = L"";
  login_info_button_xpath_ = L"";
  login_info_name_ = L"";
  login_info_password_ = L"";
}

IDispatch* WebBrowser::GetIeDispatch() {
  IDispatch* dispatch = NULL;
  if (web_browser2_)
    web_browser2_->QueryInterface(IID_IDispatch, (void**)&dispatch);

  return dispatch;
}

void WebBrowser::Show() {
  if (::IsWindow(GetControlWindow()))
    ::ShowWindow(GetControlWindow(), SW_SHOW);
}

void WebBrowser::Hide() {
  if (::IsWindow(GetControlWindow()))
    ::ShowWindow(GetControlWindow(), SW_HIDE);
}

// ----- IUnknown -----

HRESULT STDMETHODCALLTYPE WebBrowser::QueryInterface(REFIID riid,
                                                     void** ppv_object) {
  if (riid == __uuidof(IUnknown)) {
    (*ppv_object) = (IUnknown*)(IOleClientSite*)(this);
  } else if (riid == __uuidof(IOleClientSite)) {
    (*ppv_object) = static_cast<IOleClientSite*>(this);
  } else if (riid == __uuidof(IOleInPlaceSite)) {
    (*ppv_object) = static_cast<IOleInPlaceSite*>(this);
  } else if (riid == __uuidof(IOleCommandTarget)) {
    // 不能在这里提供自己实现的IDocHostUIHandler接口，
    // 因为这样无法获取IE内核建立的的IDocHostUIHandler::GetExternal接口。
    // 而自己实现的IDocHostUIHandler::GetExternal接口会导致和插件扩展的window.external
    // js函数冲突的问题， 所以要使用IE内核的IDocHostUIHandler::GetExternal接口。
    // 替换成自己实现的IDocHostUIHandler接口的时机在DISPID_DOWNLOADCOMPLETE时间中实现
    // else if (riid == __uuidof(IDocHostUIHandler)) {
    //  (*ppv_object) = static_cast<IDocHostUIHandler*>(doc_host_hander_);
    //}
    (*ppv_object) = static_cast<IOleCommandTarget*>(doc_host_hander_);
  } else if (riid == __uuidof(IServiceProvider)) {
    (*ppv_object) = static_cast<IServiceProvider*>(server_provider_);
  } else {
    return E_NOINTERFACE;
  }

  AddRef();
  return S_OK;
}

ULONG STDMETHODCALLTYPE WebBrowser::AddRef() {
  InterlockedIncrement(&com_ref_count_);
  return com_ref_count_;
}

ULONG STDMETHODCALLTYPE WebBrowser::Release() {
  InterlockedDecrement(&com_ref_count_);
  return com_ref_count_;
}

// ---------- IOleWindow ----------

HRESULT STDMETHODCALLTYPE
WebBrowser::GetWindow(__RPC__deref_out_opt HWND* handle) {
  (*handle) = parent_handle_;
  return S_OK;
}

HRESULT STDMETHODCALLTYPE WebBrowser::ContextSensitiveHelp(BOOL enter_mode) {
  return E_NOTIMPL;
}

// ---------- IOleInPlaceSite ----------

HRESULT STDMETHODCALLTYPE WebBrowser::CanInPlaceActivate() {
  return S_OK;
}

HRESULT STDMETHODCALLTYPE WebBrowser::OnInPlaceActivate() {
  OleLockRunning(ole_object_, TRUE, FALSE);
  ole_object_->QueryInterface(&ole_in_place_object_);
  ole_in_place_object_->SetObjectRects(&rect_object_, &rect_object_);
  return S_OK;
}

HRESULT STDMETHODCALLTYPE WebBrowser::OnUIActivate() {
  return S_OK;
}

HRESULT STDMETHODCALLTYPE WebBrowser::GetWindowContext(
    __RPC__deref_out_opt IOleInPlaceFrame** frame,
    __RPC__deref_out_opt IOleInPlaceUIWindow** document,
    __RPC__out LPRECT position_rect,
    __RPC__out LPRECT clip_rect,
    __RPC__inout LPOLEINPLACEFRAMEINFO frame_info) {
  HWND hwnd = parent_handle_;

  (*frame) = NULL;
  (*document) = NULL;
  (*position_rect).left = rect_object_.left;
  (*position_rect).top = rect_object_.top;
  (*position_rect).right = rect_object_.right;
  (*position_rect).bottom = rect_object_.bottom;
  *clip_rect = *position_rect;

  frame_info->fMDIApp = false;
  frame_info->hwndFrame = hwnd;
  frame_info->haccel = NULL;
  frame_info->cAccelEntries = 0;

  return S_OK;
}

HRESULT STDMETHODCALLTYPE WebBrowser::Scroll(SIZE scroll_extant) {
  return E_NOTIMPL;
}

HRESULT STDMETHODCALLTYPE WebBrowser::OnUIDeactivate(BOOL undoable) {
  return S_OK;
}

HWND WebBrowser::GetControlWindow() {
  if (control_handle_ != 0)
    return control_handle_;

  if (ole_in_place_object_ == 0)
    return 0;

  ole_in_place_object_->GetWindow(&control_handle_);
  return control_handle_;
}

HRESULT STDMETHODCALLTYPE WebBrowser::OnInPlaceDeactivate() {
  control_handle_ = 0;
  // ole_in_place_object_ = 0;

  return S_OK;
}

HRESULT STDMETHODCALLTYPE WebBrowser::DiscardUndoState() {
  return E_NOTIMPL;
}

HRESULT STDMETHODCALLTYPE WebBrowser::DeactivateAndUndo() {
  return E_NOTIMPL;
}

HRESULT STDMETHODCALLTYPE
WebBrowser::OnPosRectChange(__RPC__in LPCRECT position_rect) {
  return S_OK;
}

// ---------- IOleClientSite ----------

HRESULT STDMETHODCALLTYPE WebBrowser::SaveObject() {
  return E_NOTIMPL;
}

HRESULT STDMETHODCALLTYPE
WebBrowser::GetMoniker(DWORD assign,
                       DWORD which_moniker,
                       __RPC__deref_out_opt IMoniker** moniker) {
  if ((assign == OLEGETMONIKER_ONLYIFTHERE) &&
      (which_moniker == OLEWHICHMK_CONTAINER))
    return E_FAIL;

  return E_NOTIMPL;
}

HRESULT STDMETHODCALLTYPE
WebBrowser::GetContainer(__RPC__deref_out_opt IOleContainer** container) {
  return E_NOINTERFACE;
}

HRESULT STDMETHODCALLTYPE WebBrowser::ShowObject() {
  return S_OK;
}

HRESULT STDMETHODCALLTYPE WebBrowser::OnShowWindow(BOOL show) {
  return S_OK;
}

HRESULT STDMETHODCALLTYPE WebBrowser::RequestNewObjectLayout() {
  return E_NOTIMPL;
}

// ----- IStorage -----

HRESULT STDMETHODCALLTYPE
WebBrowser::CreateStream(__RPC__in_string const OLECHAR* name,
                         DWORD mode,
                         DWORD reserved1,
                         DWORD reserved2,
                         __RPC__deref_out_opt IStream** stream) {
  return E_NOTIMPL;
}

HRESULT STDMETHODCALLTYPE WebBrowser::OpenStream(const OLECHAR* name,
                                                 void* reserved1,
                                                 DWORD mode,
                                                 DWORD reserved2,
                                                 IStream** stream) {
  return E_NOTIMPL;
}

HRESULT STDMETHODCALLTYPE
WebBrowser::CreateStorage(__RPC__in_string const OLECHAR* name,
                          DWORD mode,
                          DWORD reserved1,
                          DWORD reserved2,
                          __RPC__deref_out_opt IStorage** storage) {
  return E_NOTIMPL;
}

HRESULT STDMETHODCALLTYPE
WebBrowser::OpenStorage(__RPC__in_opt_string const OLECHAR* name,
                        __RPC__in_opt IStorage* priority,
                        DWORD mode,
                        __RPC__deref_opt_in_opt SNB exclude,
                        DWORD reserved,
                        __RPC__deref_out_opt IStorage** storage) {
  return E_NOTIMPL;
}

HRESULT STDMETHODCALLTYPE WebBrowser::CopyTo(DWORD ciid_exclude,
                                             const IID* rgiid_exclude,
                                             __RPC__in_opt SNB exclude,
                                             IStorage* storage_dest) {
  return E_NOTIMPL;
}

HRESULT STDMETHODCALLTYPE
WebBrowser::MoveElementTo(__RPC__in_string const OLECHAR* name,
                          __RPC__in_opt IStorage* storage_dest,
                          __RPC__in_string const OLECHAR* new_name,
                          DWORD flags) {
  return E_NOTIMPL;
}

HRESULT STDMETHODCALLTYPE WebBrowser::Commit(DWORD commit_flags) {
  return E_NOTIMPL;
}

HRESULT STDMETHODCALLTYPE WebBrowser::Revert() {
  return E_NOTIMPL;
}

HRESULT STDMETHODCALLTYPE WebBrowser::EnumElements(DWORD reserved1,
                                                   void* reserved2,
                                                   DWORD reserved3,
                                                   IEnumSTATSTG** statstg) {
  return E_NOTIMPL;
}

HRESULT STDMETHODCALLTYPE
WebBrowser::DestroyElement(__RPC__in_string const OLECHAR* name) {
  return E_NOTIMPL;
}

HRESULT STDMETHODCALLTYPE
WebBrowser::RenameElement(__RPC__in_string const OLECHAR* old_name,
                          __RPC__in_string const OLECHAR* new_name) {
  return E_NOTIMPL;
}

HRESULT STDMETHODCALLTYPE
WebBrowser::SetElementTimes(__RPC__in_opt_string const OLECHAR* name,
                            __RPC__in_opt const FILETIME* creation_time,
                            __RPC__in_opt const FILETIME* access_time,
                            __RPC__in_opt const FILETIME* modification_time) {
  return E_NOTIMPL;
}

HRESULT STDMETHODCALLTYPE WebBrowser::SetClass(__RPC__in REFCLSID class_id) {
  return S_OK;
}

HRESULT STDMETHODCALLTYPE WebBrowser::SetStateBits(DWORD state_bits,
                                                   DWORD mask) {
  return E_NOTIMPL;
}

HRESULT STDMETHODCALLTYPE WebBrowser::Stat(__RPC__out STATSTG* pstatstg,
                                           DWORD state_flag) {
  return E_NOTIMPL;
}

}  // namespace ie
