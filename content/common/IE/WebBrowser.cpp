#pragma once
#include "WebBrowser.h"
#include "IEEventHandler.h"
#include "third_party/minhook/include/MinHook.h"
#include <vector>
#include <string>
#include <map>
#include <set>
#include <list>
#include <algorithm>
#include "base/win/windows_version.h"
#include <Wininet.h>
#include <WinUser.h>
#include "base/json/json_writer.h"
#include "base/json/json_reader.h"
#include "base/values.h"
#include "base/strings/utf_string_conversions.h"
#include "url/gurl.h"
#include <io.h>
#include <Windows.h>
#include <winternl.h>
#include <sstream>
#include <iostream>
#include <sddl.h>
#include "base/json/json_writer.h"
#include "base/values.h"
#include "base/win/win_util.h"
#include "HttpMonitor.h"
#include <stack>
#include "XPathParse.h"
#include "DevToolsHost.h"
#include "third_party/blink/public/platform/web_input_event.h"
#include <shellapi.h>
#include <ShlObj.h>

#define WM_IE_MOUSEACTIVATE WM_USER+5255
#define WM_COOKIEUPDATED   WM_USER+5801
// #define GWL_WNDPROC (-4) just for compiling
#define GWL_WNDPROC (-4)

namespace IE {

  using namespace std;


  typedef struct _OBJECT_BASIC_INFORMATION {
    ULONG Attributes;
    ACCESS_MASK GrantedAccess;
    ULONG HandleCount;
    ULONG PointerCount;
    ULONG PagedPoolUsage;
    ULONG NonPagedPoolUsage;
    ULONG Reserved[3];
    ULONG NameInformationLength;
    ULONG TypeInformationLength;
    ULONG SecurityDescriptorLength;
    LARGE_INTEGER CreateTime;
  } OBJECT_BASIC_INFORMATION, *POBJECT_BASIC_INFORMATION;

  typedef struct __PUBLIC_OBJECT_TYPE_INFORMATION {
    UNICODE_STRING TypeName;
    ULONG Reserved[22];    // reserved for internal use
  } PUBLIC_OBJECT_TYPE_INFORMATION, *PPUBLIC_OBJECT_TYPE_INFORMATION;

  typedef struct _OBJECT_NAME_INFORMATION {
    UNICODE_STRING ObjectName;
  } OBJECT_NAME_INFORMATION, *POBJECT_NAME_INFORMATION;

  static std::wstring IEFilesArry[] = { L"advpack.dll", L"ieframe.dll",L"iepeers.dll",L"ieproxy.dll",L"iertutil.dll",
                              L"ieui.dll",L"imgutil.dll",L"jscript.dll",L"jsproxy.dll",L"mshtml.dll",
                              L"mshtmled.dll",L"mshtmler.dll",L"msrating.dll",L"pngfilt.dll",L"urlmon.dll",
                              L"wininet.dll",L"xmllite.dll",L"normaliz.dll", L"dxtrans.dll", L"dxtmsft.dll",L"shlwapi.dll",L"jsproxy.dll" };
  static std::wstring IEMuiFileArry[] = { L"8shtmled.dll.mui",L"8srating.dll.mui",L"8shtml.dll.mui",L"8eframe.dll.mui",L"8ertutil.dll.mui",
                                L"8bscript.dll.mui",L"8eui.dll.mui",L"8netcpl.cpl.mui",L"8ininet.dll.mui",L"8shtmler.dll.mui",
                                L"8rlmon.dll.mui", L"8epeers.dll.mui" };
  static std::vector<std::wstring> IECoreFiles = std::vector<std::wstring>(IEFilesArry, IEFilesArry + 22);
  static std::vector<std::wstring> IEMuiFiles= std::vector<std::wstring>(IEMuiFileArry, IEMuiFileArry + 12);

  typedef HMODULE(WINAPI *LOADLIBRARYEXW)(LPCTSTR, HANDLE, DWORD);
  static LOADLIBRARYEXW fpLoadLibExW = NULL;

  typedef DWORD(__stdcall *fMyLdrLoadDll)(PWCHAR, ULONG, UNICODE_STRING*, PHANDLE);
  fMyLdrLoadDll fpLdrLoadDll=NULL;

  typedef DWORD(__stdcall *fMyNtCreateFile)(PHANDLE, ACCESS_MASK, POBJECT_ATTRIBUTES, PIO_STATUS_BLOCK, PLARGE_INTEGER, ULONG, ULONG
                ,ULONG,ULONG,PVOID,ULONG);
  fMyNtCreateFile fpNtCreateFile = NULL;

  typedef NTSTATUS(WINAPI *NTOPENFILE)(PHANDLE, ACCESS_MASK, POBJECT_ATTRIBUTES, PIO_STATUS_BLOCK, ULONG, ULONG);
  NTOPENFILE fpNtOpenFile = NULL;

  typedef LONG(__stdcall *REGOPENKEYEX)(HKEY, LPCTSTR, DWORD, REGSAM, PHKEY);
  REGOPENKEYEX fpRegOpenKeyEX = NULL;

  typedef NTSTATUS(__stdcall *NTOPENKEYEX)
    (
      IN HANDLE       KeyHandle,
      IN ULONG        DesiredAccess,
      IN POBJECT_ATTRIBUTES ObjectAttributes,
      _In_  ULONG              OpenOptions
      );
  NTOPENKEYEX fpNtOpenKeyEx = NULL;

  static std::wstring sidString = L"";

  static std::wstring ieFilePath = L"";

  typedef BOOL(WINAPI *HTTPQUERYINFOW)(HINTERNET, DWORD, LPVOID, LPDWORD, LPDWORD);
  static HTTPQUERYINFOW fpHttpQueryInfoW = NULL;

  typedef BOOL(WINAPI *HTTPQUERYINFOA)(HINTERNET, DWORD, LPVOID, LPDWORD, LPDWORD);
  static HTTPQUERYINFOA fpHttpQueryInfoA = NULL;

  typedef BOOL(WINAPI *INTERNETQUERYOPTION)(HINTERNET, DWORD, LPVOID, LPDWORD);
  static INTERNETQUERYOPTION fpInternetQueryOption = NULL;

  static bool setCookieSelf = false;
  static CRITICAL_SECTION criticalSection = CRITICAL_SECTION();

  typedef BOOL(WINAPI *INTERNETSETCOOKIEEXW)(LPCTSTR, LPCTSTR, LPCTSTR, DWORD, DWORD_PTR);
  static INTERNETSETCOOKIEEXW fpInternetSetCookie = NULL;

  typedef NTSTATUS(__stdcall *QUERYOBJECT)(HANDLE, OBJECT_INFORMATION_CLASS, PVOID, ULONG, PULONG);
  static QUERYOBJECT fpQueryObject = NULL;

  typedef HRESULT(__stdcall *COGETCLASSOBJECT)(REFCLSID, DWORD, COSERVERINFO *, REFIID, LPVOID *);
  static COGETCLASSOBJECT fpCoGetClassObject = NULL;

  typedef HRESULT(__stdcall *LOADREGTYPELIB)(REFGUID, WORD, WORD, LCID, ITypeLib**);
  static LOADREGTYPELIB fpLoadRegTypeLib = NULL;

  static bool cookieHookFlg = false;

  WNDPROC WebBrowser::oldWndProc = NULL;
  WNDPROC WebBrowser::oldControlWndProc = NULL;
  HHOOK WebBrowser::hNextHook = NULL;
  WebBrowser* WebBrowser::pThis = NULL;
  IOleInPlaceActiveObject* WebBrowser::pIOIPAO=NULL;
  static bool isFlashAxHook = false;

  //xp下设置cookie时，可能还未能够完成hook api，所以如果还未hook就把cookie记录在cookieBuffer中，等到hook完成时再设置
  static std::vector<std::pair<std::wstring, std::wstring>> cookieBuffer = std::vector<std::pair<std::wstring, std::wstring>>();

  std::wstring GetIECorePath()
  {
    wchar_t path[MAX_PATH + 1] = { 0 };
    GetModuleFileName(NULL, path, MAX_PATH);
    wchar_t driver[_MAX_DRIVE + 1] = { 0 };
    wchar_t dir[_MAX_DIR + 1] = { 0 };
    wchar_t fname[_MAX_FNAME + 1] = { 0 };
    wchar_t ext[_MAX_EXT + 1] = { 0 };
    ::_wsplitpath_s(path, driver, dir, fname, ext);
    std::wstring ieCorePath = std::wstring(driver) + dir + L"ie8core";
    return ieCorePath;
  }

  std::pair<std::wstring, std::vector<std::wstring>> SpliteCookie(const std::wstring& host, const std::wstring& header)
  {
    std::pair<std::wstring, std::vector<std::wstring>> UrlCookies;
    size_t nPosL = 0, nPosR = 0;
    bool bSecure = false;
    std::vector<std::wstring> cookie;
    std::wstring lowercase = header;
    std::transform(lowercase.begin(), lowercase.end(), lowercase.begin(), ::tolower);
    while (nPosL != std::wstring::npos)
    {
      nPosL = lowercase.find(L"set-cookie: ", nPosR);
      if (nPosL == std::wstring::npos)
        break;
      nPosL += wcslen(L"set-cookie: ");
      nPosR = lowercase.find(L"\r\n", nPosL);
      if (nPosR == std::wstring::npos)
        break;
      std::wstring temp = header.substr(nPosL, nPosR - nPosL);
      cookie.push_back(temp);
    }
    std::wstring url = L"";
    if (bSecure)
      url = L"https://" + host + L"/";
    else
      url = L"http://" + host + L"/";
    UrlCookies = std::make_pair(url, cookie);
    return UrlCookies;
  }

  std::wstring CookieToJson(const std::pair<std::wstring, std::vector<std::wstring>>& cookie)
  {
    base::DictionaryValue rootDict;
    rootDict.SetString("url", cookie.first);
    base::ListValue* list = new base::ListValue;
    list->AppendStrings(cookie.second);
    rootDict.Set("cookies", std::move(std::unique_ptr<base::ListValue>(list)));
    std::string buff = "";
    base::JSONWriter::Write(rootDict, &buff);
    std::wstring json = base::UTF8ToUTF16(buff);
    return json;
  }

  BOOL WINAPI DetourHttpQueryInfoW(HINTERNET hRequest, DWORD dwInfoLevel,
    LPVOID lpBuffer, LPDWORD lpdwBufferLength, LPDWORD lpdwIndex)
  {
    if (fpHttpQueryInfoW == NULL)
      return FALSE;

    BOOL Ret = fpHttpQueryInfoW(hRequest, dwInfoLevel, lpBuffer, lpdwBufferLength, lpdwIndex);
    if (Ret == FALSE)
      return Ret;
    if (dwInfoLevel != HTTP_QUERY_RAW_HEADERS_CRLF) //dwInfoLevel == HTTP_QUERY_STATUS_CODE
    {
      //http status返回302时或304时 也有页面会设置cookie，而IE可能并不会调用HTTP_QUERY_RAW_HEADERS_CRLF，导致没能拦截cookie，所以获取所有response
      DWORD len = 0;
      LPVOID buff = NULL;
      if (fpHttpQueryInfoW(hRequest, HTTP_QUERY_RAW_HEADERS_CRLF, (LPVOID)buff, &len, NULL)== FALSE
        && GetLastError()== ERROR_INSUFFICIENT_BUFFER)
      {
        buff = new byte[len+1];
        len = sizeof(byte)*(len + 1);
        memset(buff, 0, len);
        DetourHttpQueryInfoW(hRequest, HTTP_QUERY_RAW_HEADERS_CRLF, buff, &len, NULL);
        delete[] buff;
      }
    }
    else if (dwInfoLevel == HTTP_QUERY_RAW_HEADERS_CRLF)
    {
      std::wstring buff = static_cast<wchar_t*>(lpBuffer);
      std::wstring lowercase = buff;
      std::transform(lowercase.begin(), lowercase.end(), lowercase.begin(), ::tolower);
      size_t pos = lowercase.find(L"set-cookie:");
      if (pos == std::wstring::npos)
        return Ret;
      DWORD hostSize = 10240;
      wchar_t* host = new wchar_t[hostSize];
      memset(host, 0, sizeof(wchar_t) * hostSize);
      if (fpHttpQueryInfoW(hRequest, HTTP_QUERY_FLAG_REQUEST_HEADERS | HTTP_QUERY_HOST, host, &hostSize, NULL))
      {
        std::pair<std::wstring, std::vector<std::wstring>> tempPair = SpliteCookie(host, buff);
        std::wstring jsonStr=CookieToJson(tempPair);
        if (WebBrowser::GetWebBrowser())
        {
          HWND hwnd = WebBrowser::GetWebBrowser()->GetControlWindow();
          LRESULT ret = 0;
          std::wstring* postStr = new std::wstring;
          *postStr = jsonStr;
          ret=PostMessage(hwnd, WM_COOKIEUPDATED, (WPARAM)postStr, NULL); //delete after used
        }
      }
      delete[] host;
    }
    return Ret;
  }

  BOOL WINAPI DetourHttpQueryInfoA(HINTERNET hRequest, DWORD dwInfoLevel,
    LPVOID lpBuffer, LPDWORD lpdwBufferLength, LPDWORD lpdwIndex)
  {
    if (fpHttpQueryInfoA == NULL)
      return FALSE;

    BOOL Ret = fpHttpQueryInfoA(hRequest, dwInfoLevel, lpBuffer, lpdwBufferLength, lpdwIndex);
    if (Ret == FALSE)
      return Ret;
    if (dwInfoLevel != HTTP_QUERY_RAW_HEADERS_CRLF) //dwInfoLevel == HTTP_QUERY_STATUS_CODE
    {
      //http status返回302时或304时 也有页面会设置cookie，而IE可能并不会调用HTTP_QUERY_RAW_HEADERS_CRLF，导致没能拦截cookie，所以获取所有response
      DWORD len = 0;
      LPVOID buff = NULL;
      if (fpHttpQueryInfoA(hRequest, HTTP_QUERY_RAW_HEADERS_CRLF, (LPVOID)buff, &len, NULL) == FALSE
        && GetLastError() == ERROR_INSUFFICIENT_BUFFER)
      {
        buff = new byte[len + 1];
        len = sizeof(byte)*(len + 1);
        memset(buff, 0, len);
        DetourHttpQueryInfoA(hRequest, HTTP_QUERY_RAW_HEADERS_CRLF, buff, &len, NULL);
        delete[] buff;
      }
    }
    else if (dwInfoLevel == HTTP_QUERY_RAW_HEADERS_CRLF)
    {
      std::wstring buff = base::ASCIIToUTF16(static_cast<char*>(lpBuffer));
      std::wstring lowercase = buff;
      std::transform(lowercase.begin(), lowercase.end(), lowercase.begin(), ::tolower);
      size_t pos = lowercase.find(L"set-cookie:");
      if (pos == std::wstring::npos)
        return Ret;
      DWORD hostAscSize = 10240;
      char* hostAsc = new char[hostAscSize];
      memset(hostAsc, 0, sizeof(char) * hostAscSize);
      if (fpHttpQueryInfoA(hRequest, HTTP_QUERY_FLAG_REQUEST_HEADERS | HTTP_QUERY_HOST, hostAsc, &hostAscSize, NULL))
      {
        std::wstring host = base::ASCIIToUTF16(hostAsc);
        std::pair<std::wstring, std::vector<std::wstring>> tempPair = SpliteCookie(host, buff);
        std::wstring jsonStr = CookieToJson(tempPair);
        if (WebBrowser::GetWebBrowser())
        {
          HWND hwnd = WebBrowser::GetWebBrowser()->GetControlWindow();
          LRESULT ret = 0;
          std::wstring* postStr = new std::wstring;
          *postStr = jsonStr;
          ret = PostMessage(hwnd, WM_COOKIEUPDATED, (WPARAM)postStr, NULL); //delete after used
        }
      }
      delete[] hostAsc;
    }
    return Ret;
  }

  DWORD WINAPI DetourInternetSetCookieExW(LPCTSTR lpszURL, LPCTSTR lpszCookieName, LPCTSTR lpszCookieData, DWORD dwFlags, DWORD_PTR dwReserved)
  {
    if (fpInternetSetCookie == NULL)
    {
      cookieBuffer.push_back(std::make_pair(lpszURL, lpszCookieData));
      return FALSE;
    }
    std::wstring temp = lpszCookieData;
    DWORD ret = fpInternetSetCookie(lpszURL, lpszCookieName, temp.c_str(), dwFlags, dwReserved);
    EnterCriticalSection(&criticalSection);
    if ((ret == COOKIE_STATE_ACCEPT ||
      ret== COOKIE_STATE_LEASH ||
      ret== COOKIE_STATE_DOWNGRADE)
      && setCookieSelf==false)
    {
      std::wstring cookieSrc = lpszCookieData;
      std::wstring temp = lpszCookieData;
      std::transform(temp.begin(), temp.end(), temp.begin(), ::tolower);
      if ((dwFlags & INTERNET_COOKIE_HTTPONLY) == INTERNET_COOKIE_HTTPONLY)
      {
        size_t pos = temp.find(L"httponly");
        if (pos == temp.npos)
          cookieSrc = cookieSrc + L";  HTTPONLY";
      }
      std::vector<std::wstring> cookieData;
      cookieData.push_back(cookieSrc);
      GURL gurl(lpszURL);
      std::pair<std::wstring, std::vector<std::wstring>> cookies =
        make_pair(base::ASCIIToUTF16(gurl.spec()), cookieData);
      std::wstring json=CookieToJson(cookies);
      if (WebBrowser::GetWebBrowser())
      {
        HWND hwnd = WebBrowser::GetWebBrowser()->GetControlWindow();
        std::wstring* postStr = new std::wstring;
        *postStr = json;
        PostMessage(hwnd, WM_COOKIEUPDATED, (WPARAM)postStr, NULL); //delete after used
      }
    }
    LeaveCriticalSection(&criticalSection);
    return ret;
  }

  bool EnableCookieHook(const LPCTSTR wininetPath)
  {
    if (cookieHookFlg == true)
      return true;

    //hook http response中设置的cookie,IE6版本要hook HttpQueryInfoA函数 其他版本IE hook HttpQueryInfoW
    int sysIEVer = base::win::GetSystemIEVersion();
    if (sysIEVer == 6)
    {
      if (MH_CreateHookApi(wininetPath, "HttpQueryInfoA", &DetourHttpQueryInfoA, (LPVOID*)&fpHttpQueryInfoA) != MH_OK)   //hook http response中设置的cookie
        return false;
    }
    else if(sysIEVer > 6)
    {
      if (MH_CreateHookApi(wininetPath, "HttpQueryInfoW", &DetourHttpQueryInfoW, (LPVOID*)&fpHttpQueryInfoW) != MH_OK)   //hook http response中设置的cookie
        return false;
    }
    if (MH_CreateHookApi(wininetPath, "InternetSetCookieExW", &DetourInternetSetCookieExW, (LPVOID*)&fpInternetSetCookie) != MH_OK)  //hook 通过js设置的cookie
      return false;
    if (MH_EnableHook(MH_ALL_HOOKS) != MH_OK)
      return false;
    cookieHookFlg = true;
    if (cookieBuffer.empty() == false)
    {
      std::vector<std::pair<std::wstring, std::wstring>>::iterator iter = cookieBuffer.begin();
      for (; iter != cookieBuffer.end(); iter++)
      {
        DetourInternetSetCookieExW(iter->first.c_str(), NULL, iter->second.c_str(), 0, NULL);
      }
      cookieBuffer.clear();
    }
    if (fpInternetQueryOption == NULL)
    {
      HMODULE hmod = GetModuleHandle(wininetPath);
      fpInternetQueryOption = (INTERNETQUERYOPTION)GetProcAddress(hmod, "InternetQueryOptionW");
    }
    return true;
  }

  bool DisableCookieHook()
  {
    if (MH_DisableHook(MH_ALL_HOOKS) != MH_OK)
      return false;
    return true;
  }

  HMODULE WINAPI DetourLoadLibraryExW(LPCTSTR fName, HANDLE  hFile, DWORD   dwFlags)
  {
    if (fpLoadLibExW == NULL)
      return NULL;

    std::wstring path = fName;
    std::transform(path.begin(), path.end(), path.begin(), ::tolower);

    bool bNeedCookieHook = false;
    if (path == L"wininet.dll")
      bNeedCookieHook = true;

    std::wstring iePath = ieFilePath;
    std::vector<std::wstring>::iterator iter = IECoreFiles.begin();
    for (; iter != IECoreFiles.end(); iter++)
    {
      int pos = path.find(*iter);
      if (pos >= 0)
      {
        std::wstring temp = *iter;
        if (temp != L"ieproxy.dll" && temp != L"normaliz.dll")
        {
          temp = temp.replace(0, 1, 1, L'8');
        }
        path = iePath + L"\\" + temp;
        if (_waccess(path.c_str(), 0) == -1)
        {
          path = fName;
        }
        if (dwFlags == 0)
          dwFlags = LOAD_WITH_ALTERED_SEARCH_PATH;
        break;
      }
    }
    HMODULE hMod = fpLoadLibExW(path.c_str(), hFile, dwFlags);
    if (hMod && bNeedCookieHook)
      EnableCookieHook(path.c_str());
    if (hMod == 0)
    {
      DWORD err = 0;
      err = GetLastError();
    }
    return hMod;
  }

  std::wstring GetFlashAxPath()
  {
    std::wstring filePath = L"";
    wchar_t fullPath[MAX_PATH] = { 0 };
    HMODULE childDllHMod = LoadLibrary(L"chrome_child.dll");
    if (childDllHMod == NULL)
    {
      childDllHMod = LoadLibrary(L"chrome.dll");
      if (childDllHMod == NULL)
        return filePath;
    }
    GetModuleFileName(childDllHMod, fullPath, MAX_PATH);
    FreeLibrary(childDllHMod);
    wchar_t driver[_MAX_DRIVE + 1] = { 0 };
    wchar_t dir[_MAX_DIR + 1] = { 0 };
    wchar_t fname[_MAX_FNAME + 1] = { 0 };
    wchar_t ext[_MAX_EXT + 1] = { 0 };
    ::_wsplitpath_s(fullPath, driver, dir, fname, ext);
    std::wstring pathTemp = std::wstring(driver) + dir;
    base::win::Version ver = base::win::GetVersion();
    if (ver < base::win::VERSION_WIN8)
      pathTemp = pathTemp + L"FlashAx\\flash_win7.ocx";
    else
      pathTemp = pathTemp + L"FlashAx\\flash_win8.ocx";
    if (_waccess(pathTemp.c_str(), 0) == -1)
      return filePath;
    filePath = pathTemp;
    return filePath;
  }

  DWORD WINAPI DetourLdrLoadDll(PWCHAR PathToFile, ULONG Flags, UNICODE_STRING* ModuleFileName, PHANDLE ModuleHandle)
  {
    std::wstring path = ModuleFileName->Buffer;
    std::transform(path.begin(), path.end(), path.begin(), ::tolower);

    bool bNeedCookieHook = false;

    if (path == L"wininet.dll")
      bNeedCookieHook = true;

    std::wstring iePath = ieFilePath;
    std::vector<std::wstring>::iterator iter = IECoreFiles.begin();
    for (; iter != IECoreFiles.end(); iter++)
    {
      int pos = path.find(*iter);
      if (pos >= 0)
      {
        std::wstring temp = *iter;
        if (temp != L"ieproxy.dll" && temp != L"normaliz.dll" /*&& temp!=L"iepeers.dll"*/)
        {
          temp = temp.replace(0, 1, 1, L'8');
        }
        path = iePath + L"\\" + temp;
        if (_waccess(path.c_str(), 0) == -1)
        {
          path = ModuleFileName->Buffer;
        }
        break;
      }
    }
    UNICODE_STRING fName;
    fName.Buffer = (wchar_t*)path.c_str();
    fName.Length = path.size()*sizeof(wchar_t);
    fName.MaximumLength = 512 * sizeof(wchar_t);
    DWORD dw = fpLdrLoadDll(PathToFile, Flags, &fName, ModuleHandle);
    if (dw==0 && bNeedCookieHook)
      EnableCookieHook(path.c_str());
    if (dw != 0)
    {
      DWORD err = 0;
      err = GetLastError();
    }
    return dw;
  }

  std::wstring GetSidString()
  {
    if (sidString.empty())
    {
      DWORD pid = GetCurrentProcessId();
      HANDLE processHandle = OpenProcess(PROCESS_ALL_ACCESS, FALSE, pid);
      HANDLE token = 0;
      OpenProcessToken(processHandle, TOKEN_QUERY, &token);
      DWORD len = 0;
      GetTokenInformation(token, TokenUser, NULL, len, &len);
      PTOKEN_USER pSid;
      pSid = (PTOKEN_USER)GlobalAlloc(GPTR, len);
      GetTokenInformation(token, TokenUser, pSid, len, &len);
      wchar_t* buff = NULL;
      ConvertSidToStringSid(pSid->User.Sid, &buff);
      sidString = buff;
      LocalFree(buff);
      GlobalFree(pSid);
    }
    return sidString;
  }

  std::wstring QueryHkeyString(HKEY hKey)
  {
    std::wstring hKeyString = L"";
    if (fpQueryObject == NULL)
    {
      HMODULE hModTest = GetModuleHandle(L"ntdll.dll");
      fpQueryObject = (QUERYOBJECT)GetProcAddress(hModTest, "NtQueryObject");
    }
    // comment unused code by webb
    // OBJECT_NAME_INFORMATION info = { 0 };
    unsigned long dw = 0;
    NTSTATUS stat = 0;
    stat = fpQueryObject(hKey, (OBJECT_INFORMATION_CLASS)1, NULL, dw, &dw);
    if (stat != 0xC0000004) //STATUS_INFO_LENGTH_MISMATCH
      return hKeyString;
    POBJECT_NAME_INFORMATION pInfo = (POBJECT_NAME_INFORMATION)GlobalAlloc(GPTR, dw);
    if (pInfo == NULL)
      return hKeyString;
    fpQueryObject(hKey, (OBJECT_INFORMATION_CLASS)1, pInfo, dw, &dw);
    hKeyString = pInfo->ObjectName.Buffer;
    GlobalFree(pInfo);
    return hKeyString;
  }

  LONG WINAPI DetourRegOpenKeyEx(HKEY hKey, LPCTSTR lpSubKey, DWORD ulOptions, REGSAM samDesired, PHKEY phkResult)
  {
    if (fpRegOpenKeyEX == NULL)
      return 1;
    long ret = 0;

    static int log = 0;// 1;
    bool readSys = false;
    bool readHook = false;

    ret = fpRegOpenKeyEX(hKey, lpSubKey, ulOptions, samDesired, phkResult);

    std::wstring regStr = L"";
    if (*phkResult && ret == ERROR_SUCCESS)
    {
      readSys = true;
      regStr = QueryHkeyString(*phkResult);
    }
    else
    {
      readSys = false;
      if (hKey == HKEY_CLASSES_ROOT)
      {
        base::win::Version ver = base::win::GetVersion();
        if(ver== base::win::VERSION_XP)
          regStr = std::wstring(L"\\REGISTRY\\MACHINE\\SOFTWARE\\Classes\\") + lpSubKey;
        else
          regStr = std::wstring(L"\\REGISTRY\\MACHINE\\SOFTWARE\\Classes\\Wow6432Node\\") + lpSubKey;
      }
      else if (hKey == HKEY_LOCAL_MACHINE)
      {
        regStr = std::wstring(L"\\REGISTRY\\MACHINE\\") + lpSubKey;
      }
      else
      {
        regStr = QueryHkeyString(hKey);
        regStr = regStr + L"\\" + lpSubKey;
      }
    }

    std::wstring interfaceKey = L"\\REGISTRY\\MACHINE\\SOFTWARE\\Classes\\Wow6432Node\\Interface\\";
    size_t interfacePos = regStr.find(interfaceKey);
    if (interfacePos != regStr.npos && regStr.size() > interfaceKey.size() /*&& ret == ERROR_SUCCESS*/)
    {
      int len = interfacePos + interfaceKey.size();
      std::wstring subKey = regStr.substr(len);
      subKey = L"Software\\AllMobilize\\Redcore\\IE8\\Interface\\" + subKey;
      HKEY resultKey = 0;
      if (fpRegOpenKeyEX(HKEY_CURRENT_USER, subKey.c_str(), 0, KEY_READ, &resultKey) == ERROR_SUCCESS)
      {
        readHook = true;
        if (*phkResult)
          RegCloseKey(*phkResult);
        *phkResult = resultKey;
        ret = ERROR_SUCCESS;
      }
    }

    std::wstring interfaceKey1 = L"\\REGISTRY\\MACHINE\\SOFTWARE\\Classes\\Interface\\";
    interfacePos = regStr.find(interfaceKey1);
    if (interfacePos != regStr.npos && regStr.size() > interfaceKey1.size() /*&& ret == ERROR_SUCCESS*/)
    {
      int len = interfacePos + interfaceKey1.size();
      std::wstring subKey = regStr.substr(len);
      subKey = L"Software\\AllMobilize\\Redcore\\IE8\\Interface\\" + subKey;
      HKEY resultKey = 0;
      if (fpRegOpenKeyEX(HKEY_CURRENT_USER, subKey.c_str(), 0, KEY_READ, &resultKey) == ERROR_SUCCESS)
      {
        readHook = true;
        if (*phkResult)
          RegCloseKey(*phkResult);
        *phkResult = resultKey;
        ret = ERROR_SUCCESS;
      }
    }

    std::wstring key = L"\\REGISTRY\\MACHINE\\SOFTWARE\\Classes\\Wow6432Node\\CLSID\\";
    size_t pos = regStr.find(key);
    if (pos != regStr.npos && regStr.size() > key.size() /*&& ret == ERROR_SUCCESS*/)
    {
      int len = pos + key.size();
      std::wstring subKey = regStr.substr(len);
      subKey = L"Software\\AllMobilize\\Redcore\\IE8\\CLSID\\" + subKey;
      HKEY resultKey = 0;
      if (fpRegOpenKeyEX(HKEY_CURRENT_USER, subKey.c_str(), 0, KEY_READ, &resultKey) == ERROR_SUCCESS)
      {
        readHook = true;
        if (*phkResult)
          RegCloseKey(*phkResult);
        *phkResult = resultKey;
        ret = ERROR_SUCCESS;
        //log = 0;
      }
    }

    std::wstring key2 = L"\\REGISTRY\\MACHINE\\SOFTWARE\\Classes\\CLSID\\";
    pos = regStr.find(key2);
    if (pos != regStr.npos && regStr.size() > key2.size() /*&& ret == ERROR_SUCCESS*/)
    {
      int len = pos + key2.size();
      std::wstring subKey = regStr.substr(len);
      subKey = L"Software\\AllMobilize\\Redcore\\IE8\\CLSID\\" + subKey;
      HKEY resultKey = 0;
      if (fpRegOpenKeyEX(HKEY_CURRENT_USER, subKey.c_str(), 0, KEY_READ, &resultKey) == ERROR_SUCCESS)
      {
        readHook = true;
        if (*phkResult)
          RegCloseKey(*phkResult);
        *phkResult = resultKey;
        ret = ERROR_SUCCESS;
        //log = 0;
      }
    }
    if (log == 1)
    {
      //FILE* file = _wfopen(L"C:\\reg.txt", L"ab+");
      FILE* file = _wfopen(L"e:\\reg.txt", L"ab+");
      if (file)
      {
        fwrite(regStr.c_str(), 1, regStr.size() * sizeof(wchar_t), file);

        std::wstring s1 = L"  readSys : ";
        if (readSys)
          s1 = s1 + L"true";
        else
          s1 = s1 + L"false";
        fwrite(s1.c_str(), 1, s1.size() * sizeof(wchar_t), file);

        std::wstring s2 = L"  readHook : ";
        if (readHook)
          s2 = s2 + L"true";
        else
          s2 = s2 + L"false";
        fwrite(s2.c_str(), 1, s2.size() * sizeof(wchar_t), file);

        fwrite(L"\r\n", 1, 4, file);
        fclose(file);
      }
    }

    return ret;
  }

  DWORD WINAPI DetourNtCreateFile(PHANDLE FileHandle, ACCESS_MASK DesiredAccess, POBJECT_ATTRIBUTES ObjectAttributes,
    _Out_    PIO_STATUS_BLOCK   IoStatusBlock,
    _In_opt_ PLARGE_INTEGER     AllocationSize,
    _In_     ULONG              FileAttributes,
    _In_     ULONG              ShareAccess,
    _In_     ULONG              CreateDisposition,
    _In_     ULONG              CreateOptions,
    _In_     PVOID              EaBuffer,
    _In_     ULONG              EaLength)
  {
    OBJECT_ATTRIBUTES obj = *ObjectAttributes;
    UNICODE_STRING UniStr = { 0 };
    std::wstring path=obj.ObjectName->Buffer;
    std::transform(path.begin(), path.end(), path.begin(), ::tolower);
    OBJECT_ATTRIBUTES object = *ObjectAttributes;
    std::wstring temp = ieFilePath;
    wchar_t* buff = NULL;

    std::vector<std::wstring>::iterator ieCoreiter = IECoreFiles.begin();
    for (; ieCoreiter != IECoreFiles.end(); ieCoreiter++)
    {
      size_t pos = path.find(*ieCoreiter);
      if (pos != path.npos)
      {
        std::wstring fName = *ieCoreiter;
        if (fName != L"ieproxy.dll" && fName != L"normaliz.dll")
        {
          fName = fName.replace(0, 1, 1, L'8');
        }
        path = ieFilePath + L"\\" + fName;
        if (_waccess(path.c_str(), 0) == -1)
        {
          object = *ObjectAttributes;
        }
        else
        {
          path = L"\\??\\" + path;
          buff = new wchar_t[path.size() + 1];
          memset(buff, 0, (path.size() + 1) * sizeof(wchar_t));
          memcpy(buff, path.c_str(), path.size() * sizeof(wchar_t));
          UniStr.Buffer = buff;
          UniStr.Length = path.size() * sizeof(wchar_t);
          UniStr.MaximumLength = UniStr.Length + sizeof(wchar_t);
          object.ObjectName = &UniStr;
        }
        break;
      }
    }

    if (path.find(L"mshtml.tlb") != path.npos)
    {
      temp = temp + L"\\8shtml.tlb";
      temp = L"\\??\\" + temp;
      buff = new wchar_t[temp.size() + 1];
      memset(buff, 0, (temp.size() + 1) * sizeof(wchar_t));
      memcpy(buff, temp.c_str(), temp.size() * sizeof(wchar_t));
      UniStr.Buffer = buff;
      UniStr.Length = temp.size() * sizeof(wchar_t);
      UniStr.MaximumLength = UniStr.Length + sizeof(wchar_t);
      object.ObjectName = &UniStr;
    }

    if (path.find(L"iepeers.dll") != path.npos)
    {
      temp = temp + L"\\8epeers.dll";
      temp = L"\\??\\" + temp;
      buff = new wchar_t[temp.size() + 1];
      memset(buff, 0, (temp.size() + 1) * sizeof(wchar_t));
      memcpy(buff, temp.c_str(), temp.size() * sizeof(wchar_t));
      UniStr.Buffer = buff;
      UniStr.Length = temp.size() * sizeof(wchar_t);
      UniStr.MaximumLength = UniStr.Length + sizeof(wchar_t);
      object.ObjectName = &UniStr;
    }

    std::vector<std::wstring>::iterator iter = IEMuiFiles.begin();
    for (; iter != IEMuiFiles.end(); iter++)
    {
      int pos = path.find(*iter);
      if (pos >= 0)
      {
        temp = temp + L"\\" + *iter;
        if (_waccess(temp.c_str(), 0) == -1)
        {
          object = *ObjectAttributes;
        }
        else
        {
          temp = L"\\??\\" + temp;
          buff = new wchar_t[temp.size() + 1];
          memset(buff, 0, (temp.size() + 1) * sizeof(wchar_t));
          memcpy(buff, temp.c_str(), temp.size() * sizeof(wchar_t));
          UniStr.Buffer = buff;
          UniStr.Length= temp.size() * sizeof(wchar_t);
          UniStr.MaximumLength= UniStr.Length+ sizeof(wchar_t);
          object.ObjectName = &UniStr;
        }
        break;
      }
    }
    DWORD dw = fpNtCreateFile(FileHandle, DesiredAccess, &object, IoStatusBlock, AllocationSize, FileAttributes,
      ShareAccess, CreateDisposition, CreateOptions, EaBuffer, EaLength);
    if (buff)
    {
      delete[] buff;
      buff = NULL;
    }
    if (dw != 0)
    {
      DWORD err = 0;
      err=GetLastError();
    }
    return dw;
  }

  HRESULT WINAPI DetourLoadRegTypeLib(REFGUID rguid, WORD wVerMajor, WORD wVerMinor, LCID lcid, ITypeLib **pptlib)
  {
    HRESULT hResult;

    std::wstring  flashClsid = L"{D27CDB6B-AE6D-11CF-96B8-444553540000}";
    LPOLESTR buff = NULL;
    HRESULT hr = StringFromCLSID(rguid, &buff);
    std::wstring clsidStr = L"";
    if (SUCCEEDED(hr) && buff)
    {
      clsidStr = buff;
      CoTaskMemFree(buff);
    }
    if (flashClsid == clsidStr)
    {
      std::wstring pathStr = GetFlashAxPath();
      if (pathStr.empty() == false)
      {
        LPCOLESTR path = pathStr.c_str();
        hResult = LoadTypeLib(path, pptlib);
        return hResult;
      }
    }

    hResult = fpLoadRegTypeLib(rguid, wVerMajor, wVerMinor, lcid, pptlib);
    return hResult;
  }

  HRESULT WINAPI DetourCoGetClassObject(
    _In_     REFCLSID     rclsid,
    _In_     DWORD        dwClsContext,
    _In_opt_ COSERVERINFO *pServerInfo,
    _In_     REFIID       riid,
    _Out_    LPVOID       *ppv
  )
  {
    HRESULT hResult;

    std::wstring  flashClsid = L"{D27CDB6E-AE6D-11CF-96B8-444553540000}";
    LPOLESTR buff = NULL;
    HRESULT hr=StringFromCLSID(rclsid, &buff);
    std::wstring clsidStr = L"";
    if (SUCCEEDED(hr) && buff)
    {
      clsidStr = buff;
      CoTaskMemFree(buff);
    }
    if (flashClsid == clsidStr)
    {
      std::wstring path = GetFlashAxPath();
      HMODULE hmod = LoadLibrary(path.c_str());
      if (hmod)
      {
        typedef HRESULT(__stdcall* DLLGETCLASSOBJECT)(REFCLSID, REFIID, LPVOID   *ppv);
        DLLGETCLASSOBJECT fpDllGetClassObj = NULL;
        fpDllGetClassObj = (DLLGETCLASSOBJECT)GetProcAddress(hmod, "DllGetClassObject");
        if (fpDllGetClassObj)
        {
          hResult=fpDllGetClassObj(rclsid, riid, ppv);
          return hResult;
        }
      }
    }

    hResult = fpCoGetClassObject(rclsid, dwClsContext, pServerInfo, riid, ppv);
    return hResult;
  }

  bool EnableLoadLibraryHook()
  {
    ieFilePath = GetIECorePath();
    ::SetDllDirectory(ieFilePath.c_str());
    if (MH_CreateHookApi(L"Kernel32", "LoadLibraryExW", &DetourLoadLibraryExW, (LPVOID*)&fpLoadLibExW) != MH_OK)
      return false;
    if (MH_CreateHookApi(L"ntdll.dll", "LdrLoadDll", &DetourLdrLoadDll, (LPVOID*)&fpLdrLoadDll) != MH_OK)
      return false;
    if (MH_CreateHookApi(L"ntdll.dll", "NtCreateFile", &DetourNtCreateFile, (LPVOID*)&fpNtCreateFile) != MH_OK)
      return false;

    base::win::Version ver = base::win::GetVersion();
    if (ver == base::win::VERSION_WIN8 || ver == base::win::VERSION_WIN8_1)
    {
      if (MH_CreateHookApi(L"kernelbase.dll", "RegOpenKeyExW", &DetourRegOpenKeyEx, (LPVOID*)&fpRegOpenKeyEX) != MH_OK)
        return false;
    }
    else if (ver == base::win::VERSION_XP)
    {
      if (MH_CreateHookApi(L"advapi32.dll", "RegOpenKeyExW", &DetourRegOpenKeyEx, (LPVOID*)&fpRegOpenKeyEX) != MH_OK)
        return false;
    }
    else
    {
      if (MH_CreateHookApi(L"kernel32.dll", "RegOpenKeyExW", &DetourRegOpenKeyEx, (LPVOID*)&fpRegOpenKeyEX) != MH_OK)
        return false;
    }

    if (MH_EnableHook(MH_ALL_HOOKS) != MH_OK)
      return false;
    return true;
  }

  bool EnableFlashHook()
  {
    if (MH_CreateHookApi(L"Ole32.dll", "CoGetClassObject", &DetourCoGetClassObject, (LPVOID*)&fpCoGetClassObject) != MH_OK)
      return false;
    if (MH_CreateHookApi(L"OleAut32.dll", "LoadRegTypeLib", &DetourLoadRegTypeLib, (LPVOID*)&fpLoadRegTypeLib) != MH_OK)
      return false;
    if (MH_EnableHook(MH_ALL_HOOKS) != MH_OK)
      return false;
    return true;
  }

  bool DisableLoadLibraryHook()
  {
    if (MH_DisableHook(&LoadLibraryExW) != MH_OK)
      return false;
    if (MH_RemoveHook(&LoadLibraryExW) != MH_OK)
      return false;
    cookieHookFlg = false;
    return true;
  }

  WebBrowser::WebBrowser(HWND _hWndParent, IEEventHandler* delegate, int browserEmu, bool isNew)
    :comRefCount(0),
    pWebBrowser2(NULL),
    pEvHandler(NULL),
    pDocHostHandler(NULL),
    pServerProv(NULL),
    pDevToolsHost(NULL),
    pCP(NULL),
    dwCookie(0),
    hWndParent(0),
    hWndControl(0),
    oleInPlaceObject(NULL),
    oleObject(NULL),
    pDelegate(NULL),
    IEBrowserHwnd(NULL),
    
    hMod(NULL),
    hModWininet(NULL),
    hModUrlMon(NULL),
    fpUrlSetSessionOption(NULL),
    fpUrlGetSessionOption(NULL),
    isDownloading(false),
    browserEmulation((IE::IEEmulation)browserEmu),
    oldUaString(L""),
    printEnabled(true),
    mouseRightButtonEnabled(true),
    cutCopyEnabled(true),
    saveFileEnabled(true),
    getLoginInfo_frameXpath(L""),
    getLoginInfo_nameXpath(L""),
    getLoginInfo_pwdXpath(L""),
    getLoginInfo_btnXpath(L""),
    getLoginInfo_name(L""),
    getLoginInfo_pwd(L""),
    getLoginInfo_url(L"")
  {
    InitializeCriticalSectionAndSpinCount(&criticalSection, 0x00004000);
    MH_Initialize();

    ::SetRect(&rObject, -1, -1, 0, 0);
    //hWndParent = _hWndParent;
    hWndParent = NULL;    //为了解决Chrome 49中无法创建IE窗口的问题，首先让IE以桌面为父窗口建立IE窗口，然后在DISPID_TITLECHANGE事件中，再将父窗口设成chrome中的窗口
    pDelegate = delegate;

    pThis = this;

    if (CreateBrowser(isNew) == FALSE)
    {
      return;
    }
    pEvHandler->SetChildHWND(GetControlWindow());
    pEvHandler->SetParentHwnd(_hWndParent);
    hWndParent = _hWndParent;
    //ShowWindow(GetControlWindow(), SW_SHOW);
    ShowWindow(GetControlWindow(), SW_HIDE);

    oldControlWndProc = (WNDPROC)SetWindowLong(GetControlWindow(), GWL_WNDPROC, (long)SubClassControlWndProc);
    pDevToolsHost = new DevToolsHost(pWebBrowser2);
  }

  WebBrowser::~WebBrowser()
  {
    if (pDevToolsHost)
      delete pDevToolsHost;
    if(oldControlWndProc)
      SetWindowLong(GetControlWindow(), GWL_WNDPROC, (long)oldControlWndProc);
    if (oleInPlaceObject)
    {
      oleInPlaceObject->InPlaceDeactivate();
      oleInPlaceObject->Release();
    }
    if (pCP && pEvHandler && dwCookie)
    {
      pCP->Unadvise(dwCookie);
      pCP->Release();
      delete pEvHandler;
    }
    if (pWebBrowser2)
    {
      pWebBrowser2->Stop();
      pWebBrowser2->ExecWB(OLECMDID_CLOSE, OLECMDEXECOPT_DONTPROMPTUSER, 0, 0);
      pWebBrowser2->Release();
      if (oleObject)
      {
        oleObject->DoVerb(OLEIVERB_HIDE, NULL, this, 0, hWndParent, NULL);
        oleObject->Close(OLECLOSE_NOSAVE);
        OleSetContainedObject(oleObject, FALSE);
        oleObject->SetClientSite(NULL);
        CoDisconnectObject(oleObject, 0);
        oleObject->Release();
      }
    }
    if (pDocHostHandler)
      delete pDocHostHandler;
    if (pServerProv)
      delete pServerProv;
    if (hMod)
    {
      ::FreeLibrary(hMod);
    }
    DisableLoadLibraryHook();
    DisableCookieHook();
    MH_Uninitialize();

    if (hModWininet)
    {
      FreeLibrary(hModWininet);
      hModWininet = NULL;
    }
    if (hModUrlMon)
    {
      FreeLibrary(hModUrlMon);
      hModUrlMon = NULL;
      fpUrlSetSessionOption = NULL;
    }
    DeleteCriticalSection(&criticalSection);
  }

  WebBrowser * WebBrowser::GetWebBrowser()
  {
    return pThis;
  }

  bool WebBrowser::CreateBrowser(bool isNew)
  {
    if (pServerProv == NULL)
      pServerProv = new ServiceProvider(pDelegate);

    isFlashAxHook = true;
    base::win::Version version = base::win::GetVersion();
    version = base::win::VERSION_WIN8;  //不使用自带IE8内核
    if (version >= base::win::VERSION_XP && version < base::win::VERSION_WIN8 && (int)browserEmulation<=(int)IE::EMULATION8)
    {
      EnableLoadLibraryHook();
      hMod = ::LoadLibraryExW(L"ieframe.dll", NULL, NULL);
    }
    if (isFlashAxHook)
      EnableFlashHook();

    //xp下直接加载Wininet.dll会失败，这里也无法hook，所以xp下是在LoadLibraryExW的hook函数中进行的Wininet.dll hook
    hModWininet = ::LoadLibraryExW(L"wininet.dll", NULL, NULL);
    if (hModWininet)
    {
      EnableCookieHook(L"wininet.dll");
    }

    HRESULT hr = E_FAIL;
    //hMod = ::LoadLibraryExW(L"ieframe.dll", NULL, NULL);
    if (hMod)
    {
      typedef HRESULT(__stdcall *HtmlDllGetClassObj)(REFCLSID, REFIID, LPVOID);
      HtmlDllGetClassObj GetClassObj = (HtmlDllGetClassObj)::GetProcAddress(hMod, "DllGetClassObject");
      if (GetClassObj)
      {
        IClassFactory* cf = NULL;
        hr = GetClassObj(CLSID_WebBrowser, IID_IClassFactory, &cf);
        if (cf)
          hr = cf->CreateInstance(NULL, IID_IOleObject, (void**)&oleObject);
      }
    }
    else
    {
      hr = ::OleCreate(CLSID_WebBrowser,
        IID_IOleObject, OLERENDER_DRAW, 0, this, this,
        (void**)&oleObject);
    }

    if (FAILED(hr))
    {
      return FALSE;
    }

    hr = oleObject->SetClientSite(this);
    hr = OleSetContainedObject(oleObject, TRUE);

    RECT posRect;
    ::SetRect(&posRect, 0, 0, 0, 0);
    hr = oleObject->DoVerb(OLEIVERB_INPLACEACTIVATE,
      NULL, this, -1, hWndParent, &posRect);
    if (FAILED(hr))
    {
      return FALSE;
    }

    hr = oleObject->QueryInterface(&pWebBrowser2);
    if (FAILED(hr))
    {
      return FALSE;
    }

    pServerProv->SetWebBrowser(pWebBrowser2);

    if (pDocHostHandler == NULL)
      pDocHostHandler = new DocHostUIHandler(pDelegate, this);

    //pWebBrowser2->put_Silent(VARIANT_TRUE);
    //pWebBrowser2->put_RegisterAsDropTarget(VARIANT_TRUE);
    pWebBrowser2->put_Visible(VARIANT_FALSE);
    IConnectionPointContainer* pCPC = NULL;
    hr = pWebBrowser2->QueryInterface(IID_IConnectionPointContainer, (void**)&pCPC);
    if (FAILED(hr))
    {
      return FALSE;
    }
    hr = pCPC->FindConnectionPoint(DIID_DWebBrowserEvents2, &pCP);  //事件监听
    pEvHandler = new IEEvent(pDelegate, pDocHostHandler);
    pEvHandler->SetWebBrowser(pWebBrowser2);
    hr = pCP->Advise(pEvHandler, &dwCookie);
    pCPC->Release();

    //IEHttpMonitor::Init();  //IE8下会造成访问有些页面IE内核死锁，取消http请求过程拦截。

    if(isNew == false)    //DomainAuthenticate接口在不是通过DISPID_NEWWINDOW3建立的ie时，必须要先加载一个页面后才能够初始化，应该是IE的bug
      pWebBrowser2->Navigate(L"about:blank", NULL, NULL, NULL, NULL);

    return TRUE;
  }

  RECT WebBrowser::PixelToHiMetric(const RECT& _rc)
  {
    static bool s_initialized = false;
    static int s_pixelsPerInchX, s_pixelsPerInchY;
    if (!s_initialized)
    {
      HDC hdc = ::GetDC(0);
      s_pixelsPerInchX = ::GetDeviceCaps(hdc, LOGPIXELSX);
      s_pixelsPerInchY = ::GetDeviceCaps(hdc, LOGPIXELSY);
      ::ReleaseDC(0, hdc);
      s_initialized = true;
    }

    RECT rc;
    rc.left = MulDiv(2540, _rc.left, s_pixelsPerInchX);
    rc.top = MulDiv(2540, _rc.top, s_pixelsPerInchY);
    rc.right = MulDiv(2540, _rc.right, s_pixelsPerInchX);
    rc.bottom = MulDiv(2540, _rc.bottom, s_pixelsPerInchY);
    return rc;
  }

  bool WebBrowser::DownLoadFile(const std::wstring& url, const int& status, const VARIANT& var)
  {
    bool ret = false;
    if (pDelegate)
      ret = pDelegate->OnDownLoadFile(url, status, var);

    if (status == (int)BEGINDOWNLOAD)
    {
      isDownloading = true;
    }
    return ret;
  }

  void WebBrowser::UpdateCookie(const std::wstring json)
  {
    if (pDelegate)
      pDelegate->OnUpdateCookie(json);
  }

  void WebBrowser::FinishDownload(bool isCancel)
  {
    if (pServerProv == NULL || isDownloading == false)
      return;
    if (isCancel)
    {
      IDownloadManager* pIDLMgr = NULL;
      pServerProv->QueryService(SID_SDownloadManager, IID_IDownloadManager, (void**)&pIDLMgr);
      if (pIDLMgr == NULL)
        return;
      DownLoadMgr* pDLMMgr = static_cast<DownLoadMgr*>(pIDLMgr);
      if (pDLMMgr == NULL)
        return;
      pDLMMgr->CancelDownload();
      isDownloading = false;
      pIDLMgr->Release();
    }
    isDownloading = false;
    MaybeCloseOnFinishDownload();
  }

  void WebBrowser::SetFunctionControl(const std::wstring & json)
  {
    std::string temp = base::UTF16ToUTF8(json);

    std::unique_ptr<base::Value> root = base::JSONReader().ReadToValue(temp);
    if (root.get() == NULL || root->is_dict() == false)
    {
      return;
    }
    base::DictionaryValue* rootDict = NULL;
    base::DictionaryValue* functionControl = NULL;
    root->GetAsDictionary(&rootDict);
    rootDict->GetDictionary("ieFunctionControl", &functionControl);
    if (functionControl == NULL)
      return;
    functionControl->GetBoolean("mouseRightButtonEnabled", &mouseRightButtonEnabled);
    functionControl->GetBoolean("cutCopyEnabled", &cutCopyEnabled);
    functionControl->GetBoolean("printEnabled", &printEnabled);
    functionControl->GetBoolean("saveFileEnabled", &saveFileEnabled);
    functionControl->GetString("userAgentString", &uaAddString);
    AddUAString(uaAddString);
    if (pDocHostHandler)
    {
      pDocHostHandler->EnableCutCopy(cutCopyEnabled);
      pDocHostHandler->EnableMouseRightButton(mouseRightButtonEnabled);
      pDocHostHandler->EnablePrint(printEnabled);
      pDocHostHandler->EnableSaveFile(saveFileEnabled);
    }
  }

  bool WebBrowser::AutoLoginToSite(const std::wstring& frameXPath, const std::wstring & unameXPath, const std::wstring & unameValue,
    const std::wstring & pwdXPath, const std::wstring & pwdValue, const std::wstring & loginBtnXPath)
  {
    std::wstring autoLoginId = unameXPath + L"-" + pwdXPath + L"-" + loginBtnXPath;
    auto it = autoLoginTimes.find(autoLoginId);
    if (it != autoLoginTimes.end())
    {
      DWORD lastAutoLoginTime = it->second;
      if (GetTickCount() - lastAutoLoginTime < 3000)
      {
        LOG(WARNING) << "Time diff is " << GetTickCount() - lastAutoLoginTime << ", don't auto login to site.";
        return false;
      }
    }

    LOG(WARNING) << "Do auto login to site.";

    CComPtr<IHTMLDOMNode> pStartDocDom = GetRootDomNode();
    if (frameXPath.empty() == false)
      pStartDocDom = GetSubFrameDocDomNode(pStartDocDom, frameXPath);
    if (pStartDocDom == NULL)
      return false;

    CComPtr<IHTMLDOMNode> userNameInput = GetDomNodeFromXPath(pStartDocDom, unameXPath);
    if (userNameInput == NULL)
      return false;
    CComPtr<IHTMLDOMNode> userPwdInput = GetDomNodeFromXPath(pStartDocDom, pwdXPath);
    if (userPwdInput == NULL)
      return false;
    CComPtr<IHTMLDOMNode> loginButton = GetDomNodeFromXPath(pStartDocDom, loginBtnXPath);
    if (loginButton == NULL)
      return false;


    CComPtr<IHTMLElement> htmlEle = NULL;
    userNameInput->QueryInterface(IID_IHTMLElement, (void**)&htmlEle);
    if (htmlEle)
      htmlEle->setAttribute(L"value", variant_t(unameValue.c_str()));

    userPwdInput->QueryInterface(IID_IHTMLElement, (void**)&htmlEle);
    if(htmlEle)
      htmlEle->setAttribute(L"value", variant_t(pwdValue.c_str()));

    loginButton->QueryInterface(IID_IHTMLElement, (void**)&htmlEle);
    if (htmlEle)
      htmlEle->click();

    autoLoginTimes[autoLoginId] = GetTickCount();

    return true;
  }

  void WebBrowser::NotifyCommand(const std::wstring & cmd)
  {
    if (pEvHandler == NULL)
      return;
    if (cmd == L"refresh")
      pEvHandler->NotifyRefresh(true);
    else
      pEvHandler->NotifyRefresh(false);
    ClearListenLoginXpath();
  }

  void WebBrowser::TryGetLoginInfo(CComPtr<IHTMLElement> clickElement, bool enterKeyPressed)
  {
    if (getLoginInfo_nameXpath.empty() ||
      getLoginInfo_pwdXpath.empty() || getLoginInfo_btnXpath.empty())
      return;

    //CComPtr<IHTMLDOMNode> loginBtn = GetDomNodeFromXPath(L"//*[@id=\"tableheight\"]/tbody/tr[3]/td/table/tbody/tr/td[2]/table/tbody/tr[1]/td[3]/img");  //oa login
    CComPtr<IHTMLDOMNode> pStartDocDom = GetRootDomNode();
    if (getLoginInfo_frameXpath.empty() == false)
      pStartDocDom = GetSubFrameDocDomNode(pStartDocDom, getLoginInfo_frameXpath);
    if (pStartDocDom == NULL)
      return;
    CComPtr<IHTMLDOMNode> loginBtn = GetDomNodeFromXPath(pStartDocDom, getLoginInfo_btnXpath);
    CComPtr<IHTMLDOMNode> uName = GetDomNodeFromXPath(pStartDocDom, getLoginInfo_nameXpath);
    CComPtr<IHTMLDOMNode> password = GetDomNodeFromXPath(pStartDocDom, getLoginInfo_pwdXpath);

    if (loginBtn == NULL || uName == NULL || password == NULL)
      return;

    CComPtr<IHTMLUniqueName> pUniqueName_login;
    loginBtn->QueryInterface(IID_IHTMLUniqueName, (void**)&pUniqueName_login);
    long num_Login = 0;
    pUniqueName_login->get_uniqueNumber(&num_Login);

    CComPtr<IHTMLUniqueName> pUniqueName_Pwd;
    password->QueryInterface(IID_IHTMLUniqueName, (void**)&pUniqueName_Pwd);
    long num_Pwd = 0;
    pUniqueName_Pwd->get_uniqueNumber(&num_Pwd);

    long num_Click = -1;
    if (clickElement)
    {
      CComPtr<IHTMLUniqueName> pUniqueName_Click;
      clickElement->QueryInterface(IID_IHTMLUniqueName, (void**)&pUniqueName_Click);
      pUniqueName_Click->get_uniqueNumber(&num_Click);
    }

    if (num_Login == num_Click || enterKeyPressed)
    {
      CComPtr<IHTMLElement> nameElement;
      uName->QueryInterface(IID_IHTMLElement, (void**)&nameElement);
      variant_t name = L"";
      nameElement->getAttribute(L"value", 0, name.GetAddress());

      CComPtr<IHTMLElement> pwdElement;
      password->QueryInterface(IID_IHTMLElement, (void**)&pwdElement);
      variant_t pwd = L"";
      pwdElement->getAttribute(L"value", 0, pwd.GetAddress());

      if (name.vt != VT_BSTR || name.bstrVal == NULL ||
        pwd.vt != VT_BSTR || pwd.bstrVal == NULL)
        return;
      getLoginInfo_name = name.bstrVal;
      getLoginInfo_pwd = pwd.bstrVal;
    }
  }

  void WebBrowser::SetListenLoginXPath(const std::wstring& url, const std::wstring& frameXPath, const std::wstring & nameXpath,
    const std::wstring & pwdXpath, const std::wstring & loginBtnXpath)
  {
    getLoginInfo_url = url;
    getLoginInfo_frameXpath = frameXPath;
    getLoginInfo_nameXpath = nameXpath;
    getLoginInfo_pwdXpath = pwdXpath;
    getLoginInfo_btnXpath = loginBtnXpath;
    if (getLoginInfo_frameXpath.empty() == false)
    {
      CComPtr<IHTMLDOMNode> pDocDom = GetRootDomNode();
      pDocDom = GetSubFrameDocDomNode(pDocDom, getLoginInfo_frameXpath);
      if (pDocDom)
      {
        CComPtr<IHTMLElement> pEle;
        pDocDom->QueryInterface(IID_IHTMLElement, (void**)&pEle);
        if (pEle == NULL)
          return;
        CComPtr<IDispatch> pDisp;
        pEle->get_document(&pDisp);
        if (pDisp == NULL)
          return;
        CComPtr<IHTMLDocument2> pDoc2;
        pDisp->QueryInterface(IID_IHTMLDocument2, (void**)&pDoc2);
        if (pDoc2 && pEvHandler)
          pEvHandler->ListenDocmentEvent(pDoc2);
      }
    }
  }

  void WebBrowser::ConfirmLoginSuccess()
  {
    if (getLoginInfo_name.empty() || getLoginInfo_pwd.empty() ||
      getLoginInfo_nameXpath.empty() || getLoginInfo_pwdXpath.empty() ||
      getLoginInfo_btnXpath.empty() || pDelegate ==NULL)
      return;

    CComPtr<IHTMLDOMNode> pStartDocDom = GetRootDomNode();
    if (getLoginInfo_frameXpath.empty() == false)
      pStartDocDom = GetSubFrameDocDomNode(pStartDocDom, getLoginInfo_frameXpath);

    CComPtr<IHTMLDOMNode> loginBtn = GetDomNodeFromXPath(pStartDocDom, getLoginInfo_btnXpath);
    if (loginBtn)
      return;
    CComPtr<IHTMLDOMNode> uName = GetDomNodeFromXPath(pStartDocDom, getLoginInfo_nameXpath);
    if (uName)
      return;
    CComPtr<IHTMLDOMNode> password = GetDomNodeFromXPath(pStartDocDom, getLoginInfo_pwdXpath);
    if (password)
      return;

    pDelegate->OnGetLoginInfo(getLoginInfo_url, getLoginInfo_name, getLoginInfo_pwd);
    ClearListenLoginXpath();
  }

  bool WebBrowser::IsDevToolsOpened()
  {
    if (pDevToolsHost == NULL)
      return false;
    return pDevToolsHost->IsOpened();
  }

  void WebBrowser::ShowIEDevTools(bool show)
  {
    if (pDevToolsHost == NULL)
      return;
    base::win::Version ver= base::win::GetVersion();
    if (ver >= base::win::VERSION_WIN10)
    {
      wchar_t sysDir[_MAX_PATH + 1] = {0};
      if (SHGetSpecialFolderPath(NULL, sysDir, CSIDL_SYSTEMX86, FALSE) == TRUE)
      {
        std::wstring path = std::wstring(sysDir) + L"\\F12";
        ShellExecute(NULL, L"open", (path+L"\\F12Chooser.exe").c_str(), L"", path.c_str(), SW_SHOW);
        return;
      }
    }
    if (show)
      pDevToolsHost->ShowIEDevTools();
    else
      pDevToolsHost->CloseIEDevTools();
  }

  void WebBrowser::SetRect(const RECT& _rc)
  {
    rObject = _rc;
    if(oleObject)
    {
      RECT hiMetricRect = PixelToHiMetric(rObject);
      SIZEL sz;
      sz.cx = hiMetricRect.right - hiMetricRect.left;
      sz.cy = hiMetricRect.bottom - hiMetricRect.top;
      oleObject->SetExtent(DVASPECT_CONTENT, &sz);
    }

    if (oleInPlaceObject != 0)
    {
      oleInPlaceObject->SetObjectRects(&rObject, &rObject);
    }
  }

  // ----- Control methods -----

  void WebBrowser::GoBack()
  {
    this->pWebBrowser2->GoBack();
  }

  void WebBrowser::GoForward()
  {
    this->pWebBrowser2->GoForward();
  }

  void WebBrowser::Refresh()
  {
    this->pWebBrowser2->Refresh();
  }

  void WebBrowser::Navigate(wstring szUrl)
  {
    bstr_t url(szUrl.c_str());
    variant_t flags;// (0x02u); //navNoHistory
    //this->pWebBrowser2->Navigate(url, &flags, 0, 0, 0);

    variant_t url2(szUrl.c_str());

    //variant_t ua(L"User-Agent: Mozilla/5.0 (Windows NT 6.1; WOW64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/68.0.2840.9999 Safari/537.36");
    pWebBrowser2->Navigate2(&url2, &flags, 0, 0, 0);
  }

  HRESULT WebBrowser::ExcuteJavaScript(std::wstring& funName, std::vector<variant_t>& params, variant_t* result)
  {
    if (pEvHandler)
    {
      return pEvHandler->ExcuteJs(funName, params, result);
    }
    return E_FAIL;
  }

  HRESULT WebBrowser::ExcuteJavaScript(std::wstring& code, variant_t * result)
  {
    if (pEvHandler)
    {
      return pEvHandler->ExcuteJs(code, result);
    }
    return E_FAIL;
  }

  int WebBrowser::GetDocumentMode()
  {
    if (pEvHandler)
    {
      return pEvHandler->GetDocMode();
    }
    return 0;
  }

  bool WebBrowser::SetDocumentMode(IEDocumentMode mode)
  {
    if (pEvHandler)
    {
      return pEvHandler->SetDocMode(mode);
    }
    return false;
  }

  bool WebBrowser::SetBrowserCookie(const std::wstring & json, const int & flag)
  {
    std::string temp = base::UTF16ToUTF8(json);

    std::unique_ptr<base::Value> root = base::JSONReader().ReadToValue(temp);
    if (root.get() == NULL || root->is_dict() == false)
    {
      return false;
    }
    base::DictionaryValue* rootDict = NULL;
    root->GetAsDictionary(&rootDict);
    base::ListValue* cookies = NULL;
    rootDict->GetList("allCookies", &cookies);
    base::ListValue::iterator iter = cookies->begin();
    for (; iter != cookies->end(); iter++)
    {
      base::DictionaryValue* cookie=NULL;
      (*iter).GetAsDictionary(&cookie);
      if(cookie==NULL)
        continue;
      std::wstring url = L"";
      cookie->GetString("url", &url);
      std::wstring buff = L"";
      cookie->GetString("cookie",&buff);
      int opFlag = 0;
      size_t findPos = buff.find(L"HTTPONLY");
      if (findPos != std::wstring::npos)
      {
        opFlag = opFlag | INTERNET_COOKIE_HTTPONLY;
      }
      findPos = buff.find(L"; INTERNET_COOKIE_THIRD_PARTY");
      if (findPos != std::wstring::npos)
      {
        opFlag = opFlag | INTERNET_COOKIE_THIRD_PARTY;
        buff = buff.substr(0, buff.size() - wcslen(L"; INTERNET_COOKIE_THIRD_PARTY"));
      }
      opFlag = opFlag | INTERNET_COOKIE_EVALUATE_P3P;
      std::wstring p3p =
        L"CP=ALL IND DSP COR ADM CONo CUR CUSo IVAo IVDo PSA PSD TAI TELo OUR SAMo CNT COM INT NAV ONL PHY PRE PUR UNI";

      EnterCriticalSection(&criticalSection);
      setCookieSelf = true;
      DWORD ret = DetourInternetSetCookieExW(url.c_str(), NULL, buff.c_str(), opFlag, (DWORD_PTR)(LPCTSTR)p3p.c_str());
      if (ret != COOKIE_STATE_ACCEPT && ret != COOKIE_STATE_LEASH
        && ret != COOKIE_STATE_DOWNGRADE)
        LOG(WARNING) << "SetCookie Failed! \r\n url:" << url.c_str() << "\r\n cookies:" << buff.c_str();
      if(ret== COOKIE_STATE_LEASH)
        LOG(WARNING) << "SetCookie Succed! But return value is COOKIE_STATE_LEASH \r\n url:" << url.c_str() << "\r\n cookies:" << buff.c_str();
      if (ret == COOKIE_STATE_DOWNGRADE)
        LOG(WARNING) << "SetCookie Succed! But return value is COOKIE_STATE_DOWNGRADE \r\n url:" << url.c_str() << "\r\n cookies:" << buff.c_str();
      setCookieSelf = false;
      LeaveCriticalSection(&criticalSection);
    }
    return true;
  }

  std::vector<std::wstring> WebBrowser::SpliteCookieKeyVal(const std::wstring & cookie)
  {
    std::vector<std::wstring>vec;
    wistringstream f(cookie);
    std::wstring s = L"";
    wchar_t buff[10240] = { 0 };
    while (f.getline(buff, 10240, L';')) {
      vec.push_back(buff);
      memset(buff, 0, sizeof(wchar_t) * 10240);
    }
    return vec;
  }

  HRESULT WebBrowser::SetUserAgent(const std::wstring & ua)
  {
    if (fpUrlSetSessionOption == NULL)
    {
      hModUrlMon = LoadLibraryExW(L"urlmon.dll", NULL, NULL);
      if (hModUrlMon == NULL)
        return E_FAIL;
      fpUrlSetSessionOption = (UrlSetSessionOptionFun)GetProcAddress(hModUrlMon, "UrlMkSetSessionOption");
    }
    if (fpUrlSetSessionOption == NULL || ua.empty())
      return E_FAIL;
    std::string buffer = base::UTF16ToASCII(ua);
    HRESULT hr = fpUrlSetSessionOption(URLMON_OPTION_USERAGENT, (LPVOID)buffer.c_str(), buffer.size(), 0);
    return hr;
  }

  LRESULT WebBrowser::SubClassControlWndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
  {
    if (msg == WM_COOKIEUPDATED && wParam)
    {
      std::wstring* jsonStr = (std::wstring*)wParam;
      WebBrowser::GetWebBrowser()->UpdateCookie(*jsonStr);
      delete jsonStr; //delete after used
      return TRUE;
    }
    else if (msg == WM_DESTROY)
    {
      if (WebBrowser::GetWebBrowser())
        WebBrowser::GetWebBrowser()->pEvHandler->ResetDocHostUIHandler();
      LRESULT ret= CallWindowProc(oldControlWndProc, hWnd, msg, wParam, lParam);
      if (WebBrowser::GetWebBrowser())
        WebBrowser::GetWebBrowser()->pDelegate->OnBrowserClosing();
      return ret;
    }
    return CallWindowProc(oldControlWndProc, hWnd, msg, wParam, lParam);
  }

  bool WebBrowser::PreTranslateMsg(LPMSG msg)
  {
    if (msg == NULL)
      return false;
    if (msg->message == WM_MOUSEWHEEL)
    {
      int fwKeys = GET_KEYSTATE_WPARAM(msg->wParam);
      if (fwKeys == MK_CONTROL)
      {
        int x= GET_X_LPARAM(msg->lParam);
        int y= GET_Y_LPARAM(msg->lParam);
        int zDelta = GET_WHEEL_DELTA_WPARAM(msg->wParam);
        int modifiers = 1024;
        // kControllkey by Webb
        modifiers=modifiers | blink::WebInputEvent::kControlKey;
        if (WebBrowser::GetWebBrowser())
        {
          WebBrowser::GetWebBrowser()->pDelegate->OnRequestAcceleratorFromMouseWheel(zDelta, x, y, modifiers);
          return true;
        }
      }
    }
    else if (msg->message == WM_KEYDOWN || msg->message == WM_KEYUP)
    {
      bool bTranslate = true;
      WPARAM wParam = msg->wParam;

      if (wParam == VK_BACK
        //|| wParam == VK_F5
        || wParam == VK_ESCAPE)
      {
        bTranslate = false;
        //if (wParam == VK_F5 && pThis)
        //{
        //  HWND hMainHwnd = GetMainFrameHwnd(pThis->hWndParent);
        //  PostMessage(hMainHwnd, msg, wParam, lParam);
        //  return true;
        //}
      }
      else if ((wParam == VK_F12 ||
        wParam == VK_F11 ||
        wParam== VK_F5 ||
        wParam=='I') &&
        msg->message==WM_KEYDOWN)
      {
        bool hit = true;
        int modifiers = 1024;
        if (wParam == 'I')
        {
          short stateCtrl = GetAsyncKeyState(VK_CONTROL);
          short stateShift = GetAsyncKeyState(VK_SHIFT);
          if ((stateCtrl & 0x8000) ==0 || (stateShift & 0x8000)==0)
          {
            hit = false;
          }
          else
            modifiers = modifiers | blink::WebInputEvent::kControlKey | blink::WebInputEvent::kShiftKey;
        }
        if (hit && WebBrowser::GetWebBrowser())
        {
          WebBrowser::GetWebBrowser()->pDelegate->OnRequestAcceleratorFromKeyboard(wParam, modifiers);
          return true;
        }
      }
      else if ((wParam == VK_ADD || wParam == VK_SUBTRACT) &&
        msg->message == WM_KEYDOWN)
      {
        int modifiers = 1024;
        short stateCtrl = GetAsyncKeyState(VK_CONTROL);
        if (stateCtrl & 0x8000)
        {
          modifiers = modifiers | blink::WebInputEvent::kControlKey;
          if (WebBrowser::GetWebBrowser())
          {
            WebBrowser::GetWebBrowser()->pDelegate->OnRequestAcceleratorFromKeyboard(wParam, modifiers);
            return true;
          }
        }
      }
      else if (wParam == 'C' || wParam == 'X')
      { //屏蔽剪切复制快捷键
        short state = GetAsyncKeyState(VK_CONTROL);
        if ((state & 0x8000) && pThis && pThis->cutCopyEnabled == false)
        {
          return true;
        }
      }
      else if (wParam == 'P')
      { //屏蔽打印快捷键
        short state = GetAsyncKeyState(VK_CONTROL);
        if ((state & 0x8000) && pThis && pThis->printEnabled == false)
        {
          return true;
        }
      }
      else if (wParam == 'O')
      { //屏蔽打开文件快捷键
        short state = GetAsyncKeyState(VK_CONTROL);
        if (state & 0x8000)
          return true;
      }
      if (pIOIPAO && bTranslate)
      {
        HRESULT hr = pIOIPAO->TranslateAccelerator(msg);
        if (hr == S_OK)
        {
          if (wParam == VK_RETURN && WebBrowser::GetWebBrowser())
            WebBrowser::GetWebBrowser()->pDelegate->OnClickElement(NULL, true);
          return true;
        }
      }
    }
    return false;
  }

  LRESULT CALLBACK WebBrowser::MessageProc(int code, WPARAM wParam, LPARAM lParam)
  {
    if(code<0)
      return CallNextHookEx(hNextHook, code, wParam, lParam);
    LPMSG msg = (LPMSG)lParam;
    if (PreTranslateMsg(msg))
      return TRUE;
    return CallNextHookEx(hNextHook, code, wParam, lParam);
  }

  bool WebBrowser::EnableSubClass(HWND hWnd)
  {
    if (pWebBrowser2 == NULL
          || IsWindow(hWnd)==FALSE)
      return false;

    pWebBrowser2->QueryInterface(IID_IOleInPlaceActiveObject, (void**)&pIOIPAO);
    if (pIOIPAO == NULL)
      return false;

    hNextHook = SetWindowsHook(WH_MSGFILTER, MessageProc);

    IEBrowserHwnd = hWnd;
    oldWndProc = (WNDPROC)SetWindowLong(hWnd, GWL_WNDPROC, (long)SubClassWndProc);
    return true;
  }

  void WebBrowser::DisableSubClass(HWND hWnd)
  {
    if (IsWindow(hWnd) == FALSE ||
      oldWndProc==NULL ||
      hNextHook==NULL)
      return;
    SetWindowLong(hWnd, GWL_WNDPROC, (long)oldWndProc);
    UnhookWindowsHookEx(hNextHook);
    pThis = NULL;
    if (pIOIPAO)
      pIOIPAO->Release();
  }

  static BOOL parentIsForegroundWindow(HWND hwnd)
  {
    HWND hForeground = ::GetForegroundWindow();
    HWND hParentWnd = ::GetParent(hwnd);

    while (hParentWnd)
    {
      if (hForeground == hParentWnd)
        return TRUE;
      hParentWnd = ::GetParent(hParentWnd);
    }

    return FALSE;
  }

  void WebBrowser::MaybeCloseOnFinishDownload()
  {
    if (isDownloading == true || pWebBrowser2==NULL)
      return;
    IHTMLDocument2* pDoc = NULL;
    pWebBrowser2->get_Document((IDispatch**)&pDoc);
    if (pDoc == NULL)
      return;
    IHTMLElement * pBody=NULL;
    pDoc->get_body(&pBody);
    if (pBody == NULL)
      return;
    BSTR buff=NULL;
    pBody->get_innerHTML(&buff);
    pBody->Release();
    pDoc->Release();
    bstr_t html(buff,false);
    if (html.length() == 0 && pEvHandler)
    {
      pDelegate->OnBrowserClosing();
      //::PostQuitMessage(0);
    }
  }

  LRESULT WebBrowser::SubClassWndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
  {
    if (msg == WM_INITMENUPOPUP)
    {
      return 0;
    }
    else if (msg == WM_MOUSEACTIVATE)
    {
      if (!parentIsForegroundWindow(hWnd))
      {
        HWND hwnd = GetParent(hWnd);
        hwnd = GetParent(hwnd);
        hwnd = GetParent(hwnd);
        hwnd = GetParent(hwnd);
        ::PostMessage(hwnd, WM_IE_MOUSEACTIVATE, NULL, NULL);
      }
    }
    return CallWindowProc(oldWndProc, hWnd, msg, wParam, lParam);
  }

  HWND WebBrowser::GetMainFrameHwnd(HWND hWnd)
  {
    if (IsWindow(hWnd) == FALSE)
      return NULL;
    HWND hMainFrameWnd = NULL;
    while (hWnd)
    {
      hWnd = GetParent(hWnd);
      if(hWnd && IsWindow(hWnd))
        hMainFrameWnd = hWnd;
    }
    DWORD err = 0;
    err=GetLastError();
    return hMainFrameWnd;
  }

  void WebBrowser::AddUAString(const std::wstring & uaStr)
  {
    if (oldUaString.empty())
    {
      if (fpUrlGetSessionOption == NULL)
      {
        hModUrlMon = LoadLibraryExW(L"urlmon.dll", NULL, NULL);
        if (hModUrlMon == NULL)
          return;
        fpUrlGetSessionOption = (UrlMkGetSessionOptionFun)GetProcAddress(hModUrlMon, "UrlMkGetSessionOption");
      }
      if (fpUrlGetSessionOption == NULL || uaStr.empty())
        return;
      HRESULT hr = S_OK;
      DWORD len = 0;
      hr = UrlMkGetSessionOption(URLMON_OPTION_USERAGENT, NULL, 0, &len, 0);
      if (len <= 0)
        return;
      int size = len + 1;
      char* uaBuffer = new char[size];
      memset(uaBuffer, 0, sizeof(size));
      len = 0;
      hr = UrlMkGetSessionOption(URLMON_OPTION_USERAGENT, uaBuffer, size - 1, &len, 0);
      if (len == 0)
      {
        delete[] uaBuffer;
        return;
      }
      oldUaString = base::ASCIIToUTF16(std::string(uaBuffer));
      delete[] uaBuffer;
    }
    std::wstring ua = oldUaString;
    ua=ua +L" " + uaStr;
    SetUserAgent(ua);
  }

  CComPtr<IHTMLDOMNode> WebBrowser::GetRootDomNode()
  {

    CComPtr<IHTMLDOMNode> pRootDomNode = NULL;
    if (pWebBrowser2 == NULL)
      return pRootDomNode;

    CComPtr<IDispatch> docIDisp = NULL;
    pWebBrowser2->get_Document(&docIDisp);
    if (docIDisp == NULL)
      return pRootDomNode;

    CComPtr<IHTMLDocument> pHtmlDoc = NULL;
    docIDisp->QueryInterface(IID_IHTMLDocument, (void**)&pHtmlDoc);
    if (pHtmlDoc == NULL)
      return pRootDomNode;

    CComPtr<IHTMLDocument3> pHtmlDoc3;
    pHtmlDoc->QueryInterface(IID_IHTMLDocument3, (void**)&pHtmlDoc3);
    if (pHtmlDoc3 == NULL)
      return pRootDomNode;

    CComPtr<IHTMLElement> pRootElement;
    pHtmlDoc3->get_documentElement(&pRootElement);
    if (pRootElement == NULL)
      return pRootDomNode;

    pRootElement->QueryInterface(IID_IHTMLDOMNode, (void**)&pRootDomNode);
    return pRootDomNode;
  }

  CComPtr<IHTMLDOMNode> WebBrowser::GetSubFrameDocDomNode(
    CComPtr<IHTMLDOMNode> pParentDocDom, const std::wstring & frameXPath)
  {
    CComPtr<IHTMLDOMNode> pSubDocDomNode = NULL;

    if (frameXPath.empty() || pParentDocDom == NULL)
      return pSubDocDomNode;

    CComPtr<IHTMLDOMNode> pFrameNode = GetDomNodeFromXPath(pParentDocDom, frameXPath);
    if (pFrameNode == NULL)
      return pSubDocDomNode;

    CComPtr<IHTMLElement> pEle;
    pFrameNode->QueryInterface(IID_IHTMLElement, (void**)&pEle);
    if (pEle == NULL)
      return pSubDocDomNode;

    CComPtr<IHTMLIFrameElement> pIFrameEle; //iframe
    CComPtr<IHTMLFrameElement> pFrameEle;  //frame
    pEle->QueryInterface(IID_IHTMLIFrameElement, (void**)&pIFrameEle);
    if (pIFrameEle == NULL)
      pEle->QueryInterface(IID_IHTMLFrameElement, (void**)&pFrameEle);
    if(pIFrameEle==NULL && pFrameEle==NULL)
      return pSubDocDomNode;

    CComPtr<IHTMLFrameBase2> pFrameBase2;
    if(pIFrameEle)
      pIFrameEle->QueryInterface(IID_IHTMLFrameBase2, (void**)&pFrameBase2);
    else if(pFrameEle)
      pFrameEle->QueryInterface(IID_IHTMLFrameBase2, (void**)&pFrameBase2);
    if (pFrameBase2 == NULL)
      return pSubDocDomNode;
    CComPtr<IHTMLWindow2> pWnd2;
    pFrameBase2->get_contentWindow(&pWnd2);
    if (pWnd2 == NULL)
      return pSubDocDomNode;
    CComPtr<IHTMLDocument2> pDoc2;
    pWnd2->get_document(&pDoc2);
    if (pDoc2 == NULL)
      return pSubDocDomNode;
    CComPtr<IHTMLElement> pBody;
    pDoc2->get_body(&pBody);
    bstr_t html;
    if (pBody)
      pBody->get_innerHTML(html.GetAddress());

    CComPtr<IHTMLDocument3> pHtmlDoc3;
    pDoc2->QueryInterface(IID_IHTMLDocument3, (void**)&pHtmlDoc3);
    if (pHtmlDoc3 == NULL)
      return pSubDocDomNode;

    CComPtr<IHTMLElement> pSubDocElement;
    pHtmlDoc3->get_documentElement(&pSubDocElement);
    if (pSubDocElement == NULL)
      return pSubDocDomNode;

    pSubDocElement->QueryInterface(IID_IHTMLDOMNode, (void**)&pSubDocDomNode);
    return pSubDocDomNode;
  }

  CComPtr<IHTMLDOMNode> WebBrowser::GetDomNodeFromXPath(CComPtr<IHTMLDOMNode> pDocDomNode, const std::wstring & xpath)
  {
    CComPtr<IHTMLDOMNode> pFindDomNode = NULL;
    if (pWebBrowser2 == NULL || xpath.empty() || pDocDomNode == NULL)
      return pFindDomNode;

    IE::XPathParse parse;
    parse.Parse(xpath);
    parse.BeginMatch();

    //  FILE* logFile = NULL;
    //  logFile = _wfopen(L"F:\\html.txt", L"wb+");

    stack<CComPtr<IHTMLDOMNode>> domStack;
    domStack.push(pDocDomNode);
    while (domStack.empty()==false)
    {
      CComPtr<IHTMLDOMNode> currNode = domStack.top();
      domStack.pop();
      CComBSTR nodeName;
      currNode->get_nodeName(&nodeName);
      //fwrite(nodeName, 1, nodeName.Length() * sizeof(wchar_t), logFile);
      //fwrite(L"\r\n", 1, 4, logFile);

      IE::MATCHRESULT matchRet=parse.StepMatch(currNode);
      if(matchRet==IE::RESULT_NOT_MATCH)
        continue;
      if (matchRet == IE::RESULT_ALL_NODE_MATCH)
      {
        pFindDomNode = currNode;
        break;
        //CComPtr<IHTMLElement> htmlEle = NULL;
        //currNode->QueryInterface(IID_IHTMLElement, (void**)&htmlEle);
        //if (htmlEle)
        //{
        //  htmlEle->setAttribute(L"value", variant_t(L"chjyTest"));
        //  break;
        //}
      }

      VARIANT_BOOL isHasChild = VARIANT_FALSE;
      currNode->hasChildNodes(&isHasChild);
      if (isHasChild == VARIANT_TRUE)
      {
        CComPtr<IHTMLDOMNode> childNode;
        currNode->get_lastChild(&childNode);

        if (childNode == NULL)
          return pFindDomNode;
        long type = 0;
        childNode->get_nodeType(&type);
        if (type == 1)
        {
          domStack.push(childNode);
          childNode->get_nodeName(&nodeName);
        }
        CComPtr<IHTMLDOMNode> siblingNode=childNode;
        while (siblingNode !=NULL)
        {
          siblingNode->get_previousSibling(&siblingNode);
          if (siblingNode==NULL)
            break;
          type = 0;
          siblingNode->get_nodeType(&type);
          if (type == 1)
          {
            domStack.push(siblingNode);
            siblingNode->get_nodeName(&nodeName);
          }
        }
      }
    }
    //  fclose(logFile);
    return pFindDomNode;
  }

  void WebBrowser::ClearListenLoginXpath()
  {
    getLoginInfo_frameXpath = L"";
    getLoginInfo_url = L"";
    getLoginInfo_nameXpath = L"";
    getLoginInfo_pwdXpath = L"";
    getLoginInfo_btnXpath = L"";
    getLoginInfo_name = L"";
    getLoginInfo_pwd = L"";
  }

  IDispatch * WebBrowser::GetIeDispatch()
  {
    IDispatch* pDisp = NULL;
    if (pWebBrowser2)
    {
      pWebBrowser2->QueryInterface(IID_IDispatch, (void**)&pDisp);
    }
    return pDisp;
  }

  void WebBrowser::Show()
  {
    if(::IsWindow(GetControlWindow()))
    {
      ::ShowWindow(GetControlWindow(), SW_SHOW);
    }
  }

  void WebBrowser::Hide()
  {
    if (::IsWindow(GetControlWindow()))
    {
      ::ShowWindow(GetControlWindow(), SW_HIDE);
    }
  }

  // ----- IUnknown -----

  HRESULT STDMETHODCALLTYPE WebBrowser::QueryInterface(REFIID riid,
    void**ppvObject)
  {
    if (riid == __uuidof(IUnknown))
    {
      (*ppvObject) = (IUnknown*)(IOleClientSite*)(this);
    }
    else if (riid == __uuidof(IOleClientSite))
    {
      (*ppvObject) = static_cast<IOleClientSite*>(this);
    }
    else if (riid == __uuidof(IOleInPlaceSite))
    {
      (*ppvObject) = static_cast<IOleInPlaceSite*>(this);
    }
    //不能在这里提供自己实现的IDocHostUIHandler接口，因为这样无法获取IE内核建立的的IDocHostUIHandler::GetExternal接口。
    //而自己实现的IDocHostUIHandler::GetExternal接口会导致和插件扩展的window.external js函数冲突的问题，
    //所以要使用IE内核的IDocHostUIHandler::GetExternal接口。
    //替换成自己实现的IDocHostUIHandler接口的时机在DISPID_DOWNLOADCOMPLETE时间中实现
    //else if (riid == __uuidof(IDocHostUIHandler))
    //{
    //  (*ppvObject) = static_cast<IDocHostUIHandler*>(pDocHostHandler);
    //}
    else if (riid == __uuidof(IOleCommandTarget))
    {
      (*ppvObject) = static_cast<IOleCommandTarget*>(pDocHostHandler);
    }
    else if (riid == __uuidof(IServiceProvider))
    {
      (*ppvObject) = static_cast<IServiceProvider*>(pServerProv);
    }
    else
    {
      return E_NOINTERFACE;
    }

    AddRef();
    return S_OK;
  }

  ULONG STDMETHODCALLTYPE WebBrowser::AddRef(void)
  {
    InterlockedIncrement(&comRefCount);
    return comRefCount;
  }

  ULONG STDMETHODCALLTYPE WebBrowser::Release(void)
  {
    InterlockedDecrement(&comRefCount);
    return comRefCount;
  }

  // ---------- IOleWindow ----------

  HRESULT STDMETHODCALLTYPE WebBrowser::GetWindow(
    __RPC__deref_out_opt HWND *phwnd)
  {
    (*phwnd) = hWndParent;
    return S_OK;
  }

  HRESULT STDMETHODCALLTYPE WebBrowser::ContextSensitiveHelp(
    BOOL fEnterMode)
  {
    return E_NOTIMPL;
  }

  // ---------- IOleInPlaceSite ----------

  HRESULT STDMETHODCALLTYPE WebBrowser::CanInPlaceActivate(void)
  {
    return S_OK;
  }

  HRESULT STDMETHODCALLTYPE WebBrowser::OnInPlaceActivate(void)
  {
    OleLockRunning(oleObject, TRUE, FALSE);
    oleObject->QueryInterface(&oleInPlaceObject);
    oleInPlaceObject->SetObjectRects(&rObject, &rObject);
    return S_OK;

  }

  HRESULT STDMETHODCALLTYPE WebBrowser::OnUIActivate(void)
  {
    return S_OK;
  }

  HRESULT STDMETHODCALLTYPE WebBrowser::GetWindowContext(
    __RPC__deref_out_opt IOleInPlaceFrame **ppFrame,
    __RPC__deref_out_opt IOleInPlaceUIWindow **ppDoc,
    __RPC__out LPRECT lprcPosRect,
    __RPC__out LPRECT lprcClipRect,
    __RPC__inout LPOLEINPLACEFRAMEINFO lpFrameInfo)
  {
    HWND hwnd = hWndParent;

    (*ppFrame) = NULL;
    (*ppDoc) = NULL;
    (*lprcPosRect).left = rObject.left;
    (*lprcPosRect).top = rObject.top;
    (*lprcPosRect).right = rObject.right;
    (*lprcPosRect).bottom = rObject.bottom;
    *lprcClipRect = *lprcPosRect;

    lpFrameInfo->fMDIApp = false;
    lpFrameInfo->hwndFrame = hwnd;
    lpFrameInfo->haccel = NULL;
    lpFrameInfo->cAccelEntries = 0;

    return S_OK;
  }

  HRESULT STDMETHODCALLTYPE WebBrowser::Scroll(
    SIZE scrollExtant)
  {
    return E_NOTIMPL;
  }

  HRESULT STDMETHODCALLTYPE WebBrowser::OnUIDeactivate(
    BOOL fUndoable)
  {
    return S_OK;
  }

  HWND WebBrowser::GetControlWindow()
  {
    if (hWndControl != 0)
      return hWndControl;

    if (oleInPlaceObject == 0)
      return 0;

    oleInPlaceObject->GetWindow(&hWndControl);
    return hWndControl;
  }

  HRESULT STDMETHODCALLTYPE WebBrowser::OnInPlaceDeactivate(void)
  {
    hWndControl = 0;
    //oleInPlaceObject = 0;

    return S_OK;
  }

  HRESULT STDMETHODCALLTYPE WebBrowser::DiscardUndoState(void)
  {
    return E_NOTIMPL;
  }

  HRESULT STDMETHODCALLTYPE WebBrowser::DeactivateAndUndo(void)
  {
    return E_NOTIMPL;
  }

  HRESULT STDMETHODCALLTYPE WebBrowser::OnPosRectChange(
    __RPC__in LPCRECT lprcPosRect)
  {
    return S_OK;
  }

  // ---------- IOleClientSite ----------

  HRESULT STDMETHODCALLTYPE WebBrowser::SaveObject(void)
  {
    return E_NOTIMPL;
  }

  HRESULT STDMETHODCALLTYPE WebBrowser::GetMoniker(
    DWORD dwAssign,
    DWORD dwWhichMoniker,
    __RPC__deref_out_opt IMoniker **ppmk)
  {
    if ((dwAssign == OLEGETMONIKER_ONLYIFTHERE) &&
      (dwWhichMoniker == OLEWHICHMK_CONTAINER))
      return E_FAIL;

    return E_NOTIMPL;
  }

  HRESULT STDMETHODCALLTYPE WebBrowser::GetContainer(
    __RPC__deref_out_opt IOleContainer **ppContainer)
  {
    return E_NOINTERFACE;
  }

  HRESULT STDMETHODCALLTYPE WebBrowser::ShowObject(void)
  {
    return S_OK;
  }

  HRESULT STDMETHODCALLTYPE WebBrowser::OnShowWindow(
    BOOL fShow)
  {
    return S_OK;
  }

  HRESULT STDMETHODCALLTYPE WebBrowser::RequestNewObjectLayout(void)
  {
    return E_NOTIMPL;
  }

  // ----- IStorage -----

  HRESULT STDMETHODCALLTYPE WebBrowser::CreateStream(
    __RPC__in_string const OLECHAR *pwcsName,
    DWORD grfMode,
    DWORD reserved1,
    DWORD reserved2,
    __RPC__deref_out_opt IStream **ppstm)
  {
    return E_NOTIMPL;
  }

  HRESULT STDMETHODCALLTYPE WebBrowser::OpenStream(
    const OLECHAR *pwcsName,
    void *reserved1,
    DWORD grfMode,
    DWORD reserved2,
    IStream **ppstm)
  {
    return E_NOTIMPL;
  }

  HRESULT STDMETHODCALLTYPE WebBrowser::CreateStorage(
    __RPC__in_string const OLECHAR *pwcsName,
    DWORD grfMode,
    DWORD reserved1,
    DWORD reserved2,
    __RPC__deref_out_opt IStorage **ppstg)
  {
    return E_NOTIMPL;
  }

  HRESULT STDMETHODCALLTYPE WebBrowser::OpenStorage(
    __RPC__in_opt_string const OLECHAR *pwcsName,
    __RPC__in_opt IStorage *pstgPriority,
    DWORD grfMode,
    __RPC__deref_opt_in_opt SNB snbExclude,
    DWORD reserved,
    __RPC__deref_out_opt IStorage **ppstg)
  {
    return E_NOTIMPL;
  }

  HRESULT STDMETHODCALLTYPE WebBrowser::CopyTo(
    DWORD ciidExclude,
    const IID *rgiidExclude,
    __RPC__in_opt  SNB snbExclude,
    IStorage *pstgDest)
  {
    return E_NOTIMPL;
  }

  HRESULT STDMETHODCALLTYPE WebBrowser::MoveElementTo(
    __RPC__in_string const OLECHAR *pwcsName,
    __RPC__in_opt IStorage *pstgDest,
    __RPC__in_string const OLECHAR *pwcsNewName,
    DWORD grfFlags)
  {
    return E_NOTIMPL;
  }

  HRESULT STDMETHODCALLTYPE WebBrowser::Commit(
    DWORD grfCommitFlags)
  {
    return E_NOTIMPL;
  }

  HRESULT STDMETHODCALLTYPE WebBrowser::Revert(void)
  {
    return E_NOTIMPL;
  }

  HRESULT STDMETHODCALLTYPE WebBrowser::EnumElements(
    DWORD reserved1,
    void *reserved2,
    DWORD reserved3,
    IEnumSTATSTG **ppenum)
  {
    return E_NOTIMPL;
  }

  HRESULT STDMETHODCALLTYPE WebBrowser::DestroyElement(
    __RPC__in_string const OLECHAR *pwcsName)
  {
    return E_NOTIMPL;
  }

  HRESULT STDMETHODCALLTYPE WebBrowser::RenameElement(
    __RPC__in_string const OLECHAR *pwcsOldName,
    __RPC__in_string const OLECHAR *pwcsNewName)
  {
    return E_NOTIMPL;
  }

  HRESULT STDMETHODCALLTYPE WebBrowser::SetElementTimes(
    __RPC__in_opt_string const OLECHAR *pwcsName,
    __RPC__in_opt const FILETIME *pctime,
    __RPC__in_opt const FILETIME *patime,
    __RPC__in_opt const FILETIME *pmtime)
  {
    return E_NOTIMPL;
  }

  HRESULT STDMETHODCALLTYPE WebBrowser::SetClass(
    __RPC__in REFCLSID clsid)
  {
    return S_OK;
  }

  HRESULT STDMETHODCALLTYPE WebBrowser::SetStateBits(
    DWORD grfStateBits,
    DWORD grfMask)
  {
    return E_NOTIMPL;
  }

  HRESULT STDMETHODCALLTYPE WebBrowser::Stat(
    __RPC__out STATSTG *pstatstg,
    DWORD grfStatFlag)
  {
    return E_NOTIMPL;
  }

} //namespace IE
