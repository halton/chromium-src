// Copyright 2017 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "chrome/browser/vr/ui_scene_creator.h"

#include <memory>

#include "base/bind.h"
#include "base/callback.h"
#include "base/i18n/case_conversion.h"
#include "base/numerics/math_constants.h"
#include "chrome/browser/vr/databinding/binding.h"
#include "chrome/browser/vr/databinding/vector_binding.h"
#include "chrome/browser/vr/elements/audio_permission_prompt.h"
#include "chrome/browser/vr/elements/button.h"
#include "chrome/browser/vr/elements/content_element.h"
#include "chrome/browser/vr/elements/controller.h"
#include "chrome/browser/vr/elements/disc_button.h"
#include "chrome/browser/vr/elements/draw_phase.h"
#include "chrome/browser/vr/elements/environment/background.h"
#include "chrome/browser/vr/elements/environment/grid.h"
#include "chrome/browser/vr/elements/environment/stars.h"
#include "chrome/browser/vr/elements/exit_prompt.h"
#include "chrome/browser/vr/elements/full_screen_rect.h"
#include "chrome/browser/vr/elements/invisible_hit_target.h"
#include "chrome/browser/vr/elements/keyboard.h"
#include "chrome/browser/vr/elements/laser.h"
#include "chrome/browser/vr/elements/linear_layout.h"
#include "chrome/browser/vr/elements/rect.h"
#include "chrome/browser/vr/elements/repositioner.h"
#include "chrome/browser/vr/elements/reticle.h"
#include "chrome/browser/vr/elements/scaled_depth_adjuster.h"
#include "chrome/browser/vr/elements/spinner.h"
#include "chrome/browser/vr/elements/text.h"
#include "chrome/browser/vr/elements/text_input.h"
#include "chrome/browser/vr/elements/throbber.h"
#include "chrome/browser/vr/elements/toast.h"
#include "chrome/browser/vr/elements/transient_element.h"
#include "chrome/browser/vr/elements/ui_element.h"
#include "chrome/browser/vr/elements/ui_element_name.h"
#include "chrome/browser/vr/elements/ui_texture.h"
#include "chrome/browser/vr/elements/url_bar.h"
#include "chrome/browser/vr/elements/vector_icon.h"
#include "chrome/browser/vr/elements/viewport_aware_root.h"
#include "chrome/browser/vr/elements/webvr_url_toast.h"
#include "chrome/browser/vr/keyboard_delegate.h"
#include "chrome/browser/vr/model/model.h"
#include "chrome/browser/vr/speech_recognizer.h"
#include "chrome/browser/vr/target_property.h"
#include "chrome/browser/vr/ui.h"
#include "chrome/browser/vr/ui_browser_interface.h"
#include "chrome/browser/vr/ui_scene.h"
#include "chrome/browser/vr/ui_scene_constants.h"
#include "chrome/browser/vr/vector_icons/vector_icons.h"
#include "chrome/common/chrome_features.h"
#include "chrome/grit/generated_resources.h"
#include "components/vector_icons/vector_icons.h"
#include "ui/base/l10n/l10n_util.h"
#include "ui/gfx/transform_util.h"

namespace vr {

namespace {

template <typename V, typename C, typename S>
void BindColor(Model* model,
               V* view,
               C color,
               const std::string& color_string,
               S setter,
               const std::string& setter_string) {
  view->AddBinding(std::make_unique<Binding<SkColor>>(
      base::BindRepeating([](Model* m, C c) { return (m->color_scheme()).*c; },
                          base::Unretained(model), color),
      color_string,
      base::BindRepeating(
          [](V* v, S s, const SkColor& value) { (v->*s)(value); },
          base::Unretained(view), setter),
      setter_string));
}

#ifndef NDEBUG
#define VR_BIND_COLOR(m, v, c, s) BindColor(m, v, c, #c, s, #s)
#else
#define VR_BIND_COLOR(m, v, c, s) BindColor(m, v, c, "", s, "")
#endif

template <typename V, typename C, typename S>
void BindButtonColors(Model* model,
                      V* view,
                      C colors,
                      const std::string& colors_string,
                      S setter,
                      const std::string& setter_string) {
  view->AddBinding(std::make_unique<Binding<ButtonColors>>(
      base::BindRepeating([](Model* m, C c) { return (m->color_scheme()).*c; },
                          base::Unretained(model), colors),
      colors_string,
      base::BindRepeating(
          [](V* v, S s, const ButtonColors& value) { (v->*s)(value); },
          base::Unretained(view), setter),
      setter_string));
}

#ifndef NDEBUG
#define VR_BIND_BUTTON_COLORS(m, v, c, s) BindButtonColors(m, v, c, #c, s, #s)
#else
#define VR_BIND_BUTTON_COLORS(m, v, c, s) BindButtonColors(m, v, c, "", s, "")
#endif

#define VR_BIND_VISIBILITY(v, c) \
  v->AddBinding(                 \
      VR_BIND_FUNC(bool, Model, model_, c, UiElement, v.get(), SetVisible));

template <typename T, typename... Args>
std::unique_ptr<T> Create(UiElementName name, DrawPhase phase, Args&&... args) {
  auto element = std::make_unique<T>(std::forward<Args>(args)...);
  element->SetName(name);
  element->SetDrawPhase(phase);
  return element;
}

typedef VectorBinding<OmniboxSuggestion, Button> SuggestionSetBinding;
typedef typename SuggestionSetBinding::ElementBinding SuggestionBinding;

void OnSuggestionModelAdded(UiScene* scene,
                            UiBrowserInterface* browser,
                            Ui* ui,
                            Model* model,
                            SuggestionBinding* element_binding) {
  auto icon = std::make_unique<VectorIcon>(100);
  icon->SetDrawPhase(kPhaseForeground);
  icon->SetType(kTypeOmniboxSuggestionIcon);
  icon->set_hit_testable(false);
  icon->SetSize(kSuggestionIconSizeDMM, kSuggestionIconSizeDMM);
  VR_BIND_COLOR(model, icon.get(), &ColorScheme::omnibox_icon,
                &VectorIcon::SetColor);
  VectorIcon* p_icon = icon.get();

  auto icon_box = std::make_unique<UiElement>();
  icon_box->SetDrawPhase(kPhaseNone);
  icon_box->SetType(kTypeOmniboxSuggestionIconField);
  icon_box->SetSize(kSuggestionIconFieldWidthDMM, kSuggestionHeightDMM);
  icon_box->AddChild(std::move(icon));

  auto content_text = std::make_unique<Text>(kSuggestionContentTextHeightDMM);
  content_text->SetDrawPhase(kPhaseForeground);
  content_text->SetType(kTypeOmniboxSuggestionContentText);
  content_text->set_hit_testable(false);
  content_text->SetLayoutMode(TextLayoutMode::kSingleLineFixedWidth);
  content_text->SetSize(kSuggestionTextFieldWidthDMM, 0);
  content_text->SetAlignment(UiTexture::kTextAlignmentLeft);
  VR_BIND_COLOR(model, content_text.get(),
                &ColorScheme::omnibox_suggestion_content, &Text::SetColor);
  Text* p_content_text = content_text.get();

  auto description_text =
      std::make_unique<Text>(kSuggestionDescriptionTextHeightDMM);
  description_text->SetDrawPhase(kPhaseForeground);
  description_text->SetType(kTypeOmniboxSuggestionDescriptionText);
  description_text->set_hit_testable(false);
  description_text->SetLayoutMode(TextLayoutMode::kSingleLineFixedWidth);
  description_text->SetSize(kSuggestionTextFieldWidthDMM, 0);
  description_text->SetAlignment(UiTexture::kTextAlignmentLeft);
  VR_BIND_COLOR(model, description_text.get(),
                &ColorScheme::omnibox_suggestion_description, &Text::SetColor);
  Text* p_description_text = description_text.get();

  auto text_layout = std::make_unique<LinearLayout>(LinearLayout::kDown);
  text_layout->SetType(kTypeOmniboxSuggestionTextLayout);
  text_layout->set_hit_testable(false);
  text_layout->set_margin(kSuggestionLineGapDMM);
  text_layout->AddChild(std::move(content_text));
  text_layout->AddChild(std::move(description_text));

  auto right_margin = std::make_unique<UiElement>();
  right_margin->SetDrawPhase(kPhaseNone);
  right_margin->SetSize(kSuggestionRightMarginDMM, kSuggestionHeightDMM);

  auto suggestion_layout = std::make_unique<LinearLayout>(LinearLayout::kRight);
  suggestion_layout->SetType(kTypeOmniboxSuggestionLayout);
  suggestion_layout->set_hit_testable(false);
  suggestion_layout->AddChild(std::move(icon_box));
  suggestion_layout->AddChild(std::move(text_layout));
  suggestion_layout->AddChild(std::move(right_margin));

  auto background = Create<Button>(
      kNone, kPhaseForeground,
      base::BindRepeating(
          [](UiBrowserInterface* b, Ui* ui, Model* m, SuggestionBinding* e) {
            b->Navigate(e->model()->destination);
            ui->OnUiRequestedNavigation();
          },
          base::Unretained(browser), base::Unretained(ui),
          base::Unretained(model), base::Unretained(element_binding)));

  background->SetType(kTypeOmniboxSuggestionBackground);
  background->set_hit_testable(true);
  background->set_bubble_events(true);
  background->set_bounds_contain_children(true);
  background->set_hover_offset(0.0);
  VR_BIND_BUTTON_COLORS(model, background.get(),
                        &ColorScheme::suggestion_button_colors,
                        &Button::SetButtonColors);
  background->AddChild(std::move(suggestion_layout));

  element_binding->bindings().push_back(
      VR_BIND_FUNC(base::string16, SuggestionBinding, element_binding,
                   model->model()->content, Text, p_content_text, SetText));
  element_binding->bindings().push_back(
      std::make_unique<Binding<base::string16>>(
          VR_BIND_LAMBDA(
              [](SuggestionBinding* m) { return m->model()->description; },
              base::Unretained(element_binding)),
          VR_BIND_LAMBDA(
              [](Text* v, const base::string16& text) {
                v->SetVisibleImmediately(!text.empty());
                v->set_requires_layout(!text.empty());
                if (!text.empty()) {
                  v->SetText(text);
                }
              },
              base::Unretained(p_description_text))));
  element_binding->bindings().push_back(
      VR_BIND(AutocompleteMatch::Type, SuggestionBinding, element_binding,
              model->model()->type, VectorIcon, p_icon,
              view->SetIcon(AutocompleteMatch::TypeToVectorIcon(value))));
  element_binding->set_view(background.get());
  scene->AddUiElement(kOmniboxSuggestions, std::move(background));
}

void OnSuggestionModelRemoved(UiScene* scene, SuggestionBinding* binding) {
  scene->RemoveUiElement(binding->view()->id());
}

std::unique_ptr<TransientElement> CreateTransientParent(UiElementName name,
                                                        int timeout_seconds,
                                                        bool animate_opacity) {
  auto element = std::make_unique<SimpleTransientElement>(
      base::TimeDelta::FromSeconds(timeout_seconds));
  element->SetName(name);
  element->SetVisible(false);
  element->set_hit_testable(false);
  if (animate_opacity)
    element->SetTransitionedProperties({OPACITY});
  return element;
}

std::unique_ptr<Rect> CreateOmniboxSpacer(Model* model) {
  auto spacer = Create<Rect>(kNone, kPhaseForeground);
  spacer->SetType(kTypeOmniboxSuggestionSpacer);
  spacer->SetSize(kOmniboxWidthDMM, kSuggestionVerticalPaddingDMM);
  spacer->AddBinding(std::make_unique<Binding<bool>>(
      VR_BIND_LAMBDA([](Model* m) { return !m->omnibox_suggestions.empty(); },
                     base::Unretained(model)),
      VR_BIND_LAMBDA(
          [](UiElement* e, const bool& v) {
            e->SetVisible(v);
            e->set_requires_layout(v);
          },
          base::Unretained(spacer.get()))));
  VR_BIND_COLOR(model, spacer.get(), &ColorScheme::omnibox_background,
                &Rect::SetColor);
  return spacer;
}

// Util to bind the visibility of the given control element to the given
// property in the model and the visibility of the voice search UI root.
#define BIND_VISIBILITY_CONTROL_FOR_VOICE(control_element, model, property) \
  control_element->AddBinding(std::make_unique<Binding<bool>>(              \
      VR_BIND_LAMBDA(                                                       \
          [](Model* model, UiElement* voice_search_root) {                  \
            return model->property &&                                       \
                   voice_search_root->GetTargetOpacity() == 0.f;            \
          },                                                                \
          base::Unretained(model),                                          \
          base::Unretained(                                                 \
              scene_->GetUiElementByName(kSpeechRecognitionRoot))),         \
      VR_BIND_LAMBDA([](UiElement* control,                                 \
                        const bool& value) { control->SetVisible(value); }, \
                     base::Unretained(control_element))))

std::unique_ptr<UiElement> CreateSpacer(float width, float height) {
  auto spacer = Create<UiElement>(kNone, kPhaseNone);
  spacer->SetSize(width, height);
  spacer->set_hit_testable(false);
  return spacer;
}

std::unique_ptr<UiElement> CreateSnackbar(
    UiElementName name,
    Model* model,
    const gfx::VectorIcon& vector_icon,
    const base::string16& label,
    const base::string16& button_label,
    base::RepeatingCallback<void()> callback) {
  auto scaler = std::make_unique<ScaledDepthAdjuster>(kSnackbarDistance);
  scaler->set_hit_testable(false);

  auto snackbar_layout =
      Create<LinearLayout>(name, kPhaseNone, LinearLayout::kRight);
  snackbar_layout->SetTranslate(0, kSnackbarDistance * sin(kSnackbarAngle), 0);
  snackbar_layout->SetRotate(1, 0, 0, kSnackbarAngle);
  snackbar_layout->set_hit_testable(false);

  float radius = 0.5f * kSnackbarHeightDMM;
  auto snackbar_oval_left = Create<Rect>(kNone, kPhaseForeground);
  snackbar_oval_left->SetType(kTypeSnackbarDescription);
  snackbar_oval_left->SetSize(0, kSnackbarHeightDMM);
  snackbar_oval_left->SetCornerRadii({radius, 0, radius, 0});
  snackbar_oval_left->set_owner_name_for_test(name);
  VR_BIND_COLOR(model, snackbar_oval_left.get(),
                &ColorScheme::snackbar_background, &Rect::SetColor);

  auto snackbar_inner_layout =
      Create<LinearLayout>(kNone, kPhaseNone, LinearLayout::kRight);
  snackbar_inner_layout->set_margin(kSnackbarPaddingDMM * 0.5f);
  snackbar_inner_layout->set_hit_testable(false);
  snackbar_oval_left->AddBinding(VR_BIND(
      float, UiElement, snackbar_inner_layout.get(),
      model->stale_size().width() + kSnackbarPaddingDMM, UiElement,
      snackbar_oval_left.get(), view->SetSize(value, kSnackbarHeightDMM)));

  auto icon = Create<VectorIcon>(kNone, kPhaseForeground, 256);
  icon->SetSize(kSnackbarIconWidthDMM, kSnackbarIconWidthDMM);
  icon->SetBackgroundColor(SK_ColorTRANSPARENT);
  icon->SetIcon(vector_icon);
  icon->set_hit_testable(false);
  VR_BIND_COLOR(model, icon.get(), &ColorScheme::snackbar_foreground,
                &VectorIcon::SetColor);

  auto text = Create<Text>(kNone, kPhaseForeground, kSnackbarFontHeightDMM);
  text->SetText(label);
  text->SetLayoutMode(TextLayoutMode::kSingleLineFixedHeight);
  text->set_hit_testable(false);
  VR_BIND_COLOR(model, text.get(), &ColorScheme::snackbar_foreground,
                &Text::SetColor);

  auto button = Create<Button>(kNone, kPhaseForeground, callback);
  button->SetType(kTypeSnackbarButton);
  VR_BIND_BUTTON_COLORS(model, button.get(),
                        &ColorScheme::snackbar_button_colors,
                        &Button::SetButtonColors);
  button->set_hover_offset(0.0f);
  button->SetCornerRadii({0, radius, 0, radius});
  button->SetSize(0, kSnackbarHeightDMM);
  button->set_owner_name_for_test(name);
  button->set_hit_testable(true);

  auto button_layout =
      Create<LinearLayout>(kNone, kPhaseNone, LinearLayout::kRight);
  button_layout->set_hit_testable(false);

  auto button_text =
      Create<Text>(kNone, kPhaseForeground, kSnackbarFontHeightDMM);
  button_text->SetText(button_label);
  button_text->SetLayoutMode(TextLayoutMode::kSingleLineFixedHeight);
  button_text->set_hit_testable(false);
  button_text->AddBinding(
      VR_BIND_FUNC(SkColor, Model, model,
                   model->color_scheme().snackbar_button_colors.foreground,
                   Text, button_text.get(), SetColor));

  button->AddBinding(VR_BIND(float, UiElement, button_layout.get(),
                             model->stale_size().width() + kSnackbarPaddingDMM,
                             UiElement, button.get(),
                             view->SetSize(value, kSnackbarHeightDMM)));

  button_layout->AddChild(std::move(button_text));
  button_layout->AddChild(CreateSpacer(0.5f * kSnackbarPaddingDMM, 0.0f));

  button->AddChild(std::move(button_layout));

  snackbar_inner_layout->AddChild(CreateSpacer(0.0f, 0.0f));
  snackbar_inner_layout->AddChild(std::move(icon));
  snackbar_inner_layout->AddChild(std::move(text));
  snackbar_oval_left->AddChild(std::move(snackbar_inner_layout));

  snackbar_layout->AddChild(std::move(snackbar_oval_left));
  snackbar_layout->AddChild(std::move(button));

  scaler->AddChild(std::move(snackbar_layout));
  auto snackbar_root = Create<UiElement>(kNone, kPhaseNone);
  snackbar_root->set_hit_testable(false);
  snackbar_root->AddChild(std::move(scaler));
  return snackbar_root;
}

}  // namespace

UiSceneCreator::UiSceneCreator(UiBrowserInterface* browser,
                               UiScene* scene,
                               Ui* ui,
                               ContentInputDelegate* content_input_delegate,
                               KeyboardDelegate* keyboard_delegate,
                               TextInputDelegate* text_input_delegate,
                               Model* model)
    : browser_(browser),
      scene_(scene),
      ui_(ui),
      content_input_delegate_(content_input_delegate),
      keyboard_delegate_(keyboard_delegate),
      text_input_delegate_(text_input_delegate),
      model_(model) {}

UiSceneCreator::~UiSceneCreator() {}

void UiSceneCreator::CreateScene() {
  Create2dBrowsingSubtreeRoots();
  CreateWebVrRoot();
  CreateBackground();
  CreateViewportAwareRoot();
  CreateContentQuad();
  CreateExitPrompt();
  CreateAudioPermissionPrompt();
  CreateSystemIndicators();
  CreateUrlBar();
  CreateLoadingIndicator();
  CreateSnackbars();
  CreateOmnibox();
  CreateCloseButton();
  CreateFullscreenToast();
  CreateUnderDevelopmentNotice();
  CreateVoiceSearchUiGroup();
  CreateExitWarning();
  CreateWebVrSubtree();
  CreateKeyboard();
  CreateController();
}

void UiSceneCreator::Create2dBrowsingSubtreeRoots() {
  auto element = Create<UiElement>(k2dBrowsingRoot, kPhaseNone);
  element->set_hit_testable(false);
  element->AddBinding(std::make_unique<Binding<bool>>(
      VR_BIND_LAMBDA(
          [](Model* m) {
            bool ready = !m->background_available ||
                         (m->background_available && m->background_loaded);
            return m->browsing_enabled() && ready;
          },
          base::Unretained(model_)),
      VR_BIND_LAMBDA([](UiElement* e, const bool& v) { e->SetVisible(v); },
                     base::Unretained(element.get()))));

  scene_->AddUiElement(kRoot, std::move(element));

  element = Create<UiElement>(k2dBrowsingBackground, kPhaseNone);
  element->set_hit_testable(false);
  scene_->AddUiElement(k2dBrowsingRoot, std::move(element));

  auto repositioner = std::make_unique<Repositioner>(kContentDistance);
  repositioner->SetName(k2dBrowsingRepositioner);
  repositioner->AddBinding(
      VR_BIND_FUNC(bool, Model, model_, model->reposition_window_enabled(),
                   Repositioner, repositioner.get(), set_enable));
  repositioner->AddBinding(
      VR_BIND_FUNC(gfx::Point3F, Model, model_, model->controller.laser_origin,
                   Repositioner, repositioner.get(), set_laser_origin));
  repositioner->AddBinding(VR_BIND_FUNC(
      gfx::Vector3dF, Model, model_, model->controller.laser_direction,
      Repositioner, repositioner.get(), set_laser_direction));
  scene_->AddUiElement(k2dBrowsingRoot, std::move(repositioner));

  element = Create<UiElement>(k2dBrowsingVisibiltyControlForVoice, kPhaseNone);
  element->set_hit_testable(false);
  scene_->AddUiElement(k2dBrowsingRepositioner, std::move(element));

  element = Create<UiElement>(k2dBrowsingVisibiltyControlForSiteInfoPrompt,
                              kPhaseNone);
  element->set_hit_testable(false);
  element->AddBinding(VR_BIND_FUNC(
      bool, Model, model_,
      model->active_modal_prompt_type != kModalPromptTypeExitVRForSiteInfo,
      UiElement, element.get(), SetVisible));
  scene_->AddUiElement(k2dBrowsingVisibiltyControlForVoice, std::move(element));

  element = Create<UiElement>(k2dBrowsingOpacityControlForAudioPermissionPrompt,
                              kPhaseNone);
  element->set_hit_testable(false);
  element->AddBinding(
      VR_BIND(bool, Model, model_,
              model->active_modal_prompt_type !=
                  kModalPromptTypeExitVRForVoiceSearchRecordAudioOsPermission,
              UiElement, element.get(),
              view->SetOpacity(value ? 1.0 : kModalPromptFadeOpacity)));
  scene_->AddUiElement(k2dBrowsingVisibiltyControlForSiteInfoPrompt,
                       std::move(element));

  element = Create<UiElement>(k2dBrowsingForeground, kPhaseNone);
  element->set_hit_testable(false);
  element->SetTransitionedProperties({OPACITY});
  element->SetTransitionDuration(base::TimeDelta::FromMilliseconds(
      kSpeechRecognitionOpacityAnimationDurationMs));
  VR_BIND_VISIBILITY(element, model->default_browsing_enabled() ||
                                  model->fullscreen_enabled());
  scene_->AddUiElement(k2dBrowsingOpacityControlForAudioPermissionPrompt,
                       std::move(element));

  element = Create<UiElement>(k2dBrowsingContentGroup, kPhaseNone);
  element->SetTranslate(0, kContentVerticalOffset, -kContentDistance);
  element->SetSize(kContentWidth, kContentHeight);
  element->set_hit_testable(false);
  element->SetTransitionedProperties({TRANSFORM});
  element->AddBinding(
      VR_BIND(bool, Model, model_, model->fullscreen_enabled(), UiElement,
              element.get(),
              view->SetTranslate(
                  0, value ? kFullscreenVerticalOffset : kContentVerticalOffset,
                  value ? -kFullscreenDistance : -kContentDistance)));
  scene_->AddUiElement(k2dBrowsingForeground, std::move(element));
}

void UiSceneCreator::CreateWebVrRoot() {
  auto element = std::make_unique<UiElement>();
  element->SetName(kWebVrRoot);
  element->set_hit_testable(false);
  VR_BIND_VISIBILITY(element, model->web_vr_enabled());
  scene_->AddUiElement(kRoot, std::move(element));
}

void UiSceneCreator::CreateExitWarning() {
  auto scrim = std::make_unique<FullScreenRect>();
  scrim->SetName(kScreenDimmer);
  scrim->SetDrawPhase(kPhaseForeground);
  scrim->SetVisible(false);
  scrim->set_hit_testable(false);
  scrim->SetOpacity(kScreenDimmerOpacity);
  scrim->SetCenterColor(model_->color_scheme().dimmer_inner);
  scrim->SetEdgeColor(model_->color_scheme().dimmer_outer);
  VR_BIND_VISIBILITY(scrim, model->exiting_vr);
  scene_->AddUiElement(k2dBrowsingViewportAwareRoot, std::move(scrim));

  // Create transient exit warning.
  auto scaler = std::make_unique<ScaledDepthAdjuster>(kExitWarningDistance);
  auto exit_warning_text = std::make_unique<Text>(kExitWarningFontHeightDMM);
  exit_warning_text->SetName(kExitWarningText);
  exit_warning_text->SetDrawPhase(kPhaseForeground);
  exit_warning_text->SetText(
      l10n_util::GetStringUTF16(IDS_VR_BROWSER_UNSUPPORTED_PAGE));
  exit_warning_text->SetSize(kExitWarningTextWidthDMM, 0);
  exit_warning_text->SetVisible(true);
  exit_warning_text->set_hit_testable(false);
  VR_BIND_COLOR(model_, exit_warning_text.get(),
                &ColorScheme::exit_warning_foreground, &Text::SetColor);

  auto exit_warning_bg = std::make_unique<Rect>();
  exit_warning_bg->SetName(kExitWarningBackground);
  exit_warning_bg->SetDrawPhase(kPhaseForeground);
  exit_warning_bg->set_bounds_contain_children(true);
  exit_warning_bg->set_padding(kExitWarningXPaddingDMM,
                               kExitWarningYPaddingDMM);
  exit_warning_bg->set_corner_radius(kExitWarningCornerRadiusDMM);
  exit_warning_bg->set_hit_testable(false);
  exit_warning_bg->AddChild(std::move(exit_warning_text));
  VR_BIND_VISIBILITY(exit_warning_bg, model->exiting_vr);
  VR_BIND_COLOR(model_, exit_warning_bg.get(),
                &ColorScheme::exit_warning_background, &Rect::SetColor);
  scaler->AddChild(std::move(exit_warning_bg));
  scene_->AddUiElement(k2dBrowsingViewportAwareRoot, std::move(scaler));
}

void UiSceneCreator::CreateSystemIndicators() {
  struct Indicator {
    UiElementName name;
    const gfx::VectorIcon& icon;
    int resource_string;
    bool PermissionsModel::*signal;
  };
  const std::vector<Indicator> indicators = {
      {kAudioCaptureIndicator, vector_icons::kMicrophoneIcon,
       IDS_AUDIO_CALL_NOTIFICATION_TEXT_2,
       &PermissionsModel::audio_capture_enabled},
      {kVideoCaptureIndicator, vector_icons::kVideocamIcon,
       IDS_VIDEO_CALL_NOTIFICATION_TEXT_2,
       &PermissionsModel::video_capture_enabled},
      {kScreenCaptureIndicator, vector_icons::kScreenShareIcon,
       IDS_SCREEN_CAPTURE_NOTIFICATION_TEXT_2,
       &PermissionsModel::screen_capture_enabled},
      {kBluetoothConnectedIndicator, vector_icons::kBluetoothConnectedIcon, 0,
       &PermissionsModel::bluetooth_connected},
      {kLocationAccessIndicator, vector_icons::kLocationOnIcon, 0,
       &PermissionsModel::location_access},
  };

  std::unique_ptr<LinearLayout> indicator_layout =
      std::make_unique<LinearLayout>(LinearLayout::kRight);
  indicator_layout->SetName(kIndicatorLayout);
  indicator_layout->set_hit_testable(false);
  indicator_layout->set_y_anchoring(TOP);
  indicator_layout->SetTranslate(0, kIndicatorVerticalOffset,
                                 kIndicatorDistanceOffset);
  indicator_layout->set_margin(kIndicatorGap);
  VR_BIND_VISIBILITY(indicator_layout, !model->fullscreen_enabled());

  for (const auto& indicator : indicators) {
    auto element = std::make_unique<Toast>();
    element->SetName(indicator.name);
    element->SetDrawPhase(kPhaseForeground);
    element->set_padding(kIndicatorXPadding, kIndicatorYPadding);
    element->set_corner_radius(kIndicatorCornerRadius);
    element->SetMargin(kIndicatorMargin);
    element->AddIcon(indicator.icon, 64, kIndicatorIconSize);
    if (indicator.resource_string != 0) {
      element->AddText(l10n_util::GetStringUTF16(indicator.resource_string),
                       kIndicatorFontHeightDmm,
                       TextLayoutMode::kSingleLineFixedHeight);
    }

    VR_BIND_COLOR(model_, element.get(),
                  &ColorScheme::system_indicator_background,
                  &Toast::SetBackgroundColor);
    VR_BIND_COLOR(model_, element.get(),
                  &ColorScheme::system_indicator_foreground,
                  &Toast::SetForegroundColor);
    element->AddBinding(std::make_unique<Binding<bool>>(
        VR_BIND_LAMBDA(
            [](Model* m, bool PermissionsModel::*permission) {
              return m->permissions.*permission;
            },
            base::Unretained(model_), indicator.signal),
        VR_BIND_LAMBDA(
            [](UiElement* e, const bool& v) {
              e->SetVisible(v);
              e->set_requires_layout(v);
            },
            base::Unretained(element.get()))));

    indicator_layout->AddChild(std::move(element));
  }
  scene_->AddUiElement(k2dBrowsingContentGroup, std::move(indicator_layout));
}

void UiSceneCreator::CreateContentQuad() {
  // Place an invisible but hittable plane behind the content quad, to keep the
  // reticle roughly planar with the content if near content.
  auto hit_plane = std::make_unique<InvisibleHitTarget>();
  hit_plane->SetName(kBackplane);
  hit_plane->SetDrawPhase(kPhaseForeground);
  hit_plane->SetSize(kBackplaneSize, kSceneHeight);
  scene_->AddUiElement(k2dBrowsingContentGroup, std::move(hit_plane));

  auto shadow = Create<Shadow>(kContentQuadShadow, kPhaseForeground);
  shadow->set_intensity(kContentShadowIntesity);
  shadow->SetTranslate(0, 0, -kContentShadowOffset);

  auto main_content = std::make_unique<ContentElement>(
      content_input_delegate_,
      base::BindRepeating(&UiBrowserInterface::OnContentScreenBoundsChanged,
                          base::Unretained(browser_)));
  main_content->SetName(kContentQuad);
  main_content->SetDrawPhase(kPhaseForeground);
  main_content->SetSize(kContentWidth, kContentHeight);
  main_content->set_corner_radius(kContentCornerRadius);
  main_content->SetTranslate(0, 0, kContentShadowOffset);
  main_content->SetTransitionedProperties({BOUNDS});
  main_content->AddBinding(
      VR_BIND(bool, Model, model_, model->fullscreen_enabled(), UiElement,
              main_content.get(),
              view->SetSize(value ? kFullscreenWidth : kContentWidth,
                            value ? kFullscreenHeight : kContentHeight)));
  main_content->AddBinding(
      VR_BIND_FUNC(gfx::Transform, Model, model_, model->projection_matrix,
                   ContentElement, main_content.get(), SetProjectionMatrix));
  main_content->AddBinding(
      VR_BIND_FUNC(unsigned int, Model, model_, model->content_texture_id,
                   ContentElement, main_content.get(), SetTextureId));
  main_content->AddBinding(VR_BIND_FUNC(UiElementRenderer::TextureLocation,
                                        Model, model_, model->content_location,
                                        ContentElement, main_content.get(),
                                        SetTextureLocation));

  shadow->AddChild(std::move(main_content));
  scene_->AddUiElement(k2dBrowsingContentGroup, std::move(shadow));

  // Limit reticle distance to a sphere based on maximum content distance.
  scene_->set_background_distance(kFullscreenDistance *
                                  kBackgroundDistanceMultiplier);
}

void UiSceneCreator::CreateWebVrSubtree() {
  CreateSplashScreenForDirectWebVrLaunch();
  CreateWebVrOverlayElements();
  CreateWebVrTimeoutScreen();

  // Note, this cannot be a descendant of the viewport aware root, otherwise it
  // will fade out when the viewport aware elements reposition.
  auto bg = std::make_unique<FullScreenRect>();
  bg->SetName(kWebVrBackground);
  bg->SetDrawPhase(kPhaseBackground);
  bg->SetVisible(false);
  bg->set_hit_testable(false);
  bg->SetColor(model_->color_scheme().web_vr_background);
  bg->SetTransitionedProperties({OPACITY});
  VR_BIND_VISIBILITY(
      bg, model->web_vr_enabled() && !model->web_vr.has_produced_frames());
  scene_->AddUiElement(kWebVrRoot, std::move(bg));
}

void UiSceneCreator::CreateSplashScreenForDirectWebVrLaunch() {
  // Create transient parent.
  // TODO(crbug.com/762074): We should timeout after some time and show an
  // error if the user is stuck on the splash screen.
  auto transient_parent = std::make_unique<ShowUntilSignalTransientElement>(
      base::TimeDelta::FromSeconds(kSplashScreenMinDurationSeconds),
      base::TimeDelta::Max(),
      base::BindRepeating(
          [](Model* model, UiBrowserInterface* browser,
             TransientElementHideReason reason) {
            if (reason == TransientElementHideReason::kTimeout) {
              browser->ExitPresent();
            }
          },
          base::Unretained(model_), base::Unretained(browser_)));
  transient_parent->SetName(kSplashScreenTransientParent);
  transient_parent->set_hit_testable(false);
  transient_parent->SetTransitionedProperties({OPACITY});
  VR_BIND_VISIBILITY(transient_parent,
                     model->web_vr_autopresentation_enabled());
  transient_parent->AddBinding(VR_BIND_FUNC(
      bool, Model, model_,
      model->web_vr_autopresentation_enabled() &&
          model->web_vr.state > kWebVrAwaitingFirstFrame,
      ShowUntilSignalTransientElement, transient_parent.get(), Signal));
  scene_->AddUiElement(kWebVrViewportAwareRoot, std::move(transient_parent));

  // Add "Running in Chrome" text.
  auto text_scaler =
      std::make_unique<ScaledDepthAdjuster>(kSplashScreenTextDistance);
  auto text = std::make_unique<Text>(kSplashScreenTextFontHeightDMM);
  VR_BIND_COLOR(model_, text.get(), &ColorScheme::splash_screen_text_color,
                &Text::SetColor);
  text->SetText(l10n_util::GetStringUTF16(IDS_VR_RUNNING_IN_CHROME_MESSAGE));
  text->SetName(kSplashScreenText);
  text->SetDrawPhase(kPhaseForeground);
  text->set_hit_testable(false);
  text->SetSize(kSplashScreenTextWidthDMM, 0);
  text->SetTranslate(0, kSplashScreenTextVerticalOffsetDMM, 0);
  text_scaler->AddChild(std::move(text));
  scene_->AddUiElement(kSplashScreenTransientParent, std::move(text_scaler));
}

void UiSceneCreator::CreateWebVrTimeoutScreen() {
  auto scaler = std::make_unique<ScaledDepthAdjuster>(kTimeoutScreenDisatance);
  scaler->SetName(kWebVrTimeoutRoot);
  scaler->AddBinding(std::make_unique<Binding<bool>>(
      VR_BIND_LAMBDA(
          [](Model* model, UiElement* splash_screen) {
            // The timeout UI should only be visible when the splash screen is
            // not visible.
            return (model->web_vr.state == kWebVrTimeoutImminent ||
                    model->web_vr.state == kWebVrTimedOut) &&
                   splash_screen->GetTargetOpacity() == 0.f;
          },
          base::Unretained(model_),
          base::Unretained(
              scene_->GetUiElementByName(kSplashScreenTransientParent))),
      VR_BIND_LAMBDA(
          [](UiElement* e, const bool& value) { e->SetVisible(value); },
          base::Unretained(scaler.get()))));

  auto spinner = std::make_unique<Spinner>(512);
  spinner->SetName(kWebVrTimeoutSpinner);
  spinner->SetDrawPhase(kPhaseForeground);
  spinner->SetTransitionedProperties({OPACITY});
  spinner->SetVisible(false);
  spinner->SetSize(kTimeoutSpinnerSizeDMM, kTimeoutSpinnerSizeDMM);
  spinner->SetTranslate(0, kTimeoutSpinnerVerticalOffsetDMM, 0);
  spinner->SetColor(model_->color_scheme().web_vr_timeout_spinner);
  VR_BIND_VISIBILITY(spinner, model->web_vr.state == kWebVrTimeoutImminent);

  auto timeout_message = Create<Rect>(kWebVrTimeoutMessage, kPhaseForeground);
  timeout_message->SetVisible(false);
  timeout_message->set_bounds_contain_children(true);
  timeout_message->set_corner_radius(kTimeoutMessageCornerRadiusDMM);
  timeout_message->SetTransitionedProperties({OPACITY, TRANSFORM});
  timeout_message->set_padding(kTimeoutMessageHorizontalPaddingDMM,
                               kTimeoutMessageVerticalPaddingDMM);
  VR_BIND_VISIBILITY(timeout_message, model->web_vr.state == kWebVrTimedOut);
  timeout_message->SetColor(
      model_->color_scheme().web_vr_timeout_message_background);

  auto timeout_layout = Create<LinearLayout>(kWebVrTimeoutMessageLayout,
                                             kPhaseNone, LinearLayout::kRight);
  timeout_layout->set_hit_testable(false);
  timeout_layout->set_margin(kTimeoutMessageLayoutGapDMM);

  auto timeout_icon =
      Create<VectorIcon>(kWebVrTimeoutMessageIcon, kPhaseForeground, 512);
  timeout_icon->SetIcon(kSadTabIcon);
  timeout_icon->SetSize(kTimeoutMessageIconWidthDMM,
                        kTimeoutMessageIconHeightDMM);

  auto timeout_text = Create<Text>(kWebVrTimeoutMessageText, kPhaseForeground,
                                   kTimeoutMessageTextFontHeightDMM);
  timeout_text->SetText(
      l10n_util::GetStringUTF16(IDS_VR_WEB_VR_TIMEOUT_MESSAGE));
  timeout_text->SetColor(
      model_->color_scheme().web_vr_timeout_message_foreground);
  timeout_text->SetAlignment(UiTexture::kTextAlignmentLeft);
  timeout_text->SetSize(kTimeoutMessageTextWidthDMM,
                        kTimeoutMessageTextHeightDMM);

  auto button_scaler =
      std::make_unique<ScaledDepthAdjuster>(kTimeoutButtonDepthOffset);

  auto button =
      Create<DiscButton>(kWebVrTimeoutMessageButton, kPhaseForeground,
                         base::BindRepeating(&UiBrowserInterface::ExitPresent,
                                             base::Unretained(browser_)),
                         vector_icons::kClose16Icon);
  button->SetVisible(false);
  button->SetTranslate(0, -kTimeoutMessageTextWidthDMM, 0);
  button->SetRotate(1, 0, 0, kTimeoutButtonRotationRad);
  button->SetTransitionedProperties({OPACITY});
  button->SetSize(kWebVrTimeoutMessageButtonDiameterDMM,
                  kWebVrTimeoutMessageButtonDiameterDMM);
  VR_BIND_VISIBILITY(button, model->web_vr.state == kWebVrTimedOut);
  VR_BIND_BUTTON_COLORS(model_, button.get(), &ColorScheme::button_colors,
                        &DiscButton::SetButtonColors);

  auto timeout_button_text =
      Create<Text>(kWebVrTimeoutMessageButtonText, kPhaseForeground,
                   kTimeoutMessageTextFontHeightDMM);

  // Disk-style button text is not uppercase. See crbug.com/787654.
  timeout_button_text->SetText(
      l10n_util::GetStringUTF16(IDS_VR_WEB_VR_EXIT_BUTTON_LABEL));
  timeout_button_text->SetColor(model_->color_scheme().web_vr_timeout_spinner);
  timeout_button_text->SetSize(kTimeoutButtonTextWidthDMM,
                               kTimeoutButtonTextHeightDMM);
  timeout_button_text->set_y_anchoring(BOTTOM);
  timeout_button_text->SetTranslate(0, -kTimeoutButtonTextVerticalOffsetDMM, 0);

  button->AddChild(std::move(timeout_button_text));
  timeout_layout->AddChild(std::move(timeout_icon));
  timeout_layout->AddChild(std::move(timeout_text));
  timeout_message->AddChild(std::move(timeout_layout));
  button_scaler->AddChild(std::move(button));
  timeout_message->AddChild(std::move(button_scaler));
  scaler->AddChild(std::move(timeout_message));
  scaler->AddChild(std::move(spinner));
  scene_->AddUiElement(kWebVrViewportAwareRoot, std::move(scaler));
}

void UiSceneCreator::CreateUnderDevelopmentNotice() {
  auto text = std::make_unique<Text>(kUnderDevelopmentNoticeFontHeightDMM);
  VR_BIND_COLOR(model_, text.get(), &ColorScheme::world_background_text,
                &Text::SetColor);
  text->SetText(l10n_util::GetStringUTF16(IDS_VR_UNDER_DEVELOPMENT_NOTICE));
  text->SetName(kUnderDevelopmentNotice);
  text->SetDrawPhase(kPhaseForeground);
  text->set_hit_testable(false);
  text->SetSize(kUnderDevelopmentNoticeWidthDMM,
                kUnderDevelopmentNoticeHeightDMM);
  text->SetTranslate(0, -kUnderDevelopmentNoticeVerticalOffsetDMM, 0);
  text->SetRotate(1, 0, 0, kUnderDevelopmentNoticeRotationRad);
  text->set_y_anchoring(BOTTOM);
  text->set_contributes_to_parent_bounds(false);
  scene_->AddUiElement(kUrlBar, std::move(text));
}

void UiSceneCreator::CreateBackground() {
  // Textured background.
  auto background =
      Create<Background>(k2dBrowsingTexturedBackground, kPhaseBackground);
  background->SetVisible(true);
  VR_BIND_VISIBILITY(background, model->background_loaded);
  background->AddBinding(
      VR_BIND_FUNC(float, Model, model_, model->color_scheme().normal_factor,
                   Background, background.get(), SetNormalFactor));
  background->AddBinding(
      VR_BIND_FUNC(float, Model, model_, model->color_scheme().incognito_factor,
                   Background, background.get(), SetIncognitoFactor));
  background->AddBinding(VR_BIND_FUNC(
      float, Model, model_, model->color_scheme().fullscreen_factor, Background,
      background.get(), SetFullscreenFactor));
  scene_->AddUiElement(k2dBrowsingBackground, std::move(background));

  auto element = Create<UiElement>(k2dBrowsingDefaultBackground, kPhaseNone);
  element->set_hit_testable(false);
  VR_BIND_VISIBILITY(element, !model->background_loaded);
  scene_->AddUiElement(k2dBrowsingBackground, std::move(element));

  // Background solid-color panels.
  struct Panel {
    UiElementName name;
    int x_offset;
    int y_offset;
    int z_offset;
    int x_rotation;
    int y_rotation;
    int angle;
  };
  const std::vector<Panel> panels = {
      {kBackgroundFront, 0, 0, -1, 0, 1, 0},
      {kBackgroundLeft, -1, 0, 0, 0, 1, 1},
      {kBackgroundBack, 0, 0, 1, 0, 1, 2},
      {kBackgroundRight, 1, 0, 0, 0, 1, 3},
      {kBackgroundTop, 0, 1, 0, 1, 0, 1},
      {kBackgroundBottom, 0, -1, 0, 1, 0, -1},
  };
  for (auto& panel : panels) {
    auto panel_element = Create<Rect>(panel.name, kPhaseBackground);
    panel_element->SetSize(kSceneSize, kSceneSize);
    panel_element->SetTranslate(panel.x_offset * kSceneSize / 2,
                                panel.y_offset * kSceneSize / 2,
                                panel.z_offset * kSceneSize / 2);
    panel_element->SetRotate(panel.x_rotation, panel.y_rotation, 0,
                             base::kPiFloat / 2 * panel.angle);
    panel_element->set_hit_testable(false);
    VR_BIND_COLOR(model_, panel_element.get(), &ColorScheme::world_background,
                  &Rect::SetColor);
    VR_BIND_VISIBILITY(panel_element, model->browsing_enabled());
    scene_->AddUiElement(k2dBrowsingDefaultBackground,
                         std::move(panel_element));
  }

  auto stars = Create<Stars>(kStars, kPhaseBackground);
  stars->SetRotate(1, 0, 0, base::kPiFloat * 0.5);
  scene_->AddUiElement(k2dBrowsingTexturedBackground, std::move(stars));

  auto grid = Create<Grid>(kNone, kPhaseBackground);
  grid->SetSize(kSceneSize, kSceneSize);
  grid->SetTranslate(0.0, -kSceneHeight / 2, 0.0);
  grid->SetRotate(1, 0, 0, -base::kPiFloat / 2);
  grid->set_gridline_count(kFloorGridlineCount);
  grid->SetEdgeColor(SK_ColorTRANSPARENT);
  grid->SetCenterColor(SK_ColorTRANSPARENT);
  grid->SetGridColor(model_->color_scheme().floor_grid);
  grid->set_hit_testable(false);
  grid->SetOpacity(kGridOpacity);
  scene_->AddUiElement(k2dBrowsingTexturedBackground, std::move(grid));

  auto floor = Create<Grid>(kFloor, kPhaseBackground);
  floor->SetSize(kSceneSize, kSceneSize);
  floor->SetTranslate(0.0, -kSceneHeight / 2, 0.0);
  floor->SetRotate(1, 0, 0, -base::kPiFloat / 2);
  floor->set_gridline_count(kFloorGridlineCount);
  floor->set_focusable(false);
  VR_BIND_COLOR(model_, floor.get(), &ColorScheme::floor,
                &Grid::SetCenterColor);
  VR_BIND_COLOR(model_, floor.get(), &ColorScheme::world_background,
                &Grid::SetEdgeColor);
  VR_BIND_COLOR(model_, floor.get(), &ColorScheme::floor_grid,
                &Grid::SetGridColor);
  scene_->AddUiElement(k2dBrowsingDefaultBackground, std::move(floor));

  // Ceiling.
  auto ceiling = Create<Rect>(kCeiling, kPhaseBackground);
  ceiling->set_focusable(false);
  ceiling->SetSize(kSceneSize, kSceneSize);
  ceiling->SetTranslate(0.0, kSceneHeight / 2, 0.0);
  ceiling->SetRotate(1, 0, 0, base::kPiFloat / 2);
  VR_BIND_COLOR(model_, ceiling.get(), &ColorScheme::ceiling,
                &Rect::SetCenterColor);
  VR_BIND_COLOR(model_, ceiling.get(), &ColorScheme::world_background,
                &Rect::SetEdgeColor);
  scene_->AddUiElement(k2dBrowsingDefaultBackground, std::move(ceiling));
}

void UiSceneCreator::CreateViewportAwareRoot() {
  auto element = std::make_unique<ViewportAwareRoot>();
  element->SetName(kWebVrViewportAwareRoot);
  element->set_hit_testable(false);
  scene_->AddUiElement(kWebVrRoot, std::move(element));

  element = std::make_unique<ViewportAwareRoot>();
  element->SetName(k2dBrowsingViewportAwareRoot);
  element->set_hit_testable(false);
  scene_->AddUiElement(k2dBrowsingRepositioner, std::move(element));
}

void UiSceneCreator::CreateVoiceSearchUiGroup() {
  auto voice_search_button = Create<DiscButton>(
      kVoiceSearchButton, kPhaseForeground,
      base::BindRepeating(&UiBrowserInterface::SetVoiceSearchActive,
                          base::Unretained(browser_), true),
      vector_icons::kMicrophoneIcon);
  voice_search_button->SetSize(kVoiceSearchButtonDiameterDMM,
                               kVoiceSearchButtonDiameterDMM);
  voice_search_button->set_hover_offset(kButtonZOffsetHoverDMM);
  voice_search_button->SetTranslate(0.f, -kVoiceSearchButtonYOffsetDMM, 0.f);
  voice_search_button->set_y_anchoring(BOTTOM);
  voice_search_button->set_y_centering(TOP);
  voice_search_button->set_contributes_to_parent_bounds(false);
  VR_BIND_VISIBILITY(
      voice_search_button,
      model->speech.has_or_can_request_audio_permission && !model->incognito);
  VR_BIND_BUTTON_COLORS(model_, voice_search_button.get(),
                        &ColorScheme::button_colors,
                        &DiscButton::SetButtonColors);
  scene_->AddUiElement(kUrlBar, std::move(voice_search_button));

  auto speech_recognition_root = std::make_unique<UiElement>();
  speech_recognition_root->SetName(kSpeechRecognitionRoot);
  speech_recognition_root->SetTranslate(0.f, 0.f, -kContentDistance);
  speech_recognition_root->set_hit_testable(false);
  speech_recognition_root->SetTransitionedProperties({OPACITY});
  speech_recognition_root->SetTransitionDuration(
      base::TimeDelta::FromMilliseconds(
          kSpeechRecognitionOpacityAnimationDurationMs));
  // Set initial visibility so we don't see the voice search ui fade out.
  speech_recognition_root->SetVisibleImmediately(false);
  scene_->AddUiElement(k2dBrowsingRepositioner,
                       std::move(speech_recognition_root));

  auto inner_circle = std::make_unique<Rect>();
  inner_circle->SetName(kSpeechRecognitionCircle);
  inner_circle->SetDrawPhase(kPhaseForeground);
  inner_circle->SetSize(kCloseButtonDiameter * 2, kCloseButtonDiameter * 2);
  inner_circle->set_corner_radius(kCloseButtonDiameter);
  inner_circle->set_hit_testable(false);
  VR_BIND_COLOR(model_, inner_circle.get(),
                &ColorScheme::speech_recognition_circle_background,
                &Rect::SetColor);
  scene_->AddUiElement(kSpeechRecognitionRoot, std::move(inner_circle));

  auto microphone_icon = std::make_unique<VectorIcon>(512);
  microphone_icon->SetIcon(vector_icons::kMicrophoneIcon);
  microphone_icon->SetName(kSpeechRecognitionMicrophoneIcon);
  microphone_icon->SetDrawPhase(kPhaseForeground);
  microphone_icon->set_hit_testable(false);
  microphone_icon->SetSize(kCloseButtonDiameter, kCloseButtonDiameter);
  scene_->AddUiElement(kSpeechRecognitionRoot, std::move(microphone_icon));

  auto speech_result_parent = CreateTransientParent(
      kSpeechRecognitionResult, kSpeechRecognitionResultTimeoutSeconds, false);

  // We need to explicitly set the initial visibility of
  // kSpeechRecognitionResult as k2dBrowsingForeground's visibility depends on
  // it in a binding. However, k2dBrowsingForeground's binding updated before
  // kSpeechRecognitionResult. So the initial value needs to be correctly set
  // instead of depend on binding to kick in.
  speech_result_parent->SetVisibleImmediately(false);
  speech_result_parent->SetTransitionedProperties({OPACITY});
  speech_result_parent->SetTransitionDuration(base::TimeDelta::FromMilliseconds(
      kSpeechRecognitionOpacityAnimationDurationMs));
  speech_result_parent->AddBinding(std::make_unique<Binding<bool>>(
      VR_BIND_LAMBDA(
          [](Model* m) { return m->speech.recognition_result.empty(); },
          base::Unretained(model_)),
      VR_BIND_LAMBDA(
          [](UiElement* e, const bool& v) {
            if (v) {
              e->SetVisible(false);
            } else {
              e->SetVisibleImmediately(true);
            }
          },
          speech_result_parent.get())));
  auto speech_result =
      std::make_unique<Text>(kVoiceSearchRecognitionResultTextHeight);
  speech_result->SetName(kSpeechRecognitionResultText);
  speech_result->SetDrawPhase(kPhaseForeground);
  speech_result->SetTranslate(0.f, kSpeechRecognitionResultTextYOffset, 0.f);
  speech_result->set_hit_testable(false);
  speech_result->SetSize(kVoiceSearchRecognitionResultTextWidth, 0);
  speech_result->SetAlignment(UiTexture::kTextAlignmentCenter);
  VR_BIND_COLOR(model_, speech_result.get(), &ColorScheme::prompt_foreground,
                &Text::SetColor);
  speech_result->AddBinding(VR_BIND_FUNC(base::string16, Model, model_,
                                         model->speech.recognition_result, Text,
                                         speech_result.get(), SetText));
  speech_result_parent->AddChild(std::move(speech_result));

  auto hit_target = std::make_unique<InvisibleHitTarget>();
  hit_target->SetName(kSpeechRecognitionResultBackplane);
  hit_target->SetDrawPhase(kPhaseForeground);
  hit_target->SetSize(kPromptBackplaneSize, kPromptBackplaneSize);
  speech_result_parent->AddChild(std::move(hit_target));

  auto speech_recognition_listening = std::make_unique<UiElement>();
  UiElement* listening_ui_root = speech_recognition_listening.get();
  speech_recognition_listening->SetName(kSpeechRecognitionListening);
  speech_recognition_listening->set_hit_testable(false);
  // We need to explicitly set the initial visibility of this element for the
  // same reason as kSpeechRecognitionResult.
  speech_recognition_listening->SetVisibleImmediately(false);
  speech_recognition_listening->SetTransitionedProperties({OPACITY});
  speech_recognition_listening->SetTransitionDuration(
      base::TimeDelta::FromMilliseconds(
          kSpeechRecognitionOpacityAnimationDurationMs));
  speech_recognition_listening->AddBinding(
      std::make_unique<Binding<std::pair<bool, float>>>(
          VR_BIND_LAMBDA(
              [](Model* m, UiElement* result_parent) {
                return std::pair<bool, float>(
                    m->voice_search_enabled(),
                    result_parent->GetTargetOpacity());
              },
              base::Unretained(model_),
              base::Unretained(speech_result_parent.get())),
          VR_BIND_LAMBDA(
              [](UiElement* listening, const std::pair<bool, float>& value) {
                if (!value.first && value.second != 0.f) {
                  listening->SetVisibleImmediately(false);
                } else {
                  listening->SetVisible(value.first);
                }
              },
              base::Unretained(listening_ui_root))));
  scene_->AddUiElement(kSpeechRecognitionRoot,
                       std::move(speech_recognition_listening));
  scene_->AddUiElement(kSpeechRecognitionRoot, std::move(speech_result_parent));

  auto growing_circle = std::make_unique<Throbber>();
  growing_circle->SetName(kSpeechRecognitionListeningGrowingCircle);
  growing_circle->SetDrawPhase(kPhaseForeground);
  growing_circle->SetSize(kCloseButtonDiameter * 2, kCloseButtonDiameter * 2);
  growing_circle->set_corner_radius(kCloseButtonDiameter);
  growing_circle->set_hit_testable(false);
  VR_BIND_COLOR(model_, growing_circle.get(),
                &ColorScheme::speech_recognition_circle_background,
                &Rect::SetColor);
  growing_circle->AddBinding(
      VR_BIND(int, Model, model_, model->speech.speech_recognition_state,
              Throbber, growing_circle.get(),
              view->SetCircleGrowAnimationEnabled(
                  value == SPEECH_RECOGNITION_IN_SPEECH ||
                  value == SPEECH_RECOGNITION_RECOGNIZING ||
                  value == SPEECH_RECOGNITION_READY)));
  scene_->AddUiElement(kSpeechRecognitionListening, std::move(growing_circle));

  auto close_button = Create<DiscButton>(
      kSpeechRecognitionListeningCloseButton, kPhaseForeground,
      base::BindRepeating(&UiBrowserInterface::SetVoiceSearchActive,
                          base::Unretained(browser_), false),
      vector_icons::kClose16Icon);
  close_button->SetSize(kVoiceSearchCloseButtonWidth,
                        kVoiceSearchCloseButtonHeight);
  close_button->set_hover_offset(kButtonZOffsetHoverDMM * kContentDistance);
  close_button->SetTranslate(0.0, -kVoiceSearchCloseButtonYOffset, 0.f);
  VR_BIND_BUTTON_COLORS(model_, close_button.get(), &ColorScheme::button_colors,
                        &DiscButton::SetButtonColors);
  scene_->AddUiElement(kSpeechRecognitionListening, std::move(close_button));

  auto* root = scene_->GetUiElementByName(kSpeechRecognitionRoot);
  root->AddBinding(std::make_unique<Binding<bool>>(
      VR_BIND_LAMBDA(
          [](Model* model, UiElement* speech_listening,
             UiElement* speech_result_parent) {
            // The speech recognition root should be visible ad long as the
            // speech listening or result subtree is visibe.
            return model->voice_search_enabled() ||
                   speech_listening->GetTargetOpacity() != 0.f ||
                   speech_result_parent->GetTargetOpacity() != 0.f;
          },
          base::Unretained(model_),
          base::Unretained(
              scene_->GetUiElementByName(kSpeechRecognitionListening)),
          base::Unretained(
              scene_->GetUiElementByName(kSpeechRecognitionResult))),
      VR_BIND_LAMBDA(
          [](UiElement* e, const bool& value) { e->SetVisible(value); },
          base::Unretained(root))));

  BIND_VISIBILITY_CONTROL_FOR_VOICE(
      scene_->GetUiElementByName(k2dBrowsingVisibiltyControlForVoice), model_,
      browsing_enabled());
  BIND_VISIBILITY_CONTROL_FOR_VOICE(
      scene_->GetUiElementByName(kOmniboxVisibiltyControlForVoice), model_,
      omnibox_editing_enabled());
}

void UiSceneCreator::CreateController() {
  auto root = std::make_unique<UiElement>();
  root->SetName(kControllerRoot);
  root->set_hit_testable(false);
  VR_BIND_VISIBILITY(
      root, model->browsing_enabled() || model->web_vr.state == kWebVrTimedOut);
  scene_->AddUiElement(kRoot, std::move(root));

  auto group = std::make_unique<UiElement>();
  group->SetName(kControllerGroup);
  group->set_hit_testable(false);
  group->SetTransitionedProperties({OPACITY});
  group->AddBinding(std::make_unique<Binding<bool>>(
      VR_BIND_LAMBDA(
          [](Model* m) {
            return !m->controller.quiescent || !m->skips_redraw_when_not_dirty;
          },
          base::Unretained(model_)),
      VR_BIND_LAMBDA(
          [](UiElement* e, const bool& visible) {
            e->SetTransitionDuration(base::TimeDelta::FromMilliseconds(
                visible ? kControllerFadeInMs : kControllerFadeOutMs));
            e->SetVisible(visible);
          },
          base::Unretained(group.get()))));
  scene_->AddUiElement(kControllerRoot, std::move(group));

  auto controller = std::make_unique<Controller>();
  controller->SetDrawPhase(kPhaseForeground);
  controller->AddBinding(VR_BIND_FUNC(gfx::Transform, Model, model_,
                                      model->controller.transform, Controller,
                                      controller.get(), set_local_transform));
  controller->AddBinding(VR_BIND_FUNC(
      bool, Model, model_,
      model->controller.touchpad_button_state == UiInputManager::DOWN,
      Controller, controller.get(), set_touchpad_button_pressed));
  controller->AddBinding(
      VR_BIND_FUNC(bool, Model, model_,
                   model->controller.app_button_state == UiInputManager::DOWN,
                   Controller, controller.get(), set_app_button_pressed));
  controller->AddBinding(
      VR_BIND_FUNC(bool, Model, model_,
                   model->controller.home_button_state == UiInputManager::DOWN,
                   Controller, controller.get(), set_home_button_pressed));
  controller->AddBinding(VR_BIND_FUNC(float, Model, model_,
                                      model->controller.opacity, Controller,
                                      controller.get(), SetOpacity));
  scene_->AddUiElement(kControllerGroup, std::move(controller));

  auto laser = std::make_unique<Laser>(model_);
  laser->SetDrawPhase(kPhaseForeground);
  laser->AddBinding(VR_BIND_FUNC(float, Model, model_,
                                 model->controller.opacity, Laser, laser.get(),
                                 SetOpacity));
  scene_->AddUiElement(kControllerGroup, std::move(laser));

  auto reticle = std::make_unique<Reticle>(scene_, model_);
  reticle->SetDrawPhase(kPhaseForeground);
  scene_->AddUiElement(kControllerGroup, std::move(reticle));
}

std::unique_ptr<TextInput> UiSceneCreator::CreateTextInput(
    float font_height_meters,
    Model* model,
    TextInputInfo* text_input_model,
    TextInputDelegate* text_input_delegate) {
  auto text_input = std::make_unique<TextInput>(
      font_height_meters,
      base::BindRepeating(
          [](Model* model, bool focused) { model->editing_input = focused; },
          base::Unretained(model)),
      base::BindRepeating(
          [](TextInputInfo* model, const TextInputInfo& text_input_info) {
            *model = text_input_info;
          },
          base::Unretained(text_input_model)));
  text_input->SetDrawPhase(kPhaseNone);
  text_input->set_hit_testable(false);
  text_input->SetTextInputDelegate(text_input_delegate);
  text_input->AddBinding(std::make_unique<Binding<TextInputInfo>>(
      VR_BIND_LAMBDA([](TextInputInfo* info) { return *info; },
                     base::Unretained(text_input_model)),
      VR_BIND_LAMBDA([](TextInput* e,
                        const TextInputInfo& value) { e->UpdateInput(value); },
                     base::Unretained(text_input.get()))));
  return text_input;
}

void UiSceneCreator::CreateKeyboard() {
  auto visibility_control_root =
      Create<UiElement>(kKeyboardVisibilityControlForVoice, kPhaseNone);
  visibility_control_root->set_hit_testable(false);
  BIND_VISIBILITY_CONTROL_FOR_VOICE(visibility_control_root.get(), model_,
                                    editing_input);

  auto scaler = std::make_unique<ScaledDepthAdjuster>(kKeyboardDistance);
  scaler->SetName(kKeyboardDmmRoot);

  auto keyboard = std::make_unique<Keyboard>();
  keyboard->SetKeyboardDelegate(keyboard_delegate_);
  keyboard->SetDrawPhase(kPhaseForeground);
  keyboard->SetTranslate(0.0, kKeyboardVerticalOffsetDMM, 0.0);
  VR_BIND_VISIBILITY(keyboard, model->editing_input);
  scaler->AddChild(std::move(keyboard));
  visibility_control_root->AddChild(std::move(scaler));
  scene_->AddUiElement(k2dBrowsingRepositioner,
                       std::move(visibility_control_root));
}

void UiSceneCreator::CreateUrlBar() {
  auto scaler = std::make_unique<ScaledDepthAdjuster>(kUrlBarDistance);
  scaler->SetName(kUrlBarDmmRoot);
  scene_->AddUiElement(k2dBrowsingForeground, std::move(scaler));

  auto url_bar = Create<UiElement>(kUrlBar, kPhaseNone);
  url_bar->set_hit_testable(false);
  url_bar->SetTranslate(0, kUrlBarVerticalOffsetDMM, 0);
  url_bar->SetRotate(1, 0, 0, kUrlBarRotationRad);
  url_bar->set_bounds_contain_children(true);
  VR_BIND_VISIBILITY(url_bar, !model->fullscreen_enabled());
  scene_->AddUiElement(kUrlBarDmmRoot, std::move(url_bar));

  auto layout =
      Create<LinearLayout>(kUrlBarLayout, kPhaseNone, LinearLayout::kRight);
  layout->set_hit_testable(false);
  layout->set_bounds_contain_children(true);
  scene_->AddUiElement(kUrlBar, std::move(layout));

  auto back_button =
      Create<Button>(kUrlBarBackButton, kPhaseForeground,
                     base::BindRepeating(&UiBrowserInterface::NavigateBack,
                                         base::Unretained(browser_)));
  back_button->SetSize(kUrlBarBackButtonWidthDMM, kUrlBarHeightDMM);
  back_button->SetCornerRadii(
      {kUrlBarHeightDMM / 2, 0, kUrlBarHeightDMM / 2, 0});
  back_button->set_hover_offset(0.0f);
  back_button->AddBinding(VR_BIND_FUNC(bool, Model, model_,
                                       model->can_navigate_back, Button,
                                       back_button.get(), set_enabled));
  VR_BIND_BUTTON_COLORS(model_, back_button.get(), &ColorScheme::back_button,
                        &Button::SetButtonColors);
  scene_->AddUiElement(kUrlBarLayout, std::move(back_button));

  auto back_icon =
      Create<VectorIcon>(kUrlBarBackButtonIcon, kPhaseForeground, 128);
  back_icon->set_hit_testable(false);
  back_icon->SetIcon(vector_icons::kBackArrowIcon);
  back_icon->SetSize(kUrlBarBackButtonIconSizeDMM,
                     kUrlBarBackButtonIconSizeDMM);
  back_icon->SetTranslate(kUrlBarBackButtonIconOffsetDMM, 0, 0);
  back_icon->AddBinding(VR_BIND_FUNC(
      SkColor, Model, model_,
      model->can_navigate_back
          ? model->color_scheme().button_colors.foreground
          : model->color_scheme().button_colors.foreground_disabled,
      VectorIcon, back_icon.get(), SetColor));
  scene_->AddUiElement(kUrlBarBackButton, std::move(back_icon));

  auto separator = Create<Rect>(kUrlBarSeparator, kPhaseForeground);
  separator->SetSize(kUrlBarSeparatorWidthDMM, kUrlBarHeightDMM);
  VR_BIND_COLOR(model_, separator.get(), &ColorScheme::url_bar_separator,
                &Rect::SetColor);
  scene_->AddUiElement(kUrlBarLayout, std::move(separator));

  auto origin_region = Create<Rect>(kUrlBarOriginRegion, kPhaseForeground);
  origin_region->SetSize(kUrlBarOriginRegionWidthDMM, kUrlBarHeightDMM);
  origin_region->SetCornerRadii(
      {0, kUrlBarHeightDMM / 2, 0, kUrlBarHeightDMM / 2});
  VR_BIND_COLOR(model_, origin_region.get(), &ColorScheme::element_background,
                &Rect::SetColor);
  scene_->AddUiElement(kUrlBarLayout, std::move(origin_region));

  base::RepeatingCallback<void()> url_click_callback;
  if (base::FeatureList::IsEnabled(features::kVrBrowserKeyboard)) {
    url_click_callback = base::BindRepeating(
        [](Model* model) { model->push_mode(kModeEditingOmnibox); },
        base::Unretained(model_));
  } else {
    url_click_callback = base::BindRepeating([] {});
  }

  auto origin_content = Create<UrlBar>(
      kUrlBarOriginContent, kPhaseForeground, 512, url_click_callback,
      base::BindRepeating(&UiBrowserInterface::OnUnsupportedMode,
                          base::Unretained(browser_)));
  origin_content->SetSize(kUrlBarOriginContentWidthDMM, kUrlBarHeightDMM);
  origin_content->SetTranslate(kUrlBarOriginContentOffsetDMM, 0, 0);
  VR_BIND_VISIBILITY(origin_content, !model->fullscreen_enabled());
  origin_content->AddBinding(
      VR_BIND_FUNC(ToolbarState, Model, model_, model->toolbar_state, UrlBar,
                   origin_content.get(), SetToolbarState));
  origin_content->AddBinding(VR_BIND_FUNC(UrlBarColors, Model, model_,
                                          model->color_scheme().url_bar, UrlBar,
                                          origin_content.get(), SetColors));
  VR_BIND_COLOR(model_, origin_content.get(), &ColorScheme::element_background,
                &TexturedElement::SetBackgroundColor);
  scene_->AddUiElement(kUrlBarOriginRegion, std::move(origin_content));
}

void UiSceneCreator::CreateLoadingIndicator() {
  auto indicator_bg = std::make_unique<Rect>();
  indicator_bg->SetName(kLoadingIndicator);
  indicator_bg->SetDrawPhase(kPhaseForeground);
  indicator_bg->SetTranslate(0, kLoadingIndicatorVerticalOffsetDMM, 0);
  indicator_bg->SetSize(kLoadingIndicatorWidthDMM, kLoadingIndicatorHeightDMM);
  indicator_bg->set_y_anchoring(TOP);
  indicator_bg->SetTransitionedProperties({OPACITY});
  indicator_bg->set_corner_radius(kLoadingIndicatorHeightDMM * 0.5f);
  indicator_bg->set_contributes_to_parent_bounds(false);
  VR_BIND_VISIBILITY(indicator_bg, model->loading);
  VR_BIND_COLOR(model_, indicator_bg.get(),
                &ColorScheme::loading_indicator_background, &Rect::SetColor);

  scene_->AddUiElement(kUrlBar, std::move(indicator_bg));

  auto indicator_fg = std::make_unique<Rect>();
  indicator_fg->SetDrawPhase(kPhaseForeground);
  indicator_fg->SetName(kLoadingIndicatorForeground);
  indicator_fg->set_x_anchoring(LEFT);
  indicator_fg->set_corner_radius(kLoadingIndicatorHeightDMM * 0.5f);
  indicator_fg->set_hit_testable(false);
  VR_BIND_COLOR(model_, indicator_fg.get(),
                &ColorScheme::loading_indicator_foreground, &Rect::SetColor);
  indicator_fg->AddBinding(std::make_unique<Binding<float>>(
      VR_BIND_LAMBDA([](Model* m) { return m->load_progress; },
                     base::Unretained(model_)),
      VR_BIND_LAMBDA(
          [](Rect* r, const float& value) {
            r->SetSize(kLoadingIndicatorWidthDMM * value,
                       kLoadingIndicatorHeightDMM);
            r->SetTranslate(kLoadingIndicatorWidthDMM * value * 0.5f, 0.0f,
                            0.001f);
          },
          base::Unretained(indicator_fg.get()))));
  scene_->AddUiElement(kLoadingIndicator, std::move(indicator_fg));
}

void UiSceneCreator::CreateSnackbars() {
  auto snackbar = CreateSnackbar(
      kDownloadedSnackbar, model_, kFileDownloadDoneIcon,
      l10n_util::GetStringUTF16(IDS_VR_COMPONENT_UPDATE_READY),
      base::i18n::ToUpper(l10n_util::GetStringUTF16(IDS_VR_COMPONENT_APPLY)),
      base::BindRepeating(
          [](UiBrowserInterface* browser, Ui* ui) {
            ui->OnAssetsLoading();
            browser->LoadAssets();
          },
          base::Unretained(browser_), base::Unretained(ui_)));
  snackbar->AddBinding(std::make_unique<Binding<bool>>(
      VR_BIND_LAMBDA([](Model* m) { return m->can_apply_new_background; },
                     base::Unretained(model_)),
      VR_BIND_LAMBDA(
          [](UiElement* s, const bool& value) {
            s->SetVisible(value);
            s->SetRotate(1, 0, 0, value ? 0 : kSnackbarMoveInAngle);
          },
          base::Unretained(snackbar.get()))));
  snackbar->SetVisible(false);
  snackbar->SetRotate(1, 0, 0, kSnackbarMoveInAngle);
  snackbar->SetTransitionedProperties({OPACITY, TRANSFORM});
  snackbar->SetTransitionDuration(
      base::TimeDelta::FromMilliseconds(kSnackbarTransitionDurationMs));
  scene_->AddUiElement(k2dBrowsingRepositioner, std::move(snackbar));
}

void UiSceneCreator::CreateOmnibox() {
  auto visibility_control_root =
      Create<UiElement>(kOmniboxVisibiltyControlForVoice, kPhaseNone);
  visibility_control_root->set_hit_testable(false);

  auto scaler = std::make_unique<ScaledDepthAdjuster>(kUrlBarDistance);
  scaler->SetName(kOmniboxDmmRoot);

  auto visibility_toggle_for_audio_permission = Create<UiElement>(
      kOmniboxVisibilityControlForAudioPermissionPrompt, kPhaseNone);
  visibility_toggle_for_audio_permission->set_hit_testable(false);
  // Note that wnen the audio permissions prompt is triggered from the omnibox
  // editing mode, we don't change the opacity of the background like we do in
  // the default browsing case.
  visibility_toggle_for_audio_permission->AddBinding(VR_BIND_FUNC(
      bool, Model, model_,
      model->active_modal_prompt_type !=
          kModalPromptTypeExitVRForVoiceSearchRecordAudioOsPermission,
      UiElement, visibility_toggle_for_audio_permission.get(), SetVisible));

  auto omnibox_root = std::make_unique<UiElement>();
  omnibox_root->SetName(kOmniboxRoot);
  omnibox_root->SetDrawPhase(kPhaseNone);
  omnibox_root->SetVisible(false);
  omnibox_root->set_hit_testable(false);
  omnibox_root->SetTransitionedProperties({OPACITY});
  VR_BIND_VISIBILITY(omnibox_root, model->omnibox_editing_enabled());

  auto shadow = std::make_unique<Shadow>();
  shadow->SetName(kOmniboxShadow);
  shadow->SetDrawPhase(kPhaseForeground);
  shadow->set_intensity(kOmniboxShadowIntensity);
  shadow->set_y_anchoring(TOP);
  shadow->set_y_centering(BOTTOM);
  shadow->set_corner_radius(kOmniboxCornerRadiusDMM);

  auto omnibox_outer_layout = std::make_unique<LinearLayout>(LinearLayout::kUp);
  omnibox_outer_layout->set_hit_testable(false);
  omnibox_outer_layout->SetName(kOmniboxOuterLayout);
  omnibox_outer_layout->set_margin(kSuggestionGapDMM);
  omnibox_outer_layout->SetTranslate(
      0, kUrlBarVerticalOffsetDMM - 0.5f * kOmniboxHeightDMM,
      kOmniboxShadowOffset);
  omnibox_outer_layout->AddBinding(std::make_unique<Binding<bool>>(
      VR_BIND_LAMBDA([](Model* m) { return m->omnibox_editing_enabled(); },
                     base::Unretained(model_)),
      VR_BIND_LAMBDA(
          [](UiElement* e, const bool& v) {
            float y_offset =
                v ? kOmniboxVerticalOffsetDMM : kUrlBarVerticalOffsetDMM;
            y_offset -= 0.5 * kOmniboxHeightDMM;
            e->SetTranslate(0, y_offset, kOmniboxShadowOffset);
          },
          omnibox_outer_layout.get())));

  auto omnibox_container = std::make_unique<Rect>();
  omnibox_container->SetName(kOmniboxContainer);
  omnibox_container->SetDrawPhase(kPhaseForeground);
  omnibox_container->SetSize(kOmniboxWidthDMM, kOmniboxHeightDMM);
  omnibox_container->SetTransitionedProperties({TRANSFORM, OPACITY});
  omnibox_container->SetTransitionDuration(
      base::TimeDelta::FromMilliseconds(kOmniboxTransitionMs));
  omnibox_container->set_focusable(false);
  omnibox_container->AddBinding(std::make_unique<Binding<bool>>(
      VR_BIND_LAMBDA([](Model* m) { return m->omnibox_suggestions.empty(); },
                     base::Unretained(model_)),
      VR_BIND_LAMBDA(
          [](Rect* r, const bool& v) {
            if (v) {
              r->SetCornerRadii(
                  {kOmniboxCornerRadiusDMM, kOmniboxCornerRadiusDMM,
                   kOmniboxCornerRadiusDMM, kOmniboxCornerRadiusDMM});
            } else {
              r->SetCornerRadii(
                  {0, 0, kOmniboxCornerRadiusDMM, kOmniboxCornerRadiusDMM});
            }
          },
          omnibox_container.get())));
  VR_BIND_COLOR(model_, omnibox_container.get(),
                &ColorScheme::omnibox_background, &Rect::SetColor);

  float width = kOmniboxWidthDMM - 2 * kOmniboxTextMarginDMM -
                kOmniboxTextFieldIconSizeDMM;
  auto omnibox_text_field =
      CreateTextInput(kOmniboxTextHeightDMM, model_,
                      &model_->omnibox_text_field_info, text_input_delegate_);
  omnibox_text_field->set_input_committed_callback(base::BindRepeating(
      [](Model* model, UiBrowserInterface* browser, Ui* ui,
         const TextInputInfo& text) {
        if (!model->omnibox_suggestions.empty()) {
          browser->Navigate(model->omnibox_suggestions.front().destination);
          ui->OnUiRequestedNavigation();
        }
      },
      base::Unretained(model_), base::Unretained(browser_),
      base::Unretained(ui_)));
  omnibox_text_field->AddBinding(VR_BIND(
      TextInputInfo, Model, model_, model->omnibox_text_field_info,
      UiBrowserInterface, browser_, view->StartAutocomplete(value.text)));
  omnibox_text_field->SetSize(width, 0);
  omnibox_text_field->SetHintText(
      l10n_util::GetStringUTF16(IDS_SEARCH_OR_TYPE_URL));
  omnibox_text_field->SetName(kOmniboxTextField);
  omnibox_text_field->set_x_anchoring(LEFT);
  omnibox_text_field->SetTranslate(kOmniboxTextMarginDMM, 0, 0);
  omnibox_text_field->AddBinding(std::make_unique<Binding<bool>>(
      VR_BIND_LAMBDA([](Model* m) { return m->omnibox_editing_enabled(); },
                     base::Unretained(model_)),
      VR_BIND_LAMBDA(
          [](TextInput* e, Model* m, const bool& v) {
            if (v) {
              e->RequestFocus();
            } else {
              e->RequestUnfocus();
            }
          },
          base::Unretained(omnibox_text_field.get()),
          base::Unretained(model_))));
  omnibox_text_field->AddBinding(std::make_unique<Binding<bool>>(
      VR_BIND_LAMBDA(
          [](Model* m) { return m->has_mode_in_stack(kModeEditingOmnibox); },
          base::Unretained(model_)),
      VR_BIND_LAMBDA(
          [](TextInput* e, Model* m, const bool& unused) {
            m->omnibox_text_field_info = TextInputInfo();
          },
          base::Unretained(omnibox_text_field.get()),
          base::Unretained(model_))));
  omnibox_text_field->AddBinding(std::make_unique<Binding<AutocompleteStatus>>(
      VR_BIND_LAMBDA(
          [](Model* m) {
            AutocompleteStatus state;
            state.active = m->has_mode_in_stack(kModeEditingOmnibox);
            state.input = m->omnibox_text_field_info.text;
            return state;
          },
          base::Unretained(model_)),
      VR_BIND_LAMBDA(
          [](UiBrowserInterface* browser, const AutocompleteStatus& r) {
            if (r.active) {
              browser->StartAutocomplete(r.input);
            } else {
              browser->StopAutocomplete();
            }
          },
          base::Unretained(browser_))));
  VR_BIND_COLOR(model_, omnibox_text_field.get(), &ColorScheme::omnibox_text,
                &TextInput::SetTextColor);
  VR_BIND_COLOR(model_, omnibox_text_field.get(), &ColorScheme::cursor,
                &TextInput::SetCursorColor);
  VR_BIND_COLOR(model_, omnibox_text_field.get(), &ColorScheme::omnibox_hint,
                &TextInput::SetHintColor);

  auto mic_icon = Create<VectorIcon>(kNone, kPhaseForeground, 100);
  mic_icon->set_hit_testable(false);
  mic_icon->SetIcon(vector_icons::kMicrophoneIcon);
  mic_icon->SetSize(kOmniboxTextFieldIconSizeDMM, kOmniboxTextFieldIconSizeDMM);
  VR_BIND_COLOR(model_, mic_icon.get(), &ColorScheme::omnibox_text,
                &VectorIcon::SetColor);
  auto mic_icon_box = Create<Button>(
      kNone, kPhaseForeground,
      base::BindRepeating(
          [](UiBrowserInterface* b, Ui* ui) { b->SetVoiceSearchActive(true); },
          base::Unretained(browser_), base::Unretained(ui_)));
  mic_icon_box->set_hover_offset(kOmniboxTextFieldIconButtonHoverOffsetDMM);
  mic_icon_box->SetSize(kOmniboxTextFieldIconButtonSizeDMM,
                        kOmniboxTextFieldIconButtonSizeDMM);
  mic_icon_box->set_corner_radius(kOmniboxTextFieldIconButtonRadiusDMM);
  VR_BIND_VISIBILITY(
      mic_icon_box,
      !model->incognito && model->speech.has_or_can_request_audio_permission);
  VR_BIND_BUTTON_COLORS(model_, mic_icon_box.get(),
                        &ColorScheme::omnibox_voice_search_button_colors,
                        &Button::SetButtonColors);
  mic_icon_box->AddChild(std::move(mic_icon));

  auto text_field_layout = Create<LinearLayout>(
      kOmniboxTextFieldLayout, kPhaseNone, LinearLayout::kRight);
  text_field_layout->set_hit_testable(false);
  text_field_layout->AddChild(std::move(omnibox_text_field));
  text_field_layout->AddChild(std::move(mic_icon_box));

  // Set up the vector binding to manage suggestions dynamically.
  SuggestionSetBinding::ModelAddedCallback added_callback =
      base::BindRepeating(&OnSuggestionModelAdded, base::Unretained(scene_),
                          base::Unretained(browser_), base::Unretained(ui_),
                          base::Unretained(model_));
  SuggestionSetBinding::ModelRemovedCallback removed_callback =
      base::BindRepeating(&OnSuggestionModelRemoved, base::Unretained(scene_));

  auto suggestions_outer_layout =
      std::make_unique<LinearLayout>(LinearLayout::kDown);
  suggestions_outer_layout->SetName(kOmniboxSuggestionsOuterLayout);
  suggestions_outer_layout->set_hit_testable(false);

  auto suggestions_layout = std::make_unique<LinearLayout>(LinearLayout::kUp);
  suggestions_layout->SetName(kOmniboxSuggestions);
  suggestions_layout->SetDrawPhase(kPhaseNone);
  suggestions_layout->set_hit_testable(false);
  suggestions_layout->AddBinding(std::make_unique<SuggestionSetBinding>(
      &model_->omnibox_suggestions, added_callback, removed_callback));

  auto button_scaler =
      std::make_unique<ScaledDepthAdjuster>(kOmniboxCloseButtonDepthOffset);

  auto close_button = Create<DiscButton>(
      kOmniboxCloseButton, kPhaseForeground,
      base::BindRepeating(
          [](Model* model) { model->pop_mode(kModeEditingOmnibox); },
          base::Unretained(model_)),
      vector_icons::kBackArrowIcon);
  close_button->SetSize(kOmniboxCloseButtonDiameterDMM,
                        kOmniboxCloseButtonDiameterDMM);
  close_button->SetTranslate(0, kOmniboxCloseButtonVerticalOffsetDMM, 0);
  close_button->SetRotate(1, 0, 0, atan(kOmniboxCloseButtonVerticalOffsetDMM));
  close_button->set_hover_offset(kButtonZOffsetHoverDMM);
  VR_BIND_BUTTON_COLORS(model_, close_button.get(), &ColorScheme::button_colors,
                        &DiscButton::SetButtonColors);

  auto spacer = CreateOmniboxSpacer(model_);
  spacer->SetCornerRadii(
      {kOmniboxCornerRadiusDMM, kOmniboxCornerRadiusDMM, 0, 0});
  suggestions_outer_layout->AddChild(std::move(spacer));
  suggestions_outer_layout->AddChild(std::move(suggestions_layout));
  suggestions_outer_layout->AddChild(CreateOmniboxSpacer(model_));

  omnibox_container->AddChild(std::move(text_field_layout));

  omnibox_outer_layout->AddChild(std::move(omnibox_container));
  omnibox_outer_layout->AddChild(std::move(suggestions_outer_layout));

  shadow->AddChild(std::move(omnibox_outer_layout));

  button_scaler->AddChild(std::move(close_button));

  omnibox_root->AddChild(std::move(shadow));
  omnibox_root->AddChild(std::move(button_scaler));

  visibility_toggle_for_audio_permission->AddChild(std::move(omnibox_root));
  scaler->AddChild(std::move(visibility_toggle_for_audio_permission));

  visibility_control_root->AddChild(std::move(scaler));

  scene_->AddUiElement(k2dBrowsingRepositioner,
                       std::move(visibility_control_root));
}

void UiSceneCreator::CreateCloseButton() {
  base::RepeatingCallback<void()> click_handler = base::BindRepeating(
      [](Model* model, UiBrowserInterface* browser) {
        if (model->fullscreen_enabled()) {
          browser->ExitFullscreen();
        }
        if (model->in_cct) {
          browser->ExitCct();
        }
      },
      base::Unretained(model_), base::Unretained(browser_));
  std::unique_ptr<DiscButton> element =
      Create<DiscButton>(kCloseButton, kPhaseForeground, click_handler,
                         vector_icons::kClose16Icon);
  element->SetSize(kCloseButtonDiameter, kCloseButtonDiameter);
  element->set_hover_offset(kButtonZOffsetHoverDMM * kCloseButtonDistance);
  element->SetTranslate(0, kCloseButtonVerticalOffset, -kCloseButtonDistance);
  VR_BIND_BUTTON_COLORS(model_, element.get(), &ColorScheme::button_colors,
                        &DiscButton::SetButtonColors);

  // Close button is a special control element that needs to be hidden when
  // in WebVR, but it needs to be visible when in cct or fullscreen.
  VR_BIND_VISIBILITY(element, model->fullscreen_enabled() || model->in_cct);
  element->AddBinding(
      VR_BIND(bool, Model, model_, model->fullscreen_enabled(), UiElement,
              element.get(),
              view->SetTranslate(0,
                                 value ? kCloseButtonFullscreenVerticalOffset
                                       : kCloseButtonVerticalOffset,
                                 value ? -kCloseButtonFullscreenDistance
                                       : -kCloseButtonDistance)));
  element->AddBinding(VR_BIND(
      bool, Model, model_, model->fullscreen_enabled(), UiElement,
      element.get(),
      view->SetSize(
          value ? kCloseButtonFullscreenDiameter : kCloseButtonDiameter,
          value ? kCloseButtonFullscreenDiameter : kCloseButtonDiameter)));

  scene_->AddUiElement(k2dBrowsingForeground, std::move(element));
}

void UiSceneCreator::CreateExitPrompt() {
  std::unique_ptr<UiElement> element;

  // Place an invisible but hittable plane behind the exit prompt, to keep the
  // reticle roughly planar with the content if near content.
  auto backplane = std::make_unique<InvisibleHitTarget>();
  backplane->SetDrawPhase(kPhaseForeground);
  backplane->SetName(kExitPromptBackplane);
  backplane->SetSize(kPromptBackplaneSize, kPromptBackplaneSize);
  backplane->SetTranslate(0.0,
                          kContentVerticalOffset + kExitPromptVerticalOffset,
                          -kContentDistance);
  EventHandlers event_handlers;
  event_handlers.button_up = base::BindRepeating(
      [](UiBrowserInterface* browser, Model* m) {
        browser->OnExitVrPromptResult(
            ExitVrPromptChoice::CHOICE_NONE,
            GetReasonForPrompt(m->active_modal_prompt_type));
      },
      base::Unretained(browser_), base::Unretained(model_));
  backplane->set_event_handlers(event_handlers);
  VR_BIND_VISIBILITY(backplane, model->active_modal_prompt_type ==
                                    kModalPromptTypeExitVRForSiteInfo);
  scene_->AddUiElement(k2dBrowsingRepositioner, std::move(backplane));

  std::unique_ptr<ExitPrompt> exit_prompt = std::make_unique<ExitPrompt>(
      512,
      base::BindRepeating(&UiBrowserInterface::OnExitVrPromptResult,
                          base::Unretained(browser_),
                          ExitVrPromptChoice::CHOICE_STAY),
      base::BindRepeating(&UiBrowserInterface::OnExitVrPromptResult,
                          base::Unretained(browser_),
                          ExitVrPromptChoice::CHOICE_EXIT));
  exit_prompt->SetName(kExitPrompt);
  exit_prompt->SetDrawPhase(kPhaseForeground);
  exit_prompt->SetSize(kExitPromptWidth, kExitPromptHeight);
  VR_BIND_COLOR(model_, exit_prompt.get(), &ColorScheme::prompt_foreground,
                &TexturedElement::SetForegroundColor);
  VR_BIND_BUTTON_COLORS(model_, exit_prompt.get(),
                        &ColorScheme::prompt_primary_button_colors,
                        &ExitPrompt::SetPrimaryButtonColors);
  VR_BIND_BUTTON_COLORS(model_, exit_prompt.get(),
                        &ColorScheme::prompt_secondary_button_colors,
                        &ExitPrompt::SetSecondaryButtonColors);
  exit_prompt->AddBinding(std::make_unique<Binding<ModalPromptType>>(
      VR_BIND_LAMBDA([](Model* m) { return m->active_modal_prompt_type; },
                     base::Unretained(model_)),
      VR_BIND_LAMBDA(
          [](ExitPrompt* e, const ModalPromptType& p) {
            e->set_reason(GetReasonForPrompt(p));
            switch (p) {
              case kModalPromptTypeExitVRForSiteInfo:
                e->SetContentMessageId(
                    IDS_VR_SHELL_EXIT_PROMPT_DESCRIPTION_SITE_INFO);
                break;
              default:
                e->SetContentMessageId(IDS_VR_SHELL_EXIT_PROMPT_DESCRIPTION);
                break;
            }
          },
          base::Unretained(exit_prompt.get()))));
  scene_->AddUiElement(kExitPromptBackplane, std::move(exit_prompt));
}

void UiSceneCreator::CreateAudioPermissionPrompt() {
  std::unique_ptr<UiElement> element;

  // Place an invisible but hittable plane behind the exit prompt, to keep the
  // reticle roughly planar with the content if near content.
  auto backplane = std::make_unique<InvisibleHitTarget>();
  backplane->SetDrawPhase(kPhaseForeground);
  backplane->SetName(kAudioPermissionPromptBackplane);
  backplane->SetSize(kPromptBackplaneSize, kPromptBackplaneSize);
  backplane->SetTranslate(0.0, kContentVerticalOffset, -kOverlayPlaneDistance);
  EventHandlers event_handlers;
  event_handlers.button_up = base::BindRepeating(
      [](UiBrowserInterface* browser, Model* m) {
        browser->OnExitVrPromptResult(
            ExitVrPromptChoice::CHOICE_NONE,
            GetReasonForPrompt(m->active_modal_prompt_type));
      },
      base::Unretained(browser_), base::Unretained(model_));
  backplane->set_event_handlers(event_handlers);
  backplane->SetVisible(false);
  backplane->SetTransitionedProperties({OPACITY});
  VR_BIND_VISIBILITY(
      backplane,
      model->active_modal_prompt_type ==
          kModalPromptTypeExitVRForVoiceSearchRecordAudioOsPermission);

  std::unique_ptr<Shadow> shadow = std::make_unique<Shadow>();
  shadow->SetName(kAudioPermissionPromptShadow);
  shadow->SetDrawPhase(kPhaseForeground);

  std::unique_ptr<AudioPermissionPrompt> prompt =
      std::make_unique<AudioPermissionPrompt>(
          1024,
          base::BindRepeating(
              &UiBrowserInterface::OnExitVrPromptResult,
              base::Unretained(browser_), ExitVrPromptChoice::CHOICE_EXIT,
              UiUnsupportedMode::kVoiceSearchNeedsRecordAudioOsPermission),
          base::BindRepeating(
              &UiBrowserInterface::OnExitVrPromptResult,
              base::Unretained(browser_), ExitVrPromptChoice::CHOICE_STAY,
              UiUnsupportedMode::kVoiceSearchNeedsRecordAudioOsPermission));
  prompt->SetName(kAudioPermissionPrompt);
  prompt->SetDrawPhase(kPhaseForeground);
  prompt->SetSize(kAudioPermissionPromptWidth, kAudioPermissionPromptHeight);
  prompt->SetTranslate(0.0, 0.0f, kAudionPermisionPromptDepth);
  VR_BIND_BUTTON_COLORS(
      model_, prompt.get(),
      &ColorScheme::audio_permission_prompt_primary_button_colors,
      &AudioPermissionPrompt::SetPrimaryButtonColors);
  VR_BIND_BUTTON_COLORS(
      model_, prompt.get(),
      &ColorScheme::audio_permission_prompt_secondary_button_colors,
      &AudioPermissionPrompt::SetSecondaryButtonColors);
  VR_BIND_COLOR(model_, prompt.get(),
                &ColorScheme::audio_permission_prompt_icon_foreground,
                &AudioPermissionPrompt::SetIconColor);
  VR_BIND_COLOR(model_, prompt.get(),
                &ColorScheme::audio_permission_prompt_background,
                &TexturedElement::SetBackgroundColor);
  VR_BIND_COLOR(model_, prompt.get(), &ColorScheme::element_foreground,
                &TexturedElement::SetForegroundColor);
  shadow->AddChild(std::move(prompt));
  backplane->AddChild(std::move(shadow));
  scene_->AddUiElement(k2dBrowsingRepositioner, std::move(backplane));
}

void UiSceneCreator::CreateWebVrOverlayElements() {
  // Create url toast shown when WebVR is auto-presented.
  auto parent = CreateTransientParent(kWebVrUrlToastTransientParent,
                                      kWebVrUrlToastTimeoutSeconds, true);
  parent->AddBinding(std::make_unique<Binding<bool>>(
      VR_BIND_LAMBDA(
          [](Model* model, UiElement* splash_screen) {
            // The url toast should only be visible when the splash screen is
            // not visible.
            return model->web_vr_autopresentation_enabled() &&
                   model->web_vr.has_produced_frames() &&
                   splash_screen->GetTargetOpacity() == 0.f;
          },
          base::Unretained(model_),
          base::Unretained(
              scene_->GetUiElementByName(kSplashScreenTransientParent))),
      VR_BIND_LAMBDA(
          [](UiElement* e, const bool& value) { e->SetVisible(value); },
          base::Unretained(parent.get()))));
  scene_->AddUiElement(kWebVrViewportAwareRoot, std::move(parent));

  auto url_toast = std::make_unique<WebVrUrlToast>(
      512, base::BindRepeating(&UiBrowserInterface::OnUnsupportedMode,
                               base::Unretained(browser_)));
  url_toast->SetName(kWebVrUrlToast);
  url_toast->set_opacity_when_visible(kWebVrUrlToastOpacity);
  url_toast->SetDrawPhase(kPhaseOverlayForeground);
  url_toast->set_hit_testable(false);
  url_toast->SetTranslate(
      0, kWebVrToastDistance * sin(kWebVrUrlToastRotationRad),
      -kWebVrToastDistance * cos(kWebVrUrlToastRotationRad));
  url_toast->SetRotate(1, 0, 0, kWebVrUrlToastRotationRad);
  url_toast->SetSize(kWebVrUrlToastWidth, kWebVrUrlToastHeight);
  VR_BIND_COLOR(model_, url_toast.get(),
                &ColorScheme::web_vr_transient_toast_background,
                &TexturedElement::SetBackgroundColor);
  VR_BIND_COLOR(model_, url_toast.get(),
                &ColorScheme::web_vr_transient_toast_foreground,
                &TexturedElement::SetForegroundColor);
  url_toast->AddBinding(VR_BIND_FUNC(ToolbarState, Model, model_,
                                     model->toolbar_state, WebVrUrlToast,
                                     url_toast.get(), SetToolbarState));
  scene_->AddUiElement(kWebVrUrlToastTransientParent, std::move(url_toast));

  // Create "Press app button to exit" toast.
  parent =
      CreateTransientParent(kExclusiveScreenToastViewportAwareTransientParent,
                            kToastTimeoutSeconds, false);
  // When we first get a web vr frame, we switch states to
  // kWebVrNoTimeoutPending, when that happens, we want to SetVisible(true) to
  // kick the visibility of this element.
  VR_BIND_VISIBILITY(parent, model->web_vr.has_produced_frames() &&
                                 model->web_vr.show_exit_toast);
  scene_->AddUiElement(kWebVrViewportAwareRoot, std::move(parent));

  auto scaler = std::make_unique<ScaledDepthAdjuster>(kWebVrToastDistance);

  auto exit_toast = std::make_unique<Toast>();
  exit_toast->SetName(kExclusiveScreenToastViewportAware);
  exit_toast->SetDrawPhase(kPhaseOverlayForeground);
  exit_toast->set_hit_testable(false);
  exit_toast->SetTranslate(0, sin(kWebVrAngleRadians),
                           1.0 - cos(kWebVrAngleRadians));
  exit_toast->SetRotate(1, 0, 0, kWebVrAngleRadians);
  exit_toast->set_padding(kExclusiveScreenToastXPaddingDMM,
                          kExclusiveScreenToastYPaddingDMM);
  exit_toast->set_corner_radius(kExclusiveScreenToastCornerRadiusDMM);
  exit_toast->AddText(l10n_util::GetStringUTF16(IDS_PRESS_APP_TO_EXIT),
                      kExclusiveScreenToastTextFontHeightDMM,
                      TextLayoutMode::kSingleLineFixedHeight);

  VR_BIND_COLOR(model_, exit_toast.get(),
                &ColorScheme::exclusive_screen_toast_background,
                &Toast::SetBackgroundColor);
  VR_BIND_COLOR(model_, exit_toast.get(),
                &ColorScheme::exclusive_screen_toast_foreground,
                &Toast::SetForegroundColor);

  scaler->AddChild(std::move(exit_toast));
  scene_->AddUiElement(kExclusiveScreenToastViewportAwareTransientParent,
                       std::move(scaler));
}

void UiSceneCreator::CreateFullscreenToast() {
  auto parent = CreateTransientParent(kExclusiveScreenToastTransientParent,
                                      kToastTimeoutSeconds, false);
  VR_BIND_VISIBILITY(parent, model->fullscreen_enabled());
  scene_->AddUiElement(k2dBrowsingForeground, std::move(parent));

  auto scaler = std::make_unique<ScaledDepthAdjuster>(kFullscreenDistance);

  auto element = std::make_unique<Toast>();
  element->SetName(kExclusiveScreenToast);
  element->SetDrawPhase(kPhaseForeground);
  element->set_hit_testable(false);
  element->SetTranslate(0, kFullScreenToastOffsetDMM, 0);
  element->set_padding(kExclusiveScreenToastXPaddingDMM,
                       kExclusiveScreenToastYPaddingDMM);
  element->set_corner_radius(kExclusiveScreenToastCornerRadiusDMM);
  element->AddText(l10n_util::GetStringUTF16(IDS_PRESS_APP_TO_EXIT),
                   kExclusiveScreenToastTextFontHeightDMM,
                   TextLayoutMode::kSingleLineFixedHeight);

  VR_BIND_COLOR(model_, element.get(),
                &ColorScheme::exclusive_screen_toast_background,
                &Toast::SetBackgroundColor);
  VR_BIND_COLOR(model_, element.get(),
                &ColorScheme::exclusive_screen_toast_foreground,
                &Toast::SetForegroundColor);

  scaler->AddChild(std::move(element));
  scene_->AddUiElement(kExclusiveScreenToastTransientParent, std::move(scaler));
}

}  // namespace vr
