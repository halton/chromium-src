// Copyright 2018 The Redcore (Beijing) Technology Co.,Ltd. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// YSP+ { Resource Replace }

#include "content/browser/ysp_resource_replace_interceptor.h"

#include "base/files/file_util.h"
#include "base/json/json_reader.h"
#include "base/path_service.h"
#include "base/strings/utf_string_conversions.h"
#include "chrome/common/chrome_paths.h"
#include "content/public/browser/browser_thread.h"
#include "net/url_request/url_request.h"
#include "net/url_request/url_request_file_job.h"

namespace content {

// std::unique_ptr<base::DictionaryValue>
// YSPResourceReplaceInterceptor::resource_replace_ = NULL;
CR_DEFINE_STATIC_LOCAL(std::unique_ptr<base::DictionaryValue>,
                       resource_replace_,
                       (nullptr));

static int stringmatchlen(const char* pattern,
                          int pattern_length,
                          const char* string,
                          int string_length,
                          int nocase) {
  while (pattern_length) {
    switch (pattern[0]) {
      case '*':
        while (pattern[1] == '*') {
          pattern++;
          pattern_length--;
        }
        if (pattern_length == 1)
          return 1; /** match */
        while (string_length) {
          if (stringmatchlen(pattern + 1, pattern_length - 1, string,
                             string_length, nocase))
            return 1; /** match */
          string++;
          string_length--;
        }
        return 0; /** no match */
        break;
      case '?':
        if (string_length == 0)
          return 0; /** no match */
        string++;
        string_length--;
        break;
      case '[': {
        int inot, match;

        pattern++;
        pattern_length--;
        inot = pattern[0] == '^';
        if (inot) {
          pattern++;
          pattern_length--;
        }
        match = 0;
        while (1) {
          if (pattern[0] == '\\') {
            pattern++;
            pattern_length--;
            if (pattern[0] == string[0])
              match = 1;
          } else if (pattern[0] == ']') {
            break;
          } else if (pattern_length == 0) {
            pattern--;
            pattern_length++;
            break;
          } else if (pattern[1] == '-' && pattern_length >= 3) {
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
            pattern_length -= 2;
            if (c >= start && c <= end)
              match = 1;
          } else {
            if (!nocase) {
              if (pattern[0] == string[0])
                match = 1;
            } else {
              if (tolower(static_cast<int>(pattern[0])) ==
                  tolower(static_cast<int>(string[0])))
                match = 1;
            }
          }
          pattern++;
          pattern_length--;
        }
        if (inot)
          match = !match;
        if (!match)
          return 0; /** no match */
        string++;
        string_length--;
        break;
      }
      case '\\':
        if (pattern_length >= 2) {
          pattern++;
          pattern_length--;
        }
        break;
      /** fall through */
      default:
        if (!nocase) {
          if (pattern[0] != string[0])
            return 0; /** no match */
        } else {
          if (tolower(static_cast<int>(pattern[0])) !=
              tolower(static_cast<int>(string[0])))
            return 0; /** no match */
        }
        string++;
        string_length--;
        break;
    }
    pattern++;
    pattern_length--;
    if (string_length == 0) {
      while (*pattern == '*') {
        pattern++;
        pattern_length--;
      }
      break;
    }
  }
  if (pattern_length == 0 && string_length == 0)
    return 1;
  return 0;
}

// static std::string pathParse(const std::string& path_url) {
//  std::string file_path;
//  std::string path_url_parse;
//  int i;

//  std::string u16_url = path_url;
//  int offset = u16_url.find('/', 0);
//  file_path.assign(u16_url, offset + 1, (u16_url.length() - offset + 1));
//  i = file_path.find('/');
//  for (; i != -1; i = file_path.find('/'))
//    file_path.replace(i, 1, "\\");

//  int colon_offset = file_path.find(':');
//  if (file_path.find(':') != std::string::npos) {
//    path_url_parse.append(file_path, 0, colon_offset);
//    path_url_parse.append(file_path, colon_offset + 1,
//                          file_path.length() - colon_offset - 1);
//  }
//  else
//    path_url_parse.assign(file_path);
//  //LOG(INFO) << "Path_url:" << path_url << " file_path:" << file_path;

//  return path_url_parse;
//}

std::string YSPResourceReplaceInterceptor::ResourceReplaceCompared(
    const std::string& url) const {
  base::ListValue* resource_replace_list = nullptr;
  if (url.empty())
    return std::string();
  if (resource_replace_ &&
      resource_replace_->GetList("resourceReplace", &resource_replace_list)) {
    if (resource_replace_list && !resource_replace_list->empty()) {
      for (size_t i = 0; i < resource_replace_list->GetSize(); ++i) {
        base::DictionaryValue* bm_dict = nullptr;
        if (resource_replace_list->GetDictionary(i, &bm_dict)) {
          std::string compared_source_url;
          bm_dict->GetString("sourceUrl", &compared_source_url);
          GURL source_url(compared_source_url);
          // DLOG(INFO) << " url: " << url << " source_url: " <<
          // source_url.spec(); if (stringmatch(source_url.spec().c_str(),
          // url.c_str(), true)) {
          if (source_url.spec() == url) {
            return compared_source_url;
          }
        }
      }
    }
  }
  return std::string();
}

void YSPResourceReplaceInterceptor::SetResourceReplaceValue(
    const std::string& resource_replace) {
  if (!resource_replace.empty()) {
    std::unique_ptr<base::Value> replace_value =
        base::JSONReader::Read(resource_replace);
    resource_replace_.reset(
        static_cast<base::DictionaryValue*>(replace_value.release()));
  } else {
    resource_replace_.reset();
  }
}

void YSPResourceReplaceInterceptor::SetValueFormPostTask(
    const std::string& resource_replace) {
  // DLOG(INFO) << "resource_replace:" << resource_replace;
  content::BrowserThread::PostTask(
      content::BrowserThread::IO, FROM_HERE,
      base::Bind(&SetResourceReplaceValue, resource_replace));
}

YSPResourceReplaceInterceptor::YSPResourceReplaceInterceptor() {}

YSPResourceReplaceInterceptor::~YSPResourceReplaceInterceptor() {}

net::URLRequestJob* YSPResourceReplaceInterceptor::MaybeInterceptRequest(
    net::URLRequest* request,
    net::NetworkDelegate* network_delegate) const {
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
