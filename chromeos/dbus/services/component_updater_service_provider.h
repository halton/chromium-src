// Copyright (c) 2017 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef CHROMEOS_DBUS_SERVICES_COMPONENT_UPDATER_SERVICE_PROVIDER_H_
#define CHROMEOS_DBUS_SERVICES_COMPONENT_UPDATER_SERVICE_PROVIDER_H_

#include <memory>
#include <string>

#include "base/compiler_specific.h"
#include "base/files/file_path.h"
#include "base/macros.h"
#include "base/memory/ref_counted.h"
#include "base/memory/weak_ptr.h"
#include "chromeos/chromeos_export.h"
#include "chromeos/dbus/services/cros_dbus_service.h"
#include "dbus/exported_object.h"

namespace dbus {
class MethodCall;
}

namespace chromeos {

// This class exports D-Bus methods that manage components:
//
// LoadComponent:
// % dbus-send --system --type=method_call --print-reply
//     --dest=org.chromium.ComponentUpdaterService
//     /org/chromium/ComponentUpdaterService
//     org.chromium.ComponentUpdaterService.LoadComponent
//     "string:|component name|"
//
// % string "/run/imageloader/|component name|/|version|"
//
// UnloadComponent:
// % dbus-send --system --type=method_call --print-reply
//     --dest=org.chromium.ComponentUpdaterService
//     /org/chromium/ComponentUpdaterService
//     org.chromium.ComponentUpdaterService.UnloadComponent
//     "string:|component name|"
//
// % (returns empty response on success and error response on failure)
class CHROMEOS_EXPORT ComponentUpdaterServiceProvider
    : public CrosDBusService::ServiceProviderInterface {
 public:
  // Delegate interface providing additional resources to
  // ComponentUpdaterServiceProvider.
  class Delegate {
   public:
    Delegate() {}
    virtual ~Delegate() {}

    virtual void LoadComponent(
        const std::string& name,
        base::OnceCallback<void(const base::FilePath&)> load_callback) = 0;

    virtual bool UnloadComponent(const std::string& name) = 0;

   private:
    DISALLOW_COPY_AND_ASSIGN(Delegate);
  };

  explicit ComponentUpdaterServiceProvider(std::unique_ptr<Delegate> delegate);
  ~ComponentUpdaterServiceProvider() override;

  // CrosDBusService::ServiceProviderInterface overrides:
  void Start(scoped_refptr<dbus::ExportedObject> exported_object) override;

 private:
  // Called from ExportedObject when LoadComponent() is exported as a D-Bus
  // method or failed to be exported.
  void OnExported(const std::string& interface_name,
                  const std::string& method_name,
                  bool success);

  // Called on UI thread in response to a D-Bus request.
  void LoadComponent(dbus::MethodCall* method_call,
                     dbus::ExportedObject::ResponseSender response_sender);

  // Callback executed after component loading operation is done.
  void OnLoadComponent(dbus::MethodCall* method_call,
                       dbus::ExportedObject::ResponseSender response_sender,
                       const base::FilePath& result);

  // Called on UI thread in response to a D-Bus request.
  void UnloadComponent(dbus::MethodCall* method_call,
                       dbus::ExportedObject::ResponseSender response_sender);

  std::unique_ptr<Delegate> delegate_;
  // Keep this last so that all weak pointers will be invalidated at the
  // beginning of destruction.
  base::WeakPtrFactory<ComponentUpdaterServiceProvider> weak_ptr_factory_;

  DISALLOW_COPY_AND_ASSIGN(ComponentUpdaterServiceProvider);
};

}  // namespace chromeos

#endif  // CHROMEOS_DBUS_SERVICES_COMPONENT_UPDATER_SERVICE_PROVIDER_H_
