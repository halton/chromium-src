// Copyright 2017 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "chrome/browser/chromeos/printing/cups_printers_manager.h"

#include <algorithm>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <utility>
#include <vector>

#include "base/memory/singleton.h"
#include "base/observer_list.h"
#include "base/scoped_observer.h"
#include "base/sequence_checker.h"
#include "chrome/browser/chromeos/printing/ppd_provider_factory.h"
#include "chrome/browser/chromeos/printing/printer_event_tracker_factory.h"
#include "chrome/browser/chromeos/printing/synced_printers_manager.h"
#include "chrome/browser/chromeos/printing/synced_printers_manager_factory.h"
#include "chrome/browser/chromeos/printing/usb_printer_detector.h"
#include "chrome/browser/chromeos/printing/zeroconf_printer_detector.h"
#include "chrome/browser/profiles/profile.h"

namespace chromeos {
namespace {

class CupsPrintersManagerImpl;

// Since CupsPrintersManager listens to multiple PrinterDetectors, we need to
// disambiguate incoming observer calls based on their source, and so can't
// implement PrinterDetector::Observer directly in CupsPrintersManagerImpl.
class PrinterDetectorObserverProxy : public PrinterDetector::Observer {
 public:
  PrinterDetectorObserverProxy(CupsPrintersManagerImpl* parent,
                               int id,
                               PrinterDetector* detector)
      : parent_(parent), id_(id), observer_(this) {
    observer_.Add(detector);
  }
  ~PrinterDetectorObserverProxy() override = default;

  // Defined out of line because we need the CupsPrintersManagerImpl
  // definition first.
  void OnPrintersFound(
      const std::vector<PrinterDetector::DetectedPrinter>& printers) override;

  // We don't do anything with OnPrinterScanComplete() at the moment, so just
  // stub it out.
  void OnPrinterScanComplete() override {}

 private:
  CupsPrintersManagerImpl* parent_;
  int id_;
  ScopedObserver<PrinterDetector, PrinterDetector::Observer> observer_;
};

// This is akin to python's filter() builtin, but with reverse polarity on the
// test function -- *remove* all entries in printers for which test_fn returns
// true, discard the rest.
void FilterOutPrinters(std::vector<Printer>* printers,
                       std::function<bool(const Printer&)> test_fn) {
  auto new_end = std::remove_if(printers->begin(), printers->end(), test_fn);
  printers->resize(new_end - printers->begin());
}

// Return true if this is a USB printer.
bool IsUsbPrinter(const Printer& printer) {
  return base::StringPiece(printer.uri()).starts_with("usb://");
}

class CupsPrintersManagerImpl : public CupsPrintersManager,
                                public SyncedPrintersManager::Observer {
 public:
  // Identifiers for each of the underlying PrinterDetectors this
  // class observes.
  enum DetectorIds {
    kUsbDetector,
    kZeroconfDetector,
  };

  CupsPrintersManagerImpl(SyncedPrintersManager* synced_printers_manager,
                          std::unique_ptr<PrinterDetector> usb_detector,
                          std::unique_ptr<PrinterDetector> zeroconf_detector,
                          scoped_refptr<PpdProvider> ppd_provider,
                          PrinterEventTracker* event_tracker)
      : synced_printers_manager_(synced_printers_manager),
        synced_printers_manager_observer_(this),
        started_observing_(false),
        usb_detector_(std::move(usb_detector)),
        zeroconf_detector_(std::move(zeroconf_detector)),
        ppd_provider_(std::move(ppd_provider)),
        event_tracker_(event_tracker),
        printers_(kNumPrinterClasses),
        weak_ptr_factory_(this) {
    // Prime the printer cache with the configured and enterprise printers.
    printers_[kConfigured] = synced_printers_manager_->GetConfiguredPrinters();
    RebuildConfiguredPrintersIndex();
    printers_[kEnterprise] = synced_printers_manager_->GetEnterprisePrinters();
    synced_printers_manager_observer_.Add(synced_printers_manager_);

    // Callbacks may ensue immediately when the observer proxies are set up, so
    // these instantiations must come after everything else is initialized.
    usb_detector_observer_proxy_ =
        base::MakeUnique<PrinterDetectorObserverProxy>(this, kUsbDetector,
                                                       usb_detector_.get());
    zeroconf_detector_observer_proxy_ =
        base::MakeUnique<PrinterDetectorObserverProxy>(
            this, kZeroconfDetector, zeroconf_detector_.get());
  }

  ~CupsPrintersManagerImpl() override = default;

  // Public API function.
  std::vector<Printer> GetPrinters(PrinterClass printer_class) const override {
    DCHECK_CALLED_ON_VALID_SEQUENCE(sequence_);
    return printers_.at(printer_class);
  }

  // Public API function.
  void RemoveUnavailablePrinters(
      std::vector<Printer>* printers) const override {
    DCHECK_CALLED_ON_VALID_SEQUENCE(sequence_);
    FilterOutPrinters(printers, [this](const Printer& printer) {
      return !PrinterAvailable(printer);
    });
  }

  // Public API function.
  void UpdateConfiguredPrinter(const Printer& printer) override {
    DCHECK_CALLED_ON_VALID_SEQUENCE(sequence_);
    // If this is an 'add' instead of just an update, record the event.
    MaybeRecordInstallation(printer);
    synced_printers_manager_->UpdateConfiguredPrinter(printer);
    // Note that we will rebuild our lists when we get the observer
    // callback from |synced_printers_manager_|.
  }

  // Public API function.
  void RemoveConfiguredPrinter(const std::string& printer_id) override {
    DCHECK_CALLED_ON_VALID_SEQUENCE(sequence_);
    auto existing = synced_printers_manager_->GetPrinter(printer_id);
    if (existing != nullptr) {
      event_tracker_->RecordPrinterRemoved(*existing);
    }
    synced_printers_manager_->RemoveConfiguredPrinter(printer_id);
    // Note that we will rebuild our lists when we get the observer
    // callback from |synced_printers_manager_|.
  }

  // Public API function.
  void AddObserver(CupsPrintersManager::Observer* observer) override {
    DCHECK_CALLED_ON_VALID_SEQUENCE(sequence_);
    observer_list_.AddObserver(observer);
  }

  // Public API function.
  void RemoveObserver(CupsPrintersManager::Observer* observer) override {
    DCHECK_CALLED_ON_VALID_SEQUENCE(sequence_);
    observer_list_.RemoveObserver(observer);
  }

  // Used to initiate the callbacks after the observers have been constructed.
  void Start() override {
    if (!started_observing_) {
      usb_detector_->StartObservers();
      zeroconf_detector_->StartObservers();
      started_observing_ = true;
    }
  }

  // Public API function.
  void PrinterInstalled(const Printer& printer) override {
    DCHECK_CALLED_ON_VALID_SEQUENCE(sequence_);
    MaybeRecordInstallation(printer);
    synced_printers_manager_->PrinterInstalled(printer);
  }

  // Public API function.
  bool IsPrinterInstalled(const Printer& printer) const override {
    DCHECK_CALLED_ON_VALID_SEQUENCE(sequence_);
    return synced_printers_manager_->IsConfigurationCurrent(printer);
  }

  // Public API function.
  // Note this is linear in the number of printers.  If the number of printers
  // gets so large that a linear search is prohibative, we'll have to rethink
  // more than just this function.
  std::unique_ptr<Printer> GetPrinter(const std::string& id) const override {
    DCHECK_CALLED_ON_VALID_SEQUENCE(sequence_);
    for (const auto& printer_list : printers_) {
      for (const auto& printer : printer_list) {
        if (printer.id() == id) {
          return base::MakeUnique<Printer>(printer);
        }
      }
    }
    return std::unique_ptr<Printer>();
  }

  // SyncedPrintersManager::Observer implementation
  void OnConfiguredPrintersChanged(
      const std::vector<Printer>& printers) override {
    DCHECK_CALLED_ON_VALID_SEQUENCE(sequence_);
    printers_[kConfigured] = printers;
    RebuildConfiguredPrintersIndex();
    RebuildDetectedLists();
    UpdateConfiguredPrinterURIs();
    NotifyObservers({kConfigured});
  }

  // SyncedPrintersManager::Observer implementation
  void OnEnterprisePrintersChanged(
      const std::vector<Printer>& printers) override {
    DCHECK_CALLED_ON_VALID_SEQUENCE(sequence_);
    printers_[kEnterprise] = printers;
    NotifyObservers({kEnterprise});
  }

  // Callback entry point for PrinterDetectorObserverProxys owned by this
  // object.
  void OnPrintersFound(
      int detector_id,
      const std::vector<PrinterDetector::DetectedPrinter>& printers) {
    DCHECK_CALLED_ON_VALID_SEQUENCE(sequence_);
    switch (detector_id) {
      case kUsbDetector:
        usb_detections_ = printers;
        break;
      case kZeroconfDetector:
        zeroconf_detections_ = printers;
        break;
    }
    RebuildDetectedLists();

    // We may have new URI information for a configured printer in the changed
    // detected list.  If we do, pass the updated information along to
    // observers.
    if (UpdateConfiguredPrinterURIs()) {
      NotifyObservers({kConfigured});
    }
  }

 private:
  // Notify observers on the given classes the the relevant lists have changed.
  void NotifyObservers(
      const std::vector<CupsPrintersManager::PrinterClass>& printer_classes) {
    for (auto& observer : observer_list_) {
      for (auto printer_class : printer_classes) {
        observer.OnPrintersChanged(printer_class, printers_[printer_class]);
      }
    }
  }

  // Rebuild the index from printer id to index for configured printers.
  void RebuildConfiguredPrintersIndex() {
    DCHECK_CALLED_ON_VALID_SEQUENCE(sequence_);
    configured_printers_index_.clear();
    for (size_t i = 0; i < printers_[kConfigured].size(); ++i) {
      configured_printers_index_[printers_[kConfigured][i].id()] = i;
    }
  }

  // Cross reference the Configured printers with the raw detected printer
  // lists.  Returns true if any entries in the configured printers list
  // changed as a result of this cross referencing, false otherwise.
  bool UpdateConfiguredPrinterURIs() {
    DCHECK_CALLED_ON_VALID_SEQUENCE(sequence_);
    bool updated = false;
    for (const auto* printer_list : {&usb_detections_, &zeroconf_detections_}) {
      for (const auto& detected : *printer_list) {
        auto configured =
            configured_printers_index_.find(detected.printer.id());
        if (configured != configured_printers_index_.end()) {
          Printer* configured_printer =
              &printers_[kConfigured][configured->second];
          if (configured_printer->effective_uri() !=
              detected.printer.effective_uri()) {
            configured_printer->set_effective_uri(
                detected.printer.effective_uri());
            updated = true;
          }
        }
      }
    }
    return updated;
  }

  // Look through all sources for the detected printer with the given id.
  // Return a pointer to the printer on found, null if no entry is found.
  const PrinterDetector::DetectedPrinter* FindDetectedPrinter(
      const std::string& id) const {
    DCHECK_CALLED_ON_VALID_SEQUENCE(sequence_);
    for (const auto* printer_list : {&usb_detections_, &zeroconf_detections_}) {
      for (const auto& detected : *printer_list) {
        if (detected.printer.id() == id) {
          return &detected;
        }
      }
    }
    return nullptr;
  }

  void MaybeRecordInstallation(const Printer& printer) {
    DCHECK_CALLED_ON_VALID_SEQUENCE(sequence_);
    if (synced_printers_manager_->GetPrinter(printer.id()) != nullptr) {
      // It's just an update, not a new installation, so don't record an event.
      return;
    }
    // Get the associated detection record if one exists.
    const auto* detected = FindDetectedPrinter(printer.id());

    // For compatibility with the previous implementation, record USB printers
    // separately from other IPP printers.  Eventually we may want to shift
    // this to be split by autodetected/not autodetected instead of USB/other
    // IPP.
    if (IsUsbPrinter(printer)) {
      // If it's a usb printer, we should have the full DetectedPrinter.  We
      // can't log the printer if we don't have it.
      if (detected == nullptr) {
        LOG(WARNING) << "Failed to find USB printer " << printer.id()
                     << " for installation event logging";
        return;
      }
      // For recording purposes, this is an automatic install if the ppd
      // reference generated at detection time is the is the one we actually
      // used -- i.e. the user didn't have to change anything to obtain a ppd
      // that worked.
      PrinterEventTracker::SetupMode mode;
      if (printer.ppd_reference() == detected->printer.ppd_reference()) {
        mode = PrinterEventTracker::kAutomatic;
      } else {
        mode = PrinterEventTracker::kUser;
      }
      event_tracker_->RecordUsbPrinterInstalled(*detected, mode);
    } else {
      PrinterEventTracker::SetupMode mode;
      if (detected != nullptr &&
          (detected->printer.ppd_reference().autoconf ||
           (detected->printer.ppd_reference() == printer.ppd_reference()))) {
        // A printer is automatic if we successfully used IPP Anywhere or we
        // auto-grabbed a ppd reference that worked.
        mode = PrinterEventTracker::kAutomatic;
      } else {
        mode = PrinterEventTracker::kUser;
      }
      event_tracker_->RecordIppPrinterInstalled(printer, mode);
    }
  }

  // Return whether or not we believe this printer is currently available for
  // printing.  This is not a perfect test -- we just assume any IPP printers
  // are available because, in cases where there are a large number of
  // printers available, probing IPP printers would generate too much network
  // spam.  This is intended to help filter out local printers that are not
  // available (USB, zeroconf, ...)
  //
  // TODO(justincarlson) - Implement this.  Until it's implemented, we'll never
  // filter out unavailable printers from potential printer targets.  While
  // suboptimal, this is ok--it just means that we will fail to print if the
  // user selects a printer that's not available.
  bool PrinterAvailable(const Printer& printer) const { return true; }

  void AddDetectedList(
      const std::vector<PrinterDetector::DetectedPrinter>& detected_list) {
    DCHECK_CALLED_ON_VALID_SEQUENCE(sequence_);
    for (const PrinterDetector::DetectedPrinter& detected : detected_list) {
      if (base::ContainsKey(configured_printers_index_,
                            detected.printer.id())) {
        // It's already in the configured class, don't need to do anything
        // else here.
        continue;
      }

      // Sometimes the detector can flag a printer as IPP-everywhere compatible;
      // those printers can go directly into the automatic class without further
      // processing.
      if (detected.printer.IsIppEverywhere()) {
        printers_[kAutomatic].push_back(detected.printer);
        continue;
      }
      auto it = detected_printer_ppd_references_.find(detected.printer.id());
      if (it != detected_printer_ppd_references_.end()) {
        if (it->second == nullptr) {
          // We couldn't figure out this printer, so it's in the discovered
          // class.
          printers_[kDiscovered].push_back(detected.printer);
        } else {
          // We have a ppd reference, so we think we can set this up
          // automatically.
          printers_[kAutomatic].push_back(detected.printer);
          *printers_[kAutomatic].back().mutable_ppd_reference() = *it->second;
        }
      } else {
        // Didn't find an entry for this printer in the PpdReferences cache.  We
        // need to ask PpdProvider whether or not it can determine a
        // PpdReference.  If there's not already an outstanding request for one,
        // start one.  When the request comes back, we'll rerun classification
        // and then should be able to figure out where this printer belongs.
        if (!base::ContainsKey(inflight_ppd_reference_resolutions_,
                               detected.printer.id())) {
          inflight_ppd_reference_resolutions_.insert(detected.printer.id());
          ppd_provider_->ResolvePpdReference(
              detected.ppd_search_data,
              base::Bind(&CupsPrintersManagerImpl::ResolvePpdReferenceDone,
                         weak_ptr_factory_.GetWeakPtr(),
                         detected.printer.id()));
        }
      }
    }
  }

  // Record in UMA the appropriate event with a setup attempt for a printer is
  // abandoned.
  void RecordSetupAbandoned(const Printer& printer) override {
    DCHECK_CALLED_ON_VALID_SEQUENCE(sequence_);
    if (IsUsbPrinter(printer)) {
      const auto* detected = FindDetectedPrinter(printer.id());
      if (detected == nullptr) {
        LOG(WARNING) << "Failed to find USB printer " << printer.id()
                     << " for abandoned event logging";
        return;
      }
      event_tracker_->RecordUsbSetupAbandoned(*detected);
    } else {
      event_tracker_->RecordSetupAbandoned(printer);
    }
  }

  // Rebuild the Automatic and Discovered printers lists from the (cached) raw
  // detections.  This will also generate OnPrintersChanged events for any
  // observers observering either of the detected lists (kAutomatic and
  // kDiscovered).
  void RebuildDetectedLists() {
    DCHECK_CALLED_ON_VALID_SEQUENCE(sequence_);
    printers_[kAutomatic].clear();
    printers_[kDiscovered].clear();
    AddDetectedList(usb_detections_);
    AddDetectedList(zeroconf_detections_);
    NotifyObservers({kAutomatic, kDiscovered});
  }

  // Callback invoked on completion of PpdProvider::ResolvePpdReference.
  void ResolvePpdReferenceDone(const std::string& printer_id,
                               PpdProvider::CallbackResultCode code,
                               const Printer::PpdReference& ref) {
    DCHECK_CALLED_ON_VALID_SEQUENCE(sequence_);
    inflight_ppd_reference_resolutions_.erase(printer_id);
    // Create the entry.
    std::unique_ptr<Printer::PpdReference>& value =
        detected_printer_ppd_references_[printer_id];
    if (code == PpdProvider::SUCCESS) {
      // If we got something, populate the entry.  Otherwise let it
      // just remain null.
      value.reset(new Printer::PpdReference(ref));
    }
    RebuildDetectedLists();
  }

  SEQUENCE_CHECKER(sequence_);

  // Source lists for detected printers.
  std::vector<PrinterDetector::DetectedPrinter> usb_detections_;
  std::vector<PrinterDetector::DetectedPrinter> zeroconf_detections_;

  // Not owned.
  SyncedPrintersManager* synced_printers_manager_;
  ScopedObserver<SyncedPrintersManager, SyncedPrintersManager::Observer>
      synced_printers_manager_observer_;

  // Represents whether or not the Start() method has been called and
  // CupsPrintersManager has started to observe the usb and zeroconf printer
  // detectors. If this is true than subsequent calls to Start() will have no
  // effect.
  bool started_observing_;

  std::unique_ptr<PrinterDetector> usb_detector_;
  std::unique_ptr<PrinterDetectorObserverProxy> usb_detector_observer_proxy_;

  std::unique_ptr<PrinterDetector> zeroconf_detector_;
  std::unique_ptr<PrinterDetectorObserverProxy>
      zeroconf_detector_observer_proxy_;

  scoped_refptr<PpdProvider> ppd_provider_;

  // Not owned
  PrinterEventTracker* event_tracker_;

  // Categorized printers.  This is indexed by PrinterClass.
  std::vector<std::vector<Printer>> printers_;

  // Printer ids that occur in one of our categories or printers.
  std::unordered_set<std::string> known_printer_ids_;

  // This is a dual-purpose structure.  The keys in the map are printer ids.
  // If an entry exists in this map it means we have received a response from
  // PpdProvider about a PpdReference for the given printer.  A null value
  // means we don't have a PpdReference (and so can't set up this printer
  // automatically).
  std::unordered_map<std::string, std::unique_ptr<Printer::PpdReference>>
      detected_printer_ppd_references_;

  // Printer ids for which we have sent off a request to PpdProvider for a ppd
  // reference, but have not yet gotten a response.
  std::unordered_set<std::string> inflight_ppd_reference_resolutions_;

  // Map from printer id to printers_[kConfigured] index for configured
  // printers.
  std::unordered_map<std::string, int> configured_printers_index_;

  base::ObserverList<CupsPrintersManager::Observer> observer_list_;

  base::WeakPtrFactory<CupsPrintersManagerImpl> weak_ptr_factory_;
};

void PrinterDetectorObserverProxy::OnPrintersFound(
    const std::vector<PrinterDetector::DetectedPrinter>& printers) {
  parent_->OnPrintersFound(id_, printers);
}

}  // namespace

// static
std::unique_ptr<CupsPrintersManager> CupsPrintersManager::Create(
    Profile* profile) {
  return base::MakeUnique<CupsPrintersManagerImpl>(
      SyncedPrintersManagerFactory::GetInstance()->GetForBrowserContext(
          profile),
      UsbPrinterDetector::Create(), ZeroconfPrinterDetector::Create(profile),
      CreatePpdProvider(profile),
      PrinterEventTrackerFactory::GetInstance()->GetForBrowserContext(profile));
}

// static
std::unique_ptr<CupsPrintersManager> CupsPrintersManager::Create(
    SyncedPrintersManager* synced_printers_manager,
    std::unique_ptr<PrinterDetector> usb_detector,
    std::unique_ptr<PrinterDetector> zeroconf_detector,
    scoped_refptr<PpdProvider> ppd_provider,
    PrinterEventTracker* event_tracker) {
  return base::MakeUnique<CupsPrintersManagerImpl>(
      synced_printers_manager, std::move(usb_detector),
      std::move(zeroconf_detector), std::move(ppd_provider), event_tracker);
}

}  // namespace chromeos
