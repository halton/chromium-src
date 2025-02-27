// Copyright (c) 2012 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.
//
// This file defines utility functions for eliding and formatting UI text.

#ifndef UI_GFX_TEXT_ELIDER_H_
#define UI_GFX_TEXT_ELIDER_H_

#include <stddef.h>

#include <string>
#include <vector>

#include "base/macros.h"
#include "base/strings/string16.h"
#include "build/build_config.h"
#include "ui/gfx/gfx_export.h"
#include "ui/gfx/text_constants.h"

namespace base {
class FilePath;
}

namespace gfx {
class FontList;

GFX_EXPORT extern const char kEllipsis[];
GFX_EXPORT extern const base::char16 kEllipsisUTF16[];
GFX_EXPORT extern const base::char16 kForwardSlash;

// Helper class to split + elide text, while respecting UTF-16 surrogate pairs
// and combining character sequences.
class GFX_EXPORT StringSlicer {
 public:
  // Warning: Retains a reference to |text| and |ellipsis|. They must have a
  // longer lifetime than the StringSlicer.
  StringSlicer(const base::string16& text,
               const base::string16& ellipsis,
               bool elide_in_middle,
               bool elide_at_beginning);

  // Cuts |text_| to be at most |length| UTF-16 code units long. If
  // |elide_in_middle_| is true, the middle of the string is removed to leave
  // equal-length pieces from the beginning and end of the string; otherwise,
  // the end of the string is removed and only the beginning remains. If
  // |insert_ellipsis| is true, then an ellipsis character will be inserted at
  // the cut point (note that the ellipsis will does not count towards the
  // |length| limit).
  // Note: Characters may still be omitted even if |length| is the full string
  // length, if surrogate pairs fall on the split boundary.
  base::string16 CutString(size_t length, bool insert_ellipsis) const;

 private:
  // The text to be sliced.
  const base::string16& text_;

  // Ellipsis string to use.
  const base::string16& ellipsis_;

  // If true, the middle of the string will be elided.
  bool elide_in_middle_;

  // If true, the beginning of the string will be elided.
  bool elide_at_beginning_;

  DISALLOW_COPY_AND_ASSIGN(StringSlicer);
};

// Elides |text| to fit the |available_pixel_width| with the specified behavior.
GFX_EXPORT base::string16 ElideText(
    const base::string16& text,
    const gfx::FontList& font_list,
    float available_pixel_width,
    ElideBehavior elide_behavior,
    Typesetter typesetter = Typesetter::DEFAULT);

// Elide a filename to fit a given pixel width, with an emphasis on not hiding
// the extension unless we have to. If filename contains a path, the path will
// be removed if filename doesn't fit into available_pixel_width. The elided
// filename is forced to have LTR directionality, which means that in RTL UI
// the elided filename is wrapped with LRE (Left-To-Right Embedding) mark and
// PDF (Pop Directional Formatting) mark.
GFX_EXPORT base::string16 ElideFilename(
    const base::FilePath& filename,
    const gfx::FontList& font_list,
    float available_pixel_width,
    Typesetter typesetter = Typesetter::DEFAULT);

// Functions to elide strings when the font information is unknown. As opposed
// to the above functions, ElideString() and ElideRectangleString() operate in
// terms of character units, not pixels.

// If the size of |input| is more than |max_len|, this function returns
// true and |input| is shortened into |output| by removing chars in the
// middle (they are replaced with up to 3 dots, as size permits).
// Ex: ElideString(ASCIIToUTF16("Hello"), 10, &str) puts Hello in str and
// returns false.  ElideString(ASCIIToUTF16("Hello my name is Tom"), 10, &str)
// puts "Hell...Tom" in str and returns true.
// TODO(tsepez): Doesn't handle UTF-16 surrogate pairs properly.
// TODO(tsepez): Doesn't handle bidi properly.
GFX_EXPORT bool ElideString(const base::string16& input, size_t max_len,
                            base::string16* output);

// Reformat |input| into |output| so that it fits into a |max_rows| by
// |max_cols| rectangle of characters.  Input newlines are respected, but
// lines that are too long are broken into pieces.  If |strict| is true,
// we break first at naturally occuring whitespace boundaries, otherwise
// we assume some other mechanism will do this in approximately the same
// spot after the fact.  If the word itself is too long, we always break
// intra-word (respecting UTF-16 surrogate pairs) as necssary. Truncation
// (indicated by an added 3 dots) occurs if the result is still too long.
//  Returns true if the input had to be truncated (and not just reformatted).
GFX_EXPORT bool ElideRectangleString(const base::string16& input,
                                     size_t max_rows,
                                     size_t max_cols,
                                     bool strict,
                                     base::string16* output);

// Indicates whether the |available_pixel_width| by |available_pixel_height|
// rectangle passed to |ElideRectangleText()| had insufficient space to
// accommodate the given |text|, leading to elision or truncation.
enum ReformattingResultFlags {
  INSUFFICIENT_SPACE_HORIZONTAL = 1 << 0,
  INSUFFICIENT_SPACE_VERTICAL = 1 << 1,
};

// Reformats |text| into output vector |lines| so that the resulting text fits
// into an |available_pixel_width| by |available_pixel_height| rectangle with
// the specified |font_list|. Input newlines are respected, but lines that are
// too long are broken into pieces. For words that are too wide to fit on a
// single line, the wrapping behavior can be specified with the |wrap_behavior|
// param. Returns a combination of |ReformattingResultFlags| that indicate
// whether the given rectangle had insufficient space to accommodate |text|,
// leading to elision or truncation (and not just reformatting).
GFX_EXPORT int ElideRectangleText(const base::string16& text,
                                  const gfx::FontList& font_list,
                                  float available_pixel_width,
                                  int available_pixel_height,
                                  WordWrapBehavior wrap_behavior,
                                  std::vector<base::string16>* lines);

#if defined(OS_MACOSX)
// As above, but uses the native platform typesetter (CoreText on Mac).
GFX_EXPORT int ElideRectangleTextForNativeUi(
    const base::string16& input,
    const FontList& font_list,
    float available_pixel_width,
    int available_pixel_height,
    WordWrapBehavior wrap_behavior,
    std::vector<base::string16>* lines);
#endif  // OS_MACOSX

// Truncates |string| to |length| characters. This breaks the string according
// to the specified |break_type|, which must be either WORD_BREAK or
// CHARACTER_BREAK, and adds the horizontal ellipsis character (unicode
// character 0x2026) to render "...". The supplied string is returned if the
// string has |length| characters or less.
GFX_EXPORT base::string16 TruncateString(const base::string16& string,
                                         size_t length,
                                         BreakType break_type);

}  // namespace gfx

#endif  // UI_GFX_TEXT_ELIDER_H_
