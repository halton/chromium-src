//YSP+ { Resource Replace }

#include "content/browser/ysp_resource_replace_interceptor.h"

#include "base/files/file_util.h"
#include "base/path_service.h"
#include "base/json/json_reader.h"
#include "base/strings/utf_string_conversions.h"
#include "chrome/common/chrome_paths.h"
#include "net/url_request/url_request_file_job.h"
#include "net/url_request/url_request.h"
#include "content/public/browser/browser_thread.h"

namespace content {

//std::unique_ptr<base::DictionaryValue> YSPResourceReplaceInterceptor::resourceReplace_ = NULL;
CR_DEFINE_STATIC_LOCAL(std::unique_ptr<base::DictionaryValue>, resourceReplace_,(nullptr));

static int stringmatchlen(const char *pattern, int patternLen,
  const char *string, int stringLen, int nocase) {
  while (patternLen) {
    switch (pattern[0]) {
    case '*':
      while (pattern[1] == '*') {
        pattern++;
        patternLen--;
      }
      if (patternLen == 1)
        return 1; /** match */
      while (stringLen) {
        if (stringmatchlen(pattern + 1, patternLen - 1,
          string, stringLen, nocase))
          return 1; /** match */
        string++;
        stringLen--;
      }
      return 0; /** no match */
      break;
    case '?':
      if (stringLen == 0)
        return 0; /** no match */
      string++;
      stringLen--;
      break;
    case '[':
    {
      int inot, match;

      pattern++;
      patternLen--;
      inot = pattern[0] == '^';
      if (inot) {
        pattern++;
        patternLen--;
      }
      match = 0;
      while (1) {
        if (pattern[0] == '\\') {
          pattern++;
          patternLen--;
          if (pattern[0] == string[0])
            match = 1;
        }
        else if (pattern[0] == ']') {
          break;
        }
        else if (patternLen == 0) {
          pattern--;
          patternLen++;
          break;
        }
        else if (pattern[1] == '-' && patternLen >= 3) {
          int start = pattern[0];
          int end = pattern[2];
          int c = string[0];
          if (start > end) {
            int t = start;
            start = end;
            end = t;
          }
          if (nocase) {
            start = tolower(start);
            end = tolower(end);
            c = tolower(c);
          }
          pattern += 2;
          patternLen -= 2;
          if (c >= start && c <= end)
            match = 1;
        }
        else {
          if (!nocase) {
            if (pattern[0] == string[0])
              match = 1;
          }
          else {
            if (tolower((int)pattern[0]) == tolower((int)string[0]))
              match = 1;
          }
        }
        pattern++;
        patternLen--;
      }
      if (inot)
        match = !match;
      if (!match)
        return 0; /** no match */
      string++;
      stringLen--;
      break;
    }
    case '\\':
      if (patternLen >= 2) {
        pattern++;
        patternLen--;
      }
      break;
      /** fall through */
    default:
      if (!nocase) {
        if (pattern[0] != string[0])
          return 0; /** no match */
      }
      else {
        if (tolower((int)pattern[0]) != tolower((int)string[0]))
          return 0; /** no match */
      }
      string++;
      stringLen--;
      break;
    }
    pattern++;
    patternLen--;
    if (stringLen == 0) {
      while (*pattern == '*') {
        pattern++;
        patternLen--;
      }
      break;
    }
  }
  if (patternLen == 0 && stringLen == 0)
    return 1;
  return 0;
}
#if 0
static int stringmatch(const char *pattern, const char *string, int nocase) {
  return stringmatchlen(pattern, strlen(pattern), string, strlen(string), nocase);
}
#endif

// static std::string pathParse(const std::string& path_url) {
//   std::string filePath;
//   std::string path_url_parse;
//   int i;

//   std::string u16_url = path_url;
//   int offset = u16_url.find('/', 0);
//   filePath.assign(u16_url, offset + 1, (u16_url.length() - offset + 1));
//   i = filePath.find('/');
//   for (; i != -1; i = filePath.find('/'))
//     filePath.replace(i, 1, "\\");

//   int ColonOffset = filePath.find(':');
//   if (filePath.find(':') != std::string::npos) {
//     path_url_parse.append(filePath, 0, ColonOffset);
//     path_url_parse.append(filePath, ColonOffset + 1, filePath.length() - ColonOffset - 1);
//   }
//   else
//     path_url_parse.assign(filePath);
//   //LOG(INFO) << "Path_url:" << path_url << " FilePath:" << filePath;

//   return path_url_parse;
// }

std::string YSPResourceReplaceInterceptor::ResourceReplaceCompared(const std::string url) const {
  base::ListValue* resourceReplaceList = nullptr;
  if (url.empty())
    return std::string();
  if (resourceReplace_ && resourceReplace_->GetList("resourceReplace", &resourceReplaceList))
    if (resourceReplaceList && !resourceReplaceList->empty()) {
      for (size_t i = 0; i<resourceReplaceList->GetSize(); ++i) {
        base::DictionaryValue* bmDict = nullptr;
        if (resourceReplaceList->GetDictionary(i, &bmDict)) {
          std::string comparedSourceUrl;
          bmDict->GetString("sourceUrl", &comparedSourceUrl);
          GURL sourceUrl(comparedSourceUrl);
          //DLOG(INFO) << " url: " << url << " source_url: " << sourceUrl.spec();
          //if (stringmatch(sourceUrl.spec().c_str(), url.c_str(), true)) {
          if (sourceUrl.spec() == url) {
            return comparedSourceUrl;
          }
        }
      }
    }
  return std::string();
}

void YSPResourceReplaceInterceptor::SetResourceReplaceValue(const std::string& resourceReplace) {
  if (!resourceReplace.empty()) {
    std::unique_ptr<base::Value> replaceValue = base::JSONReader::Read(resourceReplace);
    resourceReplace_.reset(static_cast<base::DictionaryValue*>(replaceValue.release()));
  } else
    resourceReplace_.reset();
}

void YSPResourceReplaceInterceptor::SetValueFormPostTask(const std::string& resourceReplace)
{
  //DLOG(INFO) << "resourceReplace:" << resourceReplace;
  content::BrowserThread::PostTask(
    content::BrowserThread::IO, FROM_HERE,
    base::Bind(&SetResourceReplaceValue, resourceReplace));
}

YSPResourceReplaceInterceptor::YSPResourceReplaceInterceptor() {
}

YSPResourceReplaceInterceptor::~YSPResourceReplaceInterceptor() {
}

net::URLRequestJob* YSPResourceReplaceInterceptor::MaybeInterceptRequest(
    net::URLRequest* request, net::NetworkDelegate* network_delegate) const {

  if (!request->url().is_valid())
    return nullptr;

  net::URLRequestJob* job = nullptr;
  // FIXME(halton): Implement YSPResourceReplaceRequestHandler
  // base::FilePath path;

  // std::string source_url = ResourceReplaceCompared(request->url().spec());
  // if (!source_url.empty()) {
  //   base::PathService::Get(chrome::DIR_USER_DATA, &path);
  //   if (path.empty())
  //     return job;
  //   path = path.Append(FILE_PATH_LITERAL("Default\\Resource"));
  //   // StringPieceType
  //   path = path.AppendASCII(pathParse(source_url).c_str());
  //   if (FILE *IsFileExist = base::OpenFile(path, "rb")) {
  //     base::CloseFile(IsFileExist);
  //     return new net::URLRequestFileJob(
  //       request, network_delegate, path,
  //       BrowserThread::GetBlockingPool()->
  //       GetTaskRunnerWithShutdownBehavior(
  //         base::SequencedWorkerPool::SKIP_ON_SHUTDOWN));
  //   }
  // }

  return job;
}

}  // namespace content
