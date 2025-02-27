// Copyright 2017 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef CHROME_BROWSER_CHROMEOS_FILE_SYSTEM_PROVIDER_FAKE_EXTENSION_PROVIDER_H_
#define CHROME_BROWSER_CHROMEOS_FILE_SYSTEM_PROVIDER_FAKE_EXTENSION_PROVIDER_H_

#include <memory>
#include <string>

#include "chrome/browser/chromeos/file_system_provider/extension_provider.h"
#include "chrome/browser/chromeos/file_system_provider/provided_file_system_interface.h"
#include "chrome/browser/chromeos/file_system_provider/provider_interface.h"
#include "extensions/common/extension_id.h"

class Profile;

namespace chromeos {
namespace file_system_provider {

class FakeExtensionProvider : public ProviderInterface {
 public:
  ~FakeExtensionProvider() override {}

  // Returns a fake provider instance for the specified extension. The extension
  // doesn't have to exist.
  static std::unique_ptr<ProviderInterface> Create(
      const extensions::ExtensionId& extension_id);

  // ProviderInterface overrides.
  std::unique_ptr<ProvidedFileSystemInterface> CreateProvidedFileSystem(
      Profile* profile,
      const ProvidedFileSystemInfo& file_system_info) override;
  const Capabilities& GetCapabilities() const override;
  const ProviderId& GetId() const override;
  const std::string& GetName() const override;

 protected:
  FakeExtensionProvider(const extensions::ExtensionId& extension_id,
                        const Capabilities& capabilities);

  ProviderId provider_id_;
  Capabilities capabilities_;
  std::string name_;
};

}  // namespace file_system_provider
}  // namespace chromeos

#endif  // CHROME_BROWSER_CHROMEOS_FILE_SYSTEM_PROVIDER_FAKE_EXTENSION_PROVIDER_H_
