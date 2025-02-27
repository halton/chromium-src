// Copyright 2017 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef CHROME_BROWSER_VR_MODEL_MODEL_H_
#define CHROME_BROWSER_VR_MODEL_MODEL_H_

#include "chrome/browser/vr/model/color_scheme.h"
#include "chrome/browser/vr/model/controller_model.h"
#include "chrome/browser/vr/model/modal_prompt_type.h"
#include "chrome/browser/vr/model/omnibox_suggestions.h"
#include "chrome/browser/vr/model/permissions_model.h"
#include "chrome/browser/vr/model/reticle_model.h"
#include "chrome/browser/vr/model/speech_recognition_model.h"
#include "chrome/browser/vr/model/text_input_info.h"
#include "chrome/browser/vr/model/toolbar_state.h"
#include "chrome/browser/vr/model/ui_mode.h"
#include "chrome/browser/vr/model/web_vr_model.h"
#include "chrome/browser/vr/ui_element_renderer.h"
#include "ui/gfx/transform.h"

namespace vr {

struct Model {
  Model();
  ~Model();

  // VR browsing state.
  bool browsing_disabled = false;
  bool loading = false;
  float load_progress = 0.0f;
  bool incognito = false;
  bool in_cct = false;
  bool can_navigate_back = false;
  ToolbarState toolbar_state;
  std::vector<OmniboxSuggestion> omnibox_suggestions;
  SpeechRecognitionModel speech;
  const ColorScheme& color_scheme() const;
  gfx::Transform projection_matrix;
  unsigned int content_texture_id = 0;
  UiElementRenderer::TextureLocation content_location =
      UiElementRenderer::kTextureLocationLocal;
  bool background_available = false;
  bool can_apply_new_background = false;
  bool background_loaded = false;

  // WebVR state.
  WebVrModel web_vr;

  std::vector<UiMode> ui_modes;
  void push_mode(UiMode mode);
  void pop_mode();
  void pop_mode(UiMode mode);
  void toggle_mode(UiMode mode);
  UiMode get_last_opaque_mode() const;
  bool has_mode_in_stack(UiMode mode) const;
  bool browsing_enabled() const;
  bool default_browsing_enabled() const;
  bool voice_search_enabled() const;
  bool omnibox_editing_enabled() const;
  bool fullscreen_enabled() const;
  bool web_vr_enabled() const;
  bool web_vr_autopresentation_enabled() const;
  bool reposition_window_enabled() const;

  // Focused text state.
  bool editing_input = false;
  TextInputInfo omnibox_text_field_info;

  // Controller state.
  ControllerModel controller;
  ReticleModel reticle;

  // State affecting both VR browsing and WebVR.
  ModalPromptType active_modal_prompt_type = kModalPromptTypeNone;
  PermissionsModel permissions;
  bool experimental_features_enabled = false;
  bool skips_redraw_when_not_dirty = false;
  bool exiting_vr = false;
};

}  // namespace vr

#endif  // CHROME_BROWSER_VR_MODEL_MODEL_H_
