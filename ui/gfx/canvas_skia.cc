// Copyright (c) 2012 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include <limits.h>
#include <stddef.h>
#include <stdint.h>

#include <memory>

#include "build/build_config.h"
#include "cc/paint/paint_canvas.h"
#include "ui/gfx/canvas.h"
#include "ui/gfx/geometry/insets.h"
#include "ui/gfx/render_text.h"
#include "ui/gfx/skia_util.h"
#include "ui/gfx/text_elider.h"
#include "ui/gfx/text_utils.h"

namespace gfx {

namespace {

// Strips accelerator character prefixes in |text| if needed, based on |flags|.
// Returns a range in |text| to underline or Range::InvalidRange() if
// underlining is not needed.
Range StripAcceleratorChars(int flags, base::string16* text) {
  if (flags & (Canvas::SHOW_PREFIX | Canvas::HIDE_PREFIX)) {
    int char_pos = -1;
    int char_span = 0;
    *text = RemoveAcceleratorChar(*text, '&', &char_pos, &char_span);
    if ((flags & Canvas::SHOW_PREFIX) && char_pos != -1)
      return Range(char_pos, char_pos + char_span);
  }
  return Range::InvalidRange();
}

// Elides |text| and adjusts |range| appropriately. If eliding causes |range|
// to no longer point to the same character in |text|, |range| is made invalid.
void ElideTextAndAdjustRange(const FontList& font_list,
                             float width,
                             base::string16* text,
                             Range* range) {
  const base::char16 start_char =
      (range->IsValid() ? text->at(range->start()) : 0);
  *text =
      ElideText(*text, font_list, width, ELIDE_TAIL, gfx::Typesetter::HARFBUZZ);
  if (!range->IsValid())
    return;
  if (range->start() >= text->length() ||
      text->at(range->start()) != start_char) {
    *range = Range::InvalidRange();
  }
}

// Updates |render_text| from the specified parameters.
void UpdateRenderText(const Rect& rect,
                      const base::string16& text,
                      const FontList& font_list,
                      int flags,
                      SkColor color,
                      RenderText* render_text) {
  render_text->SetFontList(font_list);
  render_text->SetText(text);
  render_text->SetCursorEnabled(false);
  render_text->SetDisplayRect(rect);

  // Set the text alignment explicitly based on the directionality of the UI,
  // if not specified.
  if (!(flags & (Canvas::TEXT_ALIGN_CENTER |
                 Canvas::TEXT_ALIGN_RIGHT |
                 Canvas::TEXT_ALIGN_LEFT |
                 Canvas::TEXT_ALIGN_TO_HEAD))) {
    flags |= Canvas::DefaultCanvasTextAlignment();
  }

  if (flags & Canvas::TEXT_ALIGN_TO_HEAD)
    render_text->SetHorizontalAlignment(ALIGN_TO_HEAD);
  else if (flags & Canvas::TEXT_ALIGN_RIGHT)
    render_text->SetHorizontalAlignment(ALIGN_RIGHT);
  else if (flags & Canvas::TEXT_ALIGN_CENTER)
    render_text->SetHorizontalAlignment(ALIGN_CENTER);
  else
    render_text->SetHorizontalAlignment(ALIGN_LEFT);

  render_text->set_subpixel_rendering_suppressed(
      (flags & Canvas::NO_SUBPIXEL_RENDERING) != 0);

  render_text->SetColor(color);
  const int font_style = font_list.GetFontStyle();
  render_text->SetStyle(ITALIC, (font_style & Font::ITALIC) != 0);
  render_text->SetStyle(UNDERLINE, (font_style & Font::UNDERLINE) != 0);
  render_text->SetWeight(font_list.GetFontWeight());
}

}  // namespace

// static
void Canvas::SizeStringFloat(const base::string16& text,
                             const FontList& font_list,
                             float* width,
                             float* height,
                             int line_height,
                             int flags,
                             Typesetter typesetter) {
  DCHECK_GE(*width, 0);
  DCHECK_GE(*height, 0);

  if ((flags & MULTI_LINE) && *width != 0) {
    WordWrapBehavior wrap_behavior = TRUNCATE_LONG_WORDS;
    if (flags & CHARACTER_BREAK)
      wrap_behavior = WRAP_LONG_WORDS;
    else if (!(flags & NO_ELLIPSIS))
      wrap_behavior = ELIDE_LONG_WORDS;

    std::vector<base::string16> strings;
    ElideRectangleText(text, font_list, *width, INT_MAX, wrap_behavior,
                       &strings);
    Rect rect(base::saturated_cast<int>(*width), INT_MAX);

    // Note the following DCHECK can be removed when this codepath no longer
    // uses CreateInstanceDeprecated(), which always uses BROWSER.
    DCHECK_EQ(Typesetter::BROWSER, typesetter);
    // This needs to match the instance used in ElideRectangleText.
    auto render_text = RenderText::CreateInstanceDeprecated();

    UpdateRenderText(rect, base::string16(), font_list, flags, 0,
                     render_text.get());

    float h = 0;
    float w = 0;
    for (size_t i = 0; i < strings.size(); ++i) {
      StripAcceleratorChars(flags, &strings[i]);
      render_text->SetText(strings[i]);
      const SizeF& string_size = render_text->GetStringSizeF();
      w = std::max(w, string_size.width());
      h += (i > 0 && line_height > 0) ?
               std::max(static_cast<float>(line_height), string_size.height())
                   : string_size.height();
    }
    *width = w;
    *height = h;
  } else {
    auto render_text = RenderText::CreateFor(typesetter);

    Rect rect(base::saturated_cast<int>(*width),
              base::saturated_cast<int>(*height));
    base::string16 adjusted_text = text;
    StripAcceleratorChars(flags, &adjusted_text);
    UpdateRenderText(rect, adjusted_text, font_list, flags, 0,
                     render_text.get());
    const SizeF& string_size = render_text->GetStringSizeF();
    *width = string_size.width();
    *height = string_size.height();
  }
}

void Canvas::DrawStringRectWithFlags(const base::string16& text,
                                     const FontList& font_list,
                                     SkColor color,
                                     const Rect& text_bounds,
                                     int flags) {
  if (!IntersectsClipRect(RectToSkRect(text_bounds)))
    return;

  canvas_->save();
  ClipRect(text_bounds);

  Rect rect(text_bounds);

  // Since we're drawing into a canvas anyway, just use Harfbuzz on Mac.
  auto render_text = gfx::RenderText::CreateHarfBuzzInstance();

  if (flags & MULTI_LINE) {
    WordWrapBehavior wrap_behavior = IGNORE_LONG_WORDS;
    if (flags & CHARACTER_BREAK)
      wrap_behavior = WRAP_LONG_WORDS;
    else if (!(flags & NO_ELLIPSIS))
      wrap_behavior = ELIDE_LONG_WORDS;

    std::vector<base::string16> strings;
    ElideRectangleText(text, font_list,
                       static_cast<float>(text_bounds.width()),
                       text_bounds.height(), wrap_behavior, &strings);

    for (size_t i = 0; i < strings.size(); i++) {
      Range range = StripAcceleratorChars(flags, &strings[i]);
      UpdateRenderText(rect, strings[i], font_list, flags, color,
                       render_text.get());
      int line_padding = 0;
      const int line_height = render_text->GetStringSize().height();

      // TODO(msw|asvitkine): Center Windows multi-line text: crbug.com/107357
#if !defined(OS_WIN)
      if (i == 0) {
        // TODO(msw|asvitkine): Support multi-line text with varied heights.
        const int text_height = strings.size() * line_height - line_padding;
        rect += Vector2d(0, (text_bounds.height() - text_height) / 2);
      }
#endif

      rect.set_height(line_height - line_padding);

      if (range.IsValid())
        render_text->ApplyStyle(UNDERLINE, true, range);
      render_text->SetDisplayRect(rect);
      render_text->Draw(this);
      rect += Vector2d(0, line_height);
    }
  } else {
    base::string16 adjusted_text = text;
    Range range = StripAcceleratorChars(flags, &adjusted_text);
    bool elide_text = ((flags & NO_ELLIPSIS) == 0);

#if defined(OS_LINUX)
    // On Linux, eliding really means fading the end of the string. But only
    // for LTR text. RTL text is still elided (on the left) with "...".
    if (elide_text) {
      render_text->SetText(adjusted_text);
      if (render_text->GetDisplayTextDirection() == base::i18n::LEFT_TO_RIGHT) {
        render_text->SetElideBehavior(FADE_TAIL);
        elide_text = false;
      }
    }
#endif

    if (elide_text) {
      ElideTextAndAdjustRange(font_list,
                              static_cast<float>(text_bounds.width()),
                              &adjusted_text, &range);
    }

    UpdateRenderText(rect, adjusted_text, font_list, flags, color,
                     render_text.get());
    if (range.IsValid())
      render_text->ApplyStyle(UNDERLINE, true, range);
    render_text->Draw(this);
  }

  canvas_->restore();
}

void Canvas::DrawFadedString(const base::string16& text,
                             const FontList& font_list,
                             SkColor color,
                             const Rect& display_rect,
                             int flags) {
  // If the whole string fits in the destination then just draw it directly.
  if (GetStringWidth(text, font_list) <= display_rect.width()) {
    DrawStringRectWithFlags(text, font_list, color, display_rect, flags);
    return;
  }
  // Align with content directionality instead of fading both ends.
  flags &= ~TEXT_ALIGN_CENTER;
  if (!(flags & (TEXT_ALIGN_LEFT | TEXT_ALIGN_RIGHT)))
    flags |= TEXT_ALIGN_TO_HEAD;
  flags |= NO_ELLIPSIS;

  // TODO(tapted): Remove Canvas::DrawFadedString() - it's unused.
  auto render_text = RenderText::CreateInstanceDeprecated();
  Rect rect = display_rect;
  UpdateRenderText(rect, text, font_list, flags, color, render_text.get());
  render_text->SetElideBehavior(FADE_TAIL);

  canvas_->save();
  ClipRect(display_rect);
  render_text->Draw(this);
  canvas_->restore();
}

}  // namespace gfx
