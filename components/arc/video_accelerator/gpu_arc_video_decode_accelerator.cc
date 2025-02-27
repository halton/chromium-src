// Copyright 2017 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "components/arc/video_accelerator/gpu_arc_video_decode_accelerator.h"

#include "base/callback_helpers.h"
#include "base/metrics/histogram_macros.h"
#include "build/build_config.h"
#include "components/arc/video_accelerator/protected_buffer_manager.h"
#include "media/base/video_frame.h"
#include "media/base/video_types.h"
#include "media/gpu/format_utils.h"
#include "media/gpu/gpu_video_decode_accelerator_factory.h"
#include "mojo/public/cpp/system/platform_handle.h"

#define VLOGF(level) VLOG(level) << __func__ << "(): "
#define DVLOGF(level) DVLOG(level) << __func__ << "(): "
#define VPLOGF(level) VPLOG(level) << __func__ << "(): "

// Make sure arc::mojom::VideoDecodeAccelerator::Result and
// media::VideoDecodeAccelerator::Error match.
static_assert(static_cast<int>(
                  arc::mojom::VideoDecodeAccelerator::Result::ILLEGAL_STATE) ==
                  media::VideoDecodeAccelerator::Error::ILLEGAL_STATE,
              "enum mismatch");
static_assert(
    static_cast<int>(
        arc::mojom::VideoDecodeAccelerator::Result::INVALID_ARGUMENT) ==
        media::VideoDecodeAccelerator::Error::INVALID_ARGUMENT,
    "enum mismatch");
static_assert(
    static_cast<int>(
        arc::mojom::VideoDecodeAccelerator::Result::UNREADABLE_INPUT) ==
        media::VideoDecodeAccelerator::Error::UNREADABLE_INPUT,
    "enum mismatch");
static_assert(
    static_cast<int>(
        arc::mojom::VideoDecodeAccelerator::Result::PLATFORM_FAILURE) ==
        media::VideoDecodeAccelerator::Error::PLATFORM_FAILURE,
    "enum mismatch");

namespace {

// An arbitrarily chosen limit of the number of output buffers. The number of
// output buffers used is requested from the untrusted client side.
constexpr size_t kMaxOutputBufferCount = 32;

// Maximum number of concurrent ARC video clients.
// Currently we have no way to know the resources are not enough to create more
// VDAs. Arbitrarily chosen a reasonable constant as the limit.
constexpr size_t kMaxConcurrentClients = 8;

// Maximum number of protected input buffers.
// It is disallowed to allocate more than number of protected input buffers.
constexpr size_t kMaxProtectedInputBuffers = 8;

base::ScopedFD UnwrapFdFromMojoHandle(mojo::ScopedHandle handle) {
  if (!handle.is_valid()) {
    VLOGF(1) << "Handle is invalid.";
    return base::ScopedFD();
  }

  base::PlatformFile platform_file;
  MojoResult mojo_result =
      mojo::UnwrapPlatformFile(std::move(handle), &platform_file);
  if (mojo_result != MOJO_RESULT_OK) {
    VLOGF(1) << "UnwrapPlatformFile failed: " << mojo_result;
    return base::ScopedFD();
  }

  return base::ScopedFD(platform_file);
}

arc::mojom::VideoDecodeAccelerator::Result ConvertErrorCode(
    media::VideoDecodeAccelerator::Error error) {
  switch (error) {
    case media::VideoDecodeAccelerator::ILLEGAL_STATE:
      return arc::mojom::VideoDecodeAccelerator::Result::ILLEGAL_STATE;
    case media::VideoDecodeAccelerator::INVALID_ARGUMENT:
      return arc::mojom::VideoDecodeAccelerator::Result::INVALID_ARGUMENT;
    case media::VideoDecodeAccelerator::UNREADABLE_INPUT:
      return arc::mojom::VideoDecodeAccelerator::Result::UNREADABLE_INPUT;
    case media::VideoDecodeAccelerator::PLATFORM_FAILURE:
      return arc::mojom::VideoDecodeAccelerator::Result::PLATFORM_FAILURE;
    default:
      VLOGF(1) << "Unknown error: " << error;
      return ::arc::mojom::VideoDecodeAccelerator::Result::PLATFORM_FAILURE;
  }
}
}  // namespace

namespace arc {

class GpuArcVideoDecodeAccelerator::ScopedBitstreamBuffer {
 public:
  ScopedBitstreamBuffer(ScopedBitstreamBuffer&& scoped_bitstream_buffer) {
    bitstream_buffer_ = scoped_bitstream_buffer.release();
  }
  ScopedBitstreamBuffer& operator=(
      ScopedBitstreamBuffer&& scoped_bitstream_buffer) {
    CloseIfNeeded();
    bitstream_buffer_ = scoped_bitstream_buffer.release();
    return *this;
  }
  ~ScopedBitstreamBuffer() { CloseIfNeeded(); }
  explicit ScopedBitstreamBuffer(const media::BitstreamBuffer& bitstream_buffer)
      : bitstream_buffer_(bitstream_buffer) {}

  media::BitstreamBuffer release() WARN_UNUSED_RESULT {
    return std::exchange(bitstream_buffer_, media::BitstreamBuffer());
  }

 private:
  void CloseIfNeeded() {
    if (bitstream_buffer_.handle().IsValid()) {
      VLOGF(2) << "Handle is not closed yet and closed here, "
               << "fd=" << bitstream_buffer_.handle().GetHandle();
      bitstream_buffer_.handle().Close();
    }
  }
  media::BitstreamBuffer bitstream_buffer_;
  DISALLOW_COPY_AND_ASSIGN(ScopedBitstreamBuffer);
};

// static
size_t GpuArcVideoDecodeAccelerator::client_count_ = 0;

GpuArcVideoDecodeAccelerator::GpuArcVideoDecodeAccelerator(
    const gpu::GpuPreferences& gpu_preferences,
    ProtectedBufferManager* protected_buffer_manager)
    : gpu_preferences_(gpu_preferences),
      protected_buffer_manager_(protected_buffer_manager) {}

GpuArcVideoDecodeAccelerator::~GpuArcVideoDecodeAccelerator() {
  DCHECK_CALLED_ON_VALID_THREAD(thread_checker_);
  if (vda_)
    client_count_--;
}

void GpuArcVideoDecodeAccelerator::ProvidePictureBuffers(
    uint32_t requested_num_of_buffers,
    media::VideoPixelFormat format,
    uint32_t textures_per_buffer,
    const gfx::Size& dimensions,
    uint32_t texture_target) {
  VLOGF(2);
  DCHECK_CALLED_ON_VALID_THREAD(thread_checker_);
  DCHECK(client_);

  if ((output_pixel_format_ != media::PIXEL_FORMAT_UNKNOWN) &&
      (output_pixel_format_ != format)) {
    VLOGF(1) << "Unexpected output format."
             << " output_pixel_format: "
             << media::VideoPixelFormatToString(output_pixel_format_)
             << " format: " << media::VideoPixelFormatToString(format);
    client_->NotifyError(
        mojom::VideoDecodeAccelerator::Result::INVALID_ARGUMENT);
    return;
  }

  mojom::HalPixelFormat pixel_format;
  switch (format) {
    case media::PIXEL_FORMAT_I420:
    case media::PIXEL_FORMAT_YV12:
    case media::PIXEL_FORMAT_NV12:
    case media::PIXEL_FORMAT_NV21:
      // HAL_PIXEL_FORMAT_YCbCr_420_888 is the flexible pixel format in Android
      // which handles all 420 formats, with both orderings of chroma (CbCr and
      // CrCb) as well as planar and semi-planar layouts.
      pixel_format = mojom::HalPixelFormat::HAL_PIXEL_FORMAT_YCbCr_420_888;
      break;
    case media::PIXEL_FORMAT_ARGB:
      pixel_format = mojom::HalPixelFormat::HAL_PIXEL_FORMAT_BGRA_8888;
      break;
    default:
      VLOGF(1) << "Format not supported: "
               << media::VideoPixelFormatToString(format);
      client_->NotifyError(
          mojom::VideoDecodeAccelerator::Result::INVALID_ARGUMENT);
      return;
  }

  output_pixel_format_ = format;
  pending_coded_size_ = dimensions;

  if (client_.version() == 0) {
    auto pbf = mojom::PictureBufferFormatDeprecated::New();
    pbf->pixel_format = pixel_format;
    pbf->buffer_size = media::VideoFrame::AllocationSize(format, dimensions);
    pbf->min_num_buffers = requested_num_of_buffers;
    pbf->coded_width = dimensions.width();
    pbf->coded_height = dimensions.height();
    client_->ProvidePictureBuffersDeprecated(std::move(pbf));
  } else {
    auto pbf = mojom::PictureBufferFormat::New();
    pbf->min_num_buffers = requested_num_of_buffers;
    pbf->coded_size = dimensions;
    client_->ProvidePictureBuffers(std::move(pbf));
  }
}

void GpuArcVideoDecodeAccelerator::DismissPictureBuffer(
    int32_t picture_buffer) {
  VLOGF(2) << "picture_buffer=" << picture_buffer;
  DCHECK_CALLED_ON_VALID_THREAD(thread_checker_);
  // no-ops.
  // DismissPictureBuffer() is required for VDA::ALLOCATE mode,
  // as PictureBuffers are backed by texture IDs and not refcounted then.
  // This class supports VDA::IMPORT mode only however,
  // where PictureBuffers are backed by dmabufs, which are refcounted.
  // Both the client and the VDA itself maintain their own references and
  // can drop them independently when no longer needed, so there is no need to
  // explicitly notify the client about this.
}

void GpuArcVideoDecodeAccelerator::PictureReady(const media::Picture& picture) {
  DVLOGF(4);
  DCHECK_CALLED_ON_VALID_THREAD(thread_checker_);
  DCHECK(client_);
  mojom::PicturePtr pic = mojom::Picture::New();
  pic->picture_buffer_id = picture.picture_buffer_id();
  pic->bitstream_id = picture.bitstream_buffer_id();
  pic->crop_rect = picture.visible_rect();
  client_->PictureReady(std::move(pic));
}

void GpuArcVideoDecodeAccelerator::NotifyEndOfBitstreamBuffer(
    int32_t bitstream_buffer_id) {
  DVLOGF(4);
  DCHECK_CALLED_ON_VALID_THREAD(thread_checker_);
  DCHECK(client_);
  client_->NotifyEndOfBitstreamBuffer(bitstream_buffer_id);
}

void GpuArcVideoDecodeAccelerator::NotifyFlushDone() {
  VLOGF(2);
  DCHECK_CALLED_ON_VALID_THREAD(thread_checker_);
  if (pending_flush_callbacks_.empty()) {
    VLOGF(1) << "Unexpected NotifyFlushDone() callback received from VDA.";
    client_->NotifyError(
        mojom::VideoDecodeAccelerator::Result::PLATFORM_FAILURE);
    return;
  }
  // VDA::Flush are processed in order by VDA.
  // NotifyFlushDone() is also processed in order.
  std::move(pending_flush_callbacks_.front())
      .Run(mojom::VideoDecodeAccelerator::Result::SUCCESS);
  pending_flush_callbacks_.pop();
}

void GpuArcVideoDecodeAccelerator::NotifyResetDone() {
  VLOGF(2);
  DCHECK_CALLED_ON_VALID_THREAD(thread_checker_);
  if (pending_reset_callback_.is_null()) {
    VLOGF(1) << "Unexpected NotifyResetDone() callback received from VDA.";
    client_->NotifyError(
        mojom::VideoDecodeAccelerator::Result::PLATFORM_FAILURE);
    return;
  }
  // Reset() is processed asynchronously and any preceding Flush() calls might
  // have been dropped by the VDA.
  // We still need to execute callbacks associated with them,
  // even if we did not receive NotifyFlushDone() for them from the VDA.
  while (!pending_flush_callbacks_.empty()) {
    VLOGF(2) << "Flush is cancelled.";
    std::move(pending_flush_callbacks_.front())
        .Run(mojom::VideoDecodeAccelerator::Result::CANCELLED);
    pending_flush_callbacks_.pop();
  }

  base::ResetAndReturn(&pending_reset_callback_)
      .Run(mojom::VideoDecodeAccelerator::Result::SUCCESS);
  RunPendingRequests();
}

void GpuArcVideoDecodeAccelerator::NotifyError(
    media::VideoDecodeAccelerator::Error error) {
  DCHECK_CALLED_ON_VALID_THREAD(thread_checker_);
  VLOGF(1) << "error= " << static_cast<int>(error);
  DCHECK(client_);

  // Reset and Return all the pending callbacks with PLATFORM_FAILURE
  while (!pending_flush_callbacks_.empty()) {
    std::move(pending_flush_callbacks_.front())
        .Run(mojom::VideoDecodeAccelerator::Result::PLATFORM_FAILURE);
    pending_flush_callbacks_.pop();
  }
  if (pending_reset_callback_) {
    base::ResetAndReturn(&pending_reset_callback_)
        .Run(mojom::VideoDecodeAccelerator::Result::PLATFORM_FAILURE);
  }

  error_state_ = true;
  client_->NotifyError(ConvertErrorCode(error));
  // Because the current state is ERROR, all the pending requests will be
  // dropped, and their callback will return with an error state.
  RunPendingRequests();
}

void GpuArcVideoDecodeAccelerator::RunPendingRequests() {
  DCHECK_CALLED_ON_VALID_THREAD(thread_checker_);
  while (!pending_reset_callback_ && !pending_requests_.empty()) {
    ExecuteRequest(std::move(pending_requests_.front()));
    pending_requests_.pop();
  }
}

void GpuArcVideoDecodeAccelerator::FlushRequest(
    PendingCallback cb,
    media::VideoDecodeAccelerator* vda) {
  DCHECK_CALLED_ON_VALID_THREAD(thread_checker_);
  DCHECK(vda);
  pending_flush_callbacks_.emplace(std::move(cb));
  vda->Flush();
}

void GpuArcVideoDecodeAccelerator::ResetRequest(
    PendingCallback cb,
    media::VideoDecodeAccelerator* vda) {
  DCHECK_CALLED_ON_VALID_THREAD(thread_checker_);
  DCHECK(vda);
  pending_reset_callback_ = std::move(cb);
  vda->Reset();
}

void GpuArcVideoDecodeAccelerator::DecodeRequest(
    ScopedBitstreamBuffer scoped_bitstream_buffer,
    PendingCallback cb,
    media::VideoDecodeAccelerator* vda) {
  DCHECK_CALLED_ON_VALID_THREAD(thread_checker_);
  DCHECK(vda);
  vda->Decode(scoped_bitstream_buffer.release());
}

void GpuArcVideoDecodeAccelerator::ExecuteRequest(
    std::pair<PendingRequest, PendingCallback> request) {
  DVLOGF(4);
  auto& callback = request.second;
  if (!vda_) {
    VLOGF(1) << "VDA not initialized.";
    if (callback)
      std::move(callback).Run(
          mojom::VideoDecodeAccelerator::Result::ILLEGAL_STATE);
    return;
  }
  if (error_state_) {
    VLOGF(1) << "GAVDA state: ERROR";
    if (callback)
      std::move(callback).Run(
          mojom::VideoDecodeAccelerator::Result::PLATFORM_FAILURE);
    return;
  }
  // When pending_reset_callback_ isn't null, GAVDA is awaiting a preceding
  // Reset() to be finished. Any requests are pended.
  // There is no need to take pending_flush_callbacks into account.
  // VDA::Reset() can be called while VDA::Flush() are being executed.
  // VDA::Flush()es can be called regardless of whether or not there is a
  // preceding VDA::Flush().
  if (pending_reset_callback_) {
    pending_requests_.emplace(std::move(request));
    return;
  }
  std::move(request.first).Run(std::move(callback), vda_.get());
}

void GpuArcVideoDecodeAccelerator::Initialize(
    mojom::VideoDecodeAcceleratorConfigPtr config,
    mojom::VideoDecodeClientPtr client,
    InitializeCallback callback) {
  VLOGF(2) << "profile = " << config->profile
           << ", secure_mode = " << config->secure_mode;
  DCHECK_CALLED_ON_VALID_THREAD(thread_checker_);
  CHECK(!client_);
  client_ = std::move(client);

  auto result = InitializeTask(std::move(config));

  // Report initialization status to UMA.
  const int RESULT_MAX =
      static_cast<int>(mojom::VideoDecodeAccelerator::Result::RESULT_MAX);
  UMA_HISTOGRAM_ENUMERATION(
      "Media.GpuArcVideoDecodeAccelerator.InitializeResult",
      static_cast<int>(result), RESULT_MAX);
  std::move(callback).Run(result);
}

mojom::VideoDecodeAccelerator::Result
GpuArcVideoDecodeAccelerator::InitializeTask(
    mojom::VideoDecodeAcceleratorConfigPtr config) {
  if (vda_) {
    VLOGF(1) << "Re-initialization not allowed.";
    return mojom::VideoDecodeAccelerator::Result::ILLEGAL_STATE;
  }

  if (client_count_ >= kMaxConcurrentClients) {
    VLOGF(1) << "Reject to Initialize() due to too many clients: "
             << client_count_;
    return mojom::VideoDecodeAccelerator::Result::INSUFFICIENT_RESOURCES;
  }

  if (config->secure_mode && !protected_buffer_manager_) {
    VLOGF(1) << "Secure mode unsupported";
    return mojom::VideoDecodeAccelerator::Result::PLATFORM_FAILURE;
  }

  media::VideoDecodeAccelerator::Config vda_config(config->profile);
  vda_config.output_mode =
      media::VideoDecodeAccelerator::Config::OutputMode::IMPORT;

  auto vda_factory = media::GpuVideoDecodeAcceleratorFactory::CreateWithNoGL();
  vda_ = vda_factory->CreateVDA(
      this, vda_config, gpu::GpuDriverBugWorkarounds(), gpu_preferences_);
  if (!vda_) {
    VLOGF(1) << "Failed to create VDA.";
    return mojom::VideoDecodeAccelerator::Result::PLATFORM_FAILURE;
  }

  client_count_++;
  secure_mode_ = config->secure_mode;
  error_state_ = false;
  pending_requests_ = {};
  pending_flush_callbacks_ = {};
  pending_reset_callback_.Reset();
  protected_input_handles_.clear();
  protected_output_handles_.clear();
  VLOGF(2) << "Number of concurrent clients: " << client_count_;
  return mojom::VideoDecodeAccelerator::Result::SUCCESS;
}

void GpuArcVideoDecodeAccelerator::AllocateProtectedBuffer(
    mojo::ScopedHandle handle,
    uint64_t size,
    AllocateProtectedBufferCallback callback) {
  VLOGF(2);
  DCHECK_CALLED_ON_VALID_THREAD(thread_checker_);
  if (!secure_mode_) {
    VLOGF(1) << "Not in secure mode.";
    std::move(callback).Run(false);
    return;
  }
  if (protected_input_handles_.size() >= kMaxProtectedInputBuffers) {
    VLOGF(1) << "Too many protected input buffers"
             << ", kMaxProtectedInputBuffers=" << kMaxProtectedInputBuffers;
    std::move(callback).Run(false);
    return;
  }

  base::ScopedFD fd = UnwrapFdFromMojoHandle(std::move(handle));
  if (!fd.is_valid()) {
    std::move(callback).Run(false);
    return;
  }
  VLOGF(2) << " fd=" << fd.get() << " size=" << size;

  auto protected_shmem =
      protected_buffer_manager_->AllocateProtectedSharedMemory(std::move(fd),
                                                               size);
  if (!protected_shmem) {
    VLOGF(1) << "Failed allocating protected shared memory.";
    std::move(callback).Run(false);
    return;
  }
  protected_input_handles_.emplace_back(std::move(protected_shmem));
  std::move(callback).Run(true);
}

void GpuArcVideoDecodeAccelerator::Decode(
    mojom::BitstreamBufferPtr bitstream_buffer) {
  DVLOGF(4);
  DCHECK_CALLED_ON_VALID_THREAD(thread_checker_);
  if (!vda_) {
    VLOGF(1) << "VDA not initialized.";
    return;
  }

  base::ScopedFD handle_fd =
      UnwrapFdFromMojoHandle(std::move(bitstream_buffer->handle_fd));
  if (!handle_fd.is_valid()) {
    client_->NotifyError(
        mojom::VideoDecodeAccelerator::Result::INVALID_ARGUMENT);
    return;
  }
  DVLOGF(4) << "fd=" << handle_fd.get();

  base::SharedMemoryHandle shm_handle;
  if (secure_mode_) {
    // Use protected shared memory associated with the given file descriptor.
    shm_handle = protected_buffer_manager_->GetProtectedSharedMemoryHandleFor(
        std::move(handle_fd));
    if (!shm_handle.IsValid()) {
      VLOGF(1) << "No protected shared memory found for handle";
      client_->NotifyError(
          mojom::VideoDecodeAccelerator::Result::INVALID_ARGUMENT);
      return;
    }
  } else {
    shm_handle = base::SharedMemoryHandle(
        base::FileDescriptor(handle_fd.release(), true), 0u,
        base::UnguessableToken::Create());
  }

  // Use Unretained(this) is safe, this callback will be executed in
  // ExecuteRequest().
  // This callback is immediately called or stored into a member variable.
  // All the callbacks thus should be called or deleted before |this| is
  // invalidated.
  ExecuteRequest(
      {base::BindOnce(
           &GpuArcVideoDecodeAccelerator::DecodeRequest, base::Unretained(this),
           ScopedBitstreamBuffer(media::BitstreamBuffer(
               bitstream_buffer->bitstream_id, shm_handle,
               bitstream_buffer->bytes_used, bitstream_buffer->offset))),
       PendingCallback()});
}

void GpuArcVideoDecodeAccelerator::AssignPictureBuffers(uint32_t count) {
  VLOGF(2) << "count=" << count;
  DCHECK_CALLED_ON_VALID_THREAD(thread_checker_);
  if (!vda_) {
    VLOGF(1) << "VDA not initialized.";
    return;
  }
  if (count > kMaxOutputBufferCount) {
    VLOGF(1) << "Too many output buffers requested"
             << ", count =" << count;
    client_->NotifyError(
        mojom::VideoDecodeAccelerator::Result::INVALID_ARGUMENT);
    return;
  }

  coded_size_ = pending_coded_size_;
  std::vector<media::PictureBuffer> buffers;
  for (uint32_t id = 0; id < count; ++id) {
    buffers.push_back(
        media::PictureBuffer(static_cast<int32_t>(id), coded_size_));
  }
  if (secure_mode_) {
    protected_output_handles_.clear();
    protected_output_handles_.resize(static_cast<size_t>(count));
  }
  output_buffer_count_ = static_cast<size_t>(count);
  vda_->AssignPictureBuffers(buffers);
}

void GpuArcVideoDecodeAccelerator::ImportBufferForPicture(
    int32_t picture_buffer_id,
    mojom::HalPixelFormat format,
    mojo::ScopedHandle handle,
    std::vector<VideoFramePlane> planes) {
  // TODO(owenlin): Implement this function.
  ImportBufferForPictureDeprecated(picture_buffer_id, std::move(handle),
                                   std::move(planes));
}

void GpuArcVideoDecodeAccelerator::ImportBufferForPictureDeprecated(
    int32_t picture_buffer_id,
    mojo::ScopedHandle handle,
    std::vector<VideoFramePlane> planes) {
  DVLOGF(3);
  DCHECK_CALLED_ON_VALID_THREAD(thread_checker_);
  if (!vda_) {
    VLOGF(1) << "VDA not initialized.";
    return;
  }
  if (picture_buffer_id < 0 ||
      static_cast<size_t>(picture_buffer_id) >= output_buffer_count_) {
    VLOGF(1) << "Invalid picture_buffer_id=" << picture_buffer_id;
    client_->NotifyError(
        mojom::VideoDecodeAccelerator::Result::INVALID_ARGUMENT);
    return;
  }

  base::ScopedFD handle_fd = UnwrapFdFromMojoHandle(std::move(handle));
  if (!handle_fd.is_valid()) {
    client_->NotifyError(
        mojom::VideoDecodeAccelerator::Result::INVALID_ARGUMENT);
    return;
  }

  gfx::GpuMemoryBufferHandle gmb_handle;
  gmb_handle.type = gfx::NATIVE_PIXMAP;
  if (secure_mode_) {
    DCHECK_EQ(protected_output_handles_.size(), output_buffer_count_);
    VLOGF(2) << "Allocate output protected buffer"
             << ", picture_buffer_id=" << picture_buffer_id;
    auto protected_pixmap =
        protected_buffer_manager_->AllocateProtectedNativePixmap(
            std::move(handle_fd),
            media::VideoPixelFormatToGfxBufferFormat(output_pixel_format_),
            coded_size_);
    if (!protected_pixmap) {
      VLOGF(1) << "Failed allocating protected pixmap.";
      client_->NotifyError(
          mojom::VideoDecodeAccelerator::Result::PLATFORM_FAILURE);
      return;
    }
    gmb_handle.native_pixmap_handle =
        gfx::CloneHandleForIPC(protected_pixmap->native_pixmap_handle());
    protected_output_handles_[picture_buffer_id] = std::move(protected_pixmap);
  } else {
    if (!VerifyDmabuf(handle_fd.get(), planes)) {
      VLOGF(1) << "Failed verifying dmabuf";
      client_->NotifyError(
          mojom::VideoDecodeAccelerator::Result::INVALID_ARGUMENT);
      return;
    }
    gmb_handle.native_pixmap_handle.fds.push_back(
        base::FileDescriptor(handle_fd.release(), true));
    for (const auto& plane : planes) {
      gmb_handle.native_pixmap_handle.planes.emplace_back(plane.stride,
                                                          plane.offset, 0, 0);
    }
  }

  vda_->ImportBufferForPicture(picture_buffer_id, gmb_handle);
}

void GpuArcVideoDecodeAccelerator::ReusePictureBuffer(
    int32_t picture_buffer_id) {
  DVLOGF(4);
  DCHECK_CALLED_ON_VALID_THREAD(thread_checker_);
  if (!vda_) {
    VLOGF(1) << "VDA not initialized.";
    return;
  }

  vda_->ReusePictureBuffer(picture_buffer_id);
}

void GpuArcVideoDecodeAccelerator::Flush(FlushCallback callback) {
  VLOGF(2);
  DCHECK_CALLED_ON_VALID_THREAD(thread_checker_);
  // Use Unretained(this) is safe, this callback will be executed in
  // ExecuteRequest().
  // This callback is immediately called or stored into a member variable.
  // All the callbacks thus should be called or deleted before |this| is
  // invalidated.
  ExecuteRequest({base::BindOnce(&GpuArcVideoDecodeAccelerator::FlushRequest,
                                 base::Unretained(this)),
                  std::move(callback)});
}

void GpuArcVideoDecodeAccelerator::Reset(ResetCallback callback) {
  VLOGF(2);
  DCHECK_CALLED_ON_VALID_THREAD(thread_checker_);
  // Use Unretained(this) is safe, this callback will be executed in
  // ExecuteRequest().
  // This callback is immediately called or stored into a member variable.
  // All the callbacks thus should be called or deleted before |this| is
  // invalidated.
  ExecuteRequest({base::BindOnce(&GpuArcVideoDecodeAccelerator::ResetRequest,
                                 base::Unretained(this)),
                  std::move(callback)});
}

bool GpuArcVideoDecodeAccelerator::VerifyDmabuf(
    const int dmabuf_fd,
    const std::vector<VideoFramePlane>& planes) const {
  DCHECK_CALLED_ON_VALID_THREAD(thread_checker_);
  size_t num_planes = media::VideoFrame::NumPlanes(output_pixel_format_);
  if (planes.size() != num_planes) {
    VLOGF(1) << "Invalid number of dmabuf planes passed: " << planes.size()
             << ", expected: " << num_planes;
    return false;
  }

  off_t size = lseek(dmabuf_fd, 0, SEEK_END);
  lseek(dmabuf_fd, 0, SEEK_SET);
  if (size < 0) {
    VPLOGF(1) << "Fail to find the size of dmabuf.";
    return false;
  }

  for (size_t i = 0; i < planes.size(); ++i) {
    const auto& plane = planes[i];

    DVLOGF(4) << "Plane " << i << ", offset: " << plane.offset
              << ", stride: " << plane.stride;

    size_t rows =
        media::VideoFrame::Rows(i, output_pixel_format_, coded_size_.height());
    base::CheckedNumeric<off_t> current_size(plane.offset);
    current_size += base::CheckMul(plane.stride, rows);

    if (!current_size.IsValid() || current_size.ValueOrDie() > size) {
      VLOGF(1) << "Invalid strides/offsets.";
      return false;
    }
  }

  return true;
}

}  // namespace arc
