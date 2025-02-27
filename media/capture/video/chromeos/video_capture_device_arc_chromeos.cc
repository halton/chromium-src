// Copyright 2017 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "media/capture/video/chromeos/video_capture_device_arc_chromeos.h"

#include <memory>
#include <string>
#include <utility>

#include "base/bind_helpers.h"
#include "base/synchronization/waitable_event.h"
#include "base/threading/platform_thread.h"
#include "chromeos/dbus/dbus_thread_manager.h"
#include "media/base/bind_to_current_loop.h"
#include "media/capture/video/chromeos/camera_device_context.h"
#include "media/capture/video/chromeos/camera_device_delegate.h"
#include "media/capture/video/chromeos/camera_hal_delegate.h"
#include "ui/display/display.h"
#include "ui/display/display_observer.h"
#include "ui/display/screen.h"

namespace media {

VideoCaptureDeviceArcChromeOS::VideoCaptureDeviceArcChromeOS(
    scoped_refptr<base::SingleThreadTaskRunner> task_runner_for_screen_observer,
    const VideoCaptureDeviceDescriptor& device_descriptor,
    scoped_refptr<CameraHalDelegate> camera_hal_delegate)
    : device_descriptor_(device_descriptor),
      camera_hal_delegate_(std::move(camera_hal_delegate)),
      capture_task_runner_(base::ThreadTaskRunnerHandle::Get()),
      camera_device_ipc_thread_(std::string("CameraDeviceIpcThread") +
                                device_descriptor.device_id),
      screen_observer_delegate_(new ScreenObserverDelegate(
          this,
          std::move(task_runner_for_screen_observer))),
      lens_facing_(device_descriptor.facing),
      camera_orientation_(0),
      // External cameras have lens_facing as MEDIA_VIDEO_FACING_NONE.
      // We don't want to rotate the frame even if the device rotates.
      rotates_with_device_(lens_facing_ !=
                           VideoFacingMode::MEDIA_VIDEO_FACING_NONE),
      rotation_(0),
      weak_ptr_factory_(this) {
  chromeos::DBusThreadManager::Get()->GetPowerManagerClient()->AddObserver(
      this);
}

VideoCaptureDeviceArcChromeOS::~VideoCaptureDeviceArcChromeOS() {
  DCHECK(capture_task_runner_->BelongsToCurrentThread());
  DCHECK(!camera_device_ipc_thread_.IsRunning());
  screen_observer_delegate_->RemoveObserver();
  chromeos::DBusThreadManager::Get()->GetPowerManagerClient()->RemoveObserver(
      this);
}

// VideoCaptureDevice implementation.
void VideoCaptureDeviceArcChromeOS::AllocateAndStart(
    const VideoCaptureParams& params,
    std::unique_ptr<Client> client) {
  DCHECK(capture_task_runner_->BelongsToCurrentThread());
  DCHECK(!camera_device_delegate_);

  if (!camera_device_ipc_thread_.Start()) {
    std::string error_msg = "Failed to start device thread";
    LOG(ERROR) << error_msg;
    client->OnError(FROM_HERE, error_msg);
    return;
  }
  capture_params_ = params;
  device_context_ = std::make_unique<CameraDeviceContext>(std::move(client));
  camera_device_delegate_ = std::make_unique<CameraDeviceDelegate>(
      device_descriptor_, camera_hal_delegate_,
      camera_device_ipc_thread_.task_runner());
  OpenDevice();
}

void VideoCaptureDeviceArcChromeOS::StopAndDeAllocate() {
  DCHECK(capture_task_runner_->BelongsToCurrentThread());

  if (!camera_device_delegate_) {
    return;
  }
  CloseDevice(base::Closure());
  camera_device_ipc_thread_.Stop();
  camera_device_delegate_.reset();
  device_context_.reset();
}

void VideoCaptureDeviceArcChromeOS::TakePhoto(TakePhotoCallback callback) {
  DCHECK(capture_task_runner_->BelongsToCurrentThread());
  DCHECK(camera_device_delegate_);
  camera_device_ipc_thread_.task_runner()->PostTask(
      FROM_HERE, base::Bind(&CameraDeviceDelegate::TakePhoto,
                            camera_device_delegate_->GetWeakPtr(),
                            base::Passed(&callback)));
}

void VideoCaptureDeviceArcChromeOS::GetPhotoState(
    GetPhotoStateCallback callback) {
  DCHECK(capture_task_runner_->BelongsToCurrentThread());
  camera_device_ipc_thread_.task_runner()->PostTask(
      FROM_HERE, base::Bind(&CameraDeviceDelegate::GetPhotoState,
                            camera_device_delegate_->GetWeakPtr(),
                            base::Passed(&callback)));
}

void VideoCaptureDeviceArcChromeOS::SetPhotoOptions(
    mojom::PhotoSettingsPtr settings,
    SetPhotoOptionsCallback callback) {
  DCHECK(capture_task_runner_->BelongsToCurrentThread());
  camera_device_ipc_thread_.task_runner()->PostTask(
      FROM_HERE, base::Bind(&CameraDeviceDelegate::SetPhotoOptions,
                            camera_device_delegate_->GetWeakPtr(),
                            base::Passed(&settings), base::Passed(&callback)));
}

void VideoCaptureDeviceArcChromeOS::SuspendImminent(
    power_manager::SuspendImminent::Reason reason) {
  capture_task_runner_->PostTask(
      FROM_HERE,
      base::BindOnce(&VideoCaptureDeviceArcChromeOS::CloseDevice,
                     weak_ptr_factory_.GetWeakPtr(),
                     BindToCurrentLoop(chromeos::DBusThreadManager::Get()
                                           ->GetPowerManagerClient()
                                           ->GetSuspendReadinessCallback())));
}

void VideoCaptureDeviceArcChromeOS::SuspendDone(
    const base::TimeDelta& sleep_duration) {
  capture_task_runner_->PostTask(
      FROM_HERE, base::BindOnce(&VideoCaptureDeviceArcChromeOS::OpenDevice,
                                weak_ptr_factory_.GetWeakPtr()));
}

void VideoCaptureDeviceArcChromeOS::OpenDevice() {
  DCHECK(capture_task_runner_->BelongsToCurrentThread());

  // It's safe to pass unretained |device_context_| here since
  // VideoCaptureDeviceArcChromeOS owns |camera_device_delegate_| and makes
  // sure |device_context_| outlives |camera_device_delegate_|.
  camera_device_ipc_thread_.task_runner()->PostTask(
      FROM_HERE,
      base::Bind(&CameraDeviceDelegate::AllocateAndStart,
                 camera_device_delegate_->GetWeakPtr(), capture_params_,
                 base::Unretained(device_context_.get())));
  camera_device_ipc_thread_.task_runner()->PostTask(
      FROM_HERE, base::Bind(&CameraDeviceDelegate::SetRotation,
                            camera_device_delegate_->GetWeakPtr(), rotation_));
}

void VideoCaptureDeviceArcChromeOS::CloseDevice(base::Closure callback) {
  DCHECK(capture_task_runner_->BelongsToCurrentThread());

  if (!camera_device_delegate_) {
    return;
  }
  // We do our best to allow the camera HAL cleanly shut down the device.  In
  // general we don't trust the camera HAL so if the device does not close in
  // time we simply terminate the Mojo channel by resetting
  // |camera_device_delegate_|.
  base::WaitableEvent device_closed(
      base::WaitableEvent::ResetPolicy::MANUAL,
      base::WaitableEvent::InitialState::NOT_SIGNALED);
  camera_device_ipc_thread_.task_runner()->PostTask(
      FROM_HERE, base::Bind(&CameraDeviceDelegate::StopAndDeAllocate,
                            camera_device_delegate_->GetWeakPtr(),
                            base::Bind(
                                [](base::WaitableEvent* device_closed) {
                                  device_closed->Signal();
                                },
                                base::Unretained(&device_closed))));
  base::TimeDelta kWaitTimeoutSecs = base::TimeDelta::FromSeconds(3);
  device_closed.TimedWait(kWaitTimeoutSecs);
  if (callback) {
    callback.Run();
  }
}

void VideoCaptureDeviceArcChromeOS::SetDisplayRotation(
    const display::Display& display) {
  DCHECK(capture_task_runner_->BelongsToCurrentThread());
  if (display.IsInternal())
    SetRotation(display.rotation() * 90);
}

void VideoCaptureDeviceArcChromeOS::SetRotation(int rotation) {
  DCHECK(capture_task_runner_->BelongsToCurrentThread());
  if (!rotates_with_device_) {
    rotation = 0;
  } else if (lens_facing_ == VideoFacingMode::MEDIA_VIDEO_FACING_ENVIRONMENT) {
    // Original frame when |rotation| = 0
    // -----------------------
    // |          *          |
    // |         * *         |
    // |        *   *        |
    // |       *******       |
    // |      *       *      |
    // |     *         *     |
    // -----------------------
    //
    // |rotation| = 90, this is what back camera sees
    // -----------------------
    // |    ********         |
    // |       *   ****      |
    // |       *      ***    |
    // |       *      ***    |
    // |       *   ****      |
    // |    ********         |
    // -----------------------
    //
    // |rotation| = 90, this is what front camera sees
    // -----------------------
    // |         ********    |
    // |      ****   *       |
    // |    ***      *       |
    // |    ***      *       |
    // |      ****   *       |
    // |         ********    |
    // -----------------------
    //
    // Therefore, for back camera, we need to rotate (360 - |rotation|).
    rotation = (360 - rotation) % 360;
  }
  // Take into account camera orientation w.r.t. the display. External cameras
  // would have camera_orientation_ as 0.
  rotation_ = (rotation + camera_orientation_) % 360;
  if (camera_device_ipc_thread_.IsRunning()) {
    camera_device_ipc_thread_.task_runner()->PostTask(
        FROM_HERE,
        base::Bind(&CameraDeviceDelegate::SetRotation,
                   camera_device_delegate_->GetWeakPtr(), rotation_));
  }
}

}  // namespace media
