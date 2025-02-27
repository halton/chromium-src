// Copyright (c) 2012 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "content/common/resource_messages.h"

#include "base/files/file.h"
#include "base/files/platform_file.h"
#include "ipc/ipc_mojo_param_traits.h"
#include "ipc/ipc_platform_file.h"
#include "net/base/load_timing_info.h"
#include "net/http/http_response_headers.h"

namespace IPC {

void ParamTraits<network::DataElement>::Write(base::Pickle* m,
                                              const param_type& p) {
  WriteParam(m, static_cast<int>(p.type()));
  switch (p.type()) {
    case network::DataElement::TYPE_BYTES: {
      m->WriteData(p.bytes(), static_cast<int>(p.length()));
      break;
    }
    case network::DataElement::TYPE_BYTES_DESCRIPTION: {
      WriteParam(m, p.length());
      break;
    }
    case network::DataElement::TYPE_FILE: {
      WriteParam(m, p.path());
      WriteParam(m, p.offset());
      WriteParam(m, p.length());
      WriteParam(m, p.expected_modification_time());
      break;
    }
    case network::DataElement::TYPE_RAW_FILE: {
      WriteParam(
          m, IPC::GetPlatformFileForTransit(p.file().GetPlatformFile(),
                                            false /* close_source_handle */));
      WriteParam(m, p.path());
      WriteParam(m, p.offset());
      WriteParam(m, p.length());
      WriteParam(m, p.expected_modification_time());
      break;
    }
    case network::DataElement::TYPE_FILE_FILESYSTEM: {
      WriteParam(m, p.filesystem_url());
      WriteParam(m, p.offset());
      WriteParam(m, p.length());
      WriteParam(m, p.expected_modification_time());
      break;
    }
    case network::DataElement::TYPE_BLOB: {
      WriteParam(m, p.blob_uuid());
      WriteParam(m, p.offset());
      WriteParam(m, p.length());
      break;
    }
    case network::DataElement::TYPE_DISK_CACHE_ENTRY: {
      NOTREACHED() << "Can't be sent by IPC.";
      break;
    }
    case network::DataElement::TYPE_DATA_PIPE: {
      WriteParam(m,
                 const_cast<network::mojom::DataPipeGetterPtr&>(p.data_pipe())
                     .PassInterface()
                     .PassHandle()
                     .release());
      break;
    }
    case network::DataElement::TYPE_UNKNOWN: {
      NOTREACHED();
      break;
    }
  }
}

bool ParamTraits<network::DataElement>::Read(const base::Pickle* m,
                                             base::PickleIterator* iter,
                                             param_type* r) {
  int type;
  if (!ReadParam(m, iter, &type))
    return false;
  switch (type) {
    case network::DataElement::TYPE_BYTES: {
      const char* data;
      int len;
      if (!iter->ReadData(&data, &len))
        return false;
      r->SetToBytes(data, len);
      return true;
    }
    case network::DataElement::TYPE_BYTES_DESCRIPTION: {
      uint64_t length;
      if (!ReadParam(m, iter, &length))
        return false;
      r->SetToBytesDescription(length);
      return true;
    }
    case network::DataElement::TYPE_FILE: {
      base::FilePath file_path;
      uint64_t offset, length;
      base::Time expected_modification_time;
      if (!ReadParam(m, iter, &file_path))
        return false;
      if (!ReadParam(m, iter, &offset))
        return false;
      if (!ReadParam(m, iter, &length))
        return false;
      if (!ReadParam(m, iter, &expected_modification_time))
        return false;
      r->SetToFilePathRange(file_path, offset, length,
                            expected_modification_time);
      return true;
    }
    case network::DataElement::TYPE_RAW_FILE: {
      IPC::PlatformFileForTransit platform_file_for_transit;
      if (!ReadParam(m, iter, &platform_file_for_transit))
        return false;
      base::File file = PlatformFileForTransitToFile(platform_file_for_transit);
      base::FilePath file_path;
      if (!ReadParam(m, iter, &file_path))
        return false;
      uint64_t offset;
      if (!ReadParam(m, iter, &offset))
        return false;
      uint64_t length;
      if (!ReadParam(m, iter, &length))
        return false;
      base::Time expected_modification_time;
      if (!ReadParam(m, iter, &expected_modification_time))
        return false;
      r->SetToFileRange(std::move(file), file_path, offset, length,
                        expected_modification_time);
      return true;
    }
    case network::DataElement::TYPE_FILE_FILESYSTEM: {
      GURL file_system_url;
      uint64_t offset, length;
      base::Time expected_modification_time;
      if (!ReadParam(m, iter, &file_system_url))
        return false;
      if (!ReadParam(m, iter, &offset))
        return false;
      if (!ReadParam(m, iter, &length))
        return false;
      if (!ReadParam(m, iter, &expected_modification_time))
        return false;
      r->SetToFileSystemUrlRange(file_system_url, offset, length,
                                 expected_modification_time);
      return true;
    }
    case network::DataElement::TYPE_BLOB: {
      std::string blob_uuid;
      uint64_t offset, length;
      if (!ReadParam(m, iter, &blob_uuid))
        return false;
      if (!ReadParam(m, iter, &offset))
        return false;
      if (!ReadParam(m, iter, &length))
        return false;
      r->SetToBlobRange(blob_uuid, offset, length);
      return true;
    }
    case network::DataElement::TYPE_DISK_CACHE_ENTRY: {
      NOTREACHED() << "Can't be sent by IPC.";
      return false;
    }
    case network::DataElement::TYPE_DATA_PIPE: {
      network::mojom::DataPipeGetterPtr data_pipe_getter;
      mojo::MessagePipeHandle message_pipe;
      if (!ReadParam(m, iter, &message_pipe))
        return false;
      data_pipe_getter.Bind(network::mojom::DataPipeGetterPtrInfo(
          mojo::ScopedMessagePipeHandle(message_pipe), 0u));
      r->SetToDataPipe(std::move(data_pipe_getter));
      return true;
    }
    case network::DataElement::TYPE_UNKNOWN: {
      NOTREACHED();
      return false;
    }
  }
  return false;
}

void ParamTraits<network::DataElement>::Log(const param_type& p,
                                            std::string* l) {
  l->append("<network::DataElement>");
}

void ParamTraits<scoped_refptr<network::ResourceRequestBody>>::Write(
    base::Pickle* m,
    const param_type& p) {
  WriteParam(m, p.get() != nullptr);
  if (p.get()) {
    WriteParam(m, *p->elements());
    WriteParam(m, p->identifier());
    WriteParam(m, p->contains_sensitive_info());
  }
}

bool ParamTraits<scoped_refptr<network::ResourceRequestBody>>::Read(
    const base::Pickle* m,
    base::PickleIterator* iter,
    param_type* r) {
  bool has_object;
  if (!ReadParam(m, iter, &has_object))
    return false;
  if (!has_object)
    return true;
  std::vector<network::DataElement> elements;
  if (!ReadParam(m, iter, &elements))
    return false;
  int64_t identifier;
  if (!ReadParam(m, iter, &identifier))
    return false;
  bool contains_sensitive_info;
  if (!ReadParam(m, iter, &contains_sensitive_info))
    return false;
  *r = new network::ResourceRequestBody;
  (*r)->swap_elements(&elements);
  (*r)->set_identifier(identifier);
  (*r)->set_contains_sensitive_info(contains_sensitive_info);
  return true;
}

void ParamTraits<scoped_refptr<network::ResourceRequestBody>>::Log(
    const param_type& p,
    std::string* l) {
  l->append("<ResourceRequestBody>");
}

}  // namespace IPC
