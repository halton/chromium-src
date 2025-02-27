/*
 * Copyright (C) 2014 Google Inc. All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are
 * met:
 *
 *     * Redistributions of source code must retain the above copyright
 * notice, this list of conditions and the following disclaimer.
 *     * Redistributions in binary form must reproduce the above
 * copyright notice, this list of conditions and the following disclaimer
 * in the documentation and/or other materials provided with the
 * distribution.
 *     * Neither the name of Google Inc. nor the names of its
 * contributors may be used to endorse or promote products derived from
 * this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
 * A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
 * OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 * SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 * LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 * DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 * THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#include "platform/text/Character.h"

#include <unicode/uobject.h>
#include <unicode/uscript.h>
#include <algorithm>
#include "platform/text/ICUError.h"
#include "platform/wtf/StdLibExtras.h"
#include "platform/wtf/text/StringBuilder.h"

#if defined(USING_SYSTEM_ICU)
#include "platform/text/CharacterPropertyDataGenerator.h"
#include <unicode/uniset.h>
#else
#define MUTEX_H  // Prevent compile failure of utrie2.h on Windows
#include <utrie2.h>
#endif

namespace blink {

#if defined(USING_SYSTEM_ICU)
static icu::UnicodeSet* createUnicodeSet(const UChar32* characters,
                                         size_t charactersCount,
                                         const UChar32* ranges,
                                         size_t rangesCount) {
  icu::UnicodeSet* unicodeSet = new icu::UnicodeSet();
  for (size_t i = 0; i < charactersCount; i++)
    unicodeSet->add(characters[i]);
  for (size_t i = 0; i < rangesCount; i += 2)
    unicodeSet->add(ranges[i], ranges[i + 1]);
  unicodeSet->freeze();
  return unicodeSet;
}

#define CREATE_UNICODE_SET(name)                                             \
  createUnicodeSet(name##Array, WTF_ARRAY_LENGTH(name##Array), name##Ranges, \
                   WTF_ARRAY_LENGTH(name##Ranges))

#define RETURN_HAS_PROPERTY(c, name)            \
  static icu::UnicodeSet* unicodeSet = nullptr; \
  if (!unicodeSet)                              \
    unicodeSet = CREATE_UNICODE_SET(name);      \
  return unicodeSet->contains(c);
#else
// Freezed trie tree, see CharacterDataGenerator.cpp.
extern const int32_t kSerializedCharacterDataSize;
extern const uint8_t kSerializedCharacterData[];

static UTrie2* CreateTrie() {
  // Create a Trie from the value array.
  ICUError error;
  UTrie2* trie = utrie2_openFromSerialized(
      UTrie2ValueBits::UTRIE2_16_VALUE_BITS, kSerializedCharacterData,
      kSerializedCharacterDataSize, nullptr, &error);
  DCHECK_EQ(error, U_ZERO_ERROR);
  return trie;
}

static bool HasProperty(UChar32 c, CharacterProperty property) {
  static UTrie2* trie = nullptr;
  if (!trie)
    trie = CreateTrie();
  return UTRIE2_GET16(trie, c) & static_cast<CharacterPropertyType>(property);
}

#define RETURN_HAS_PROPERTY(c, name) \
  return HasProperty(c, CharacterProperty::name);
#endif

// Takes a flattened list of closed intervals
template <class T, size_t size>
bool ValueInIntervalList(const T (&interval_list)[size], const T& value) {
  const T* bound =
      std::upper_bound(&interval_list[0], &interval_list[size], value);
  if ((bound - interval_list) % 2 == 1)
    return true;
  return bound > interval_list && *(bound - 1) == value;
}

bool Character::IsUprightInMixedVertical(UChar32 character) {
  RETURN_HAS_PROPERTY(character, kIsUprightInMixedVertical)
}

bool Character::IsCJKIdeographOrSymbolSlow(UChar32 c) {
  RETURN_HAS_PROPERTY(c, kIsCJKIdeographOrSymbol)
}

bool Character::IsPotentialCustomElementNameChar(UChar32 character) {
  RETURN_HAS_PROPERTY(character, kIsPotentialCustomElementNameChar);
}

unsigned Character::ExpansionOpportunityCount(const LChar* characters,
                                              size_t length,
                                              TextDirection direction,
                                              bool& is_after_expansion,
                                              const TextJustify text_justify) {
  unsigned count = 0;
  if (text_justify == TextJustify::kDistribute) {
    is_after_expansion = true;
    return length;
  }

  if (direction == TextDirection::kLtr) {
    for (size_t i = 0; i < length; ++i) {
      if (TreatAsSpace(characters[i])) {
        count++;
        is_after_expansion = true;
      } else {
        is_after_expansion = false;
      }
    }
  } else {
    for (size_t i = length; i > 0; --i) {
      if (TreatAsSpace(characters[i - 1])) {
        count++;
        is_after_expansion = true;
      } else {
        is_after_expansion = false;
      }
    }
  }

  return count;
}

unsigned Character::ExpansionOpportunityCount(const UChar* characters,
                                              size_t length,
                                              TextDirection direction,
                                              bool& is_after_expansion,
                                              const TextJustify text_justify) {
  unsigned count = 0;
  if (direction == TextDirection::kLtr) {
    for (size_t i = 0; i < length; ++i) {
      UChar32 character = characters[i];
      if (TreatAsSpace(character)) {
        count++;
        is_after_expansion = true;
        continue;
      }
      if (U16_IS_LEAD(character) && i + 1 < length &&
          U16_IS_TRAIL(characters[i + 1])) {
        character = U16_GET_SUPPLEMENTARY(character, characters[i + 1]);
        i++;
      }
      if (text_justify == TextJustify::kAuto &&
          IsCJKIdeographOrSymbol(character)) {
        if (!is_after_expansion)
          count++;
        count++;
        is_after_expansion = true;
        continue;
      }
      is_after_expansion = false;
    }
  } else {
    for (size_t i = length; i > 0; --i) {
      UChar32 character = characters[i - 1];
      if (TreatAsSpace(character)) {
        count++;
        is_after_expansion = true;
        continue;
      }
      if (U16_IS_TRAIL(character) && i > 1 && U16_IS_LEAD(characters[i - 2])) {
        character = U16_GET_SUPPLEMENTARY(characters[i - 2], character);
        i--;
      }
      if (text_justify == TextJustify::kAuto &&
          IsCJKIdeographOrSymbol(character)) {
        if (!is_after_expansion)
          count++;
        count++;
        is_after_expansion = true;
        continue;
      }
      is_after_expansion = false;
    }
  }
  return count;
}

bool Character::CanTextDecorationSkipInk(UChar32 codepoint) {
  if (Character::IsCJKIdeographOrSymbol(codepoint))
    return false;

  UBlockCode block = ublock_getCode(codepoint);
  switch (block) {
    // These blocks contain CJK characters we don't want to skip ink, but are
    // not ideograph that IsCJKIdeographOrSymbol() does not cover.
    case UBLOCK_HANGUL_JAMO:
    case UBLOCK_HANGUL_COMPATIBILITY_JAMO:
    case UBLOCK_HANGUL_SYLLABLES:
    case UBLOCK_HANGUL_JAMO_EXTENDED_A:
    case UBLOCK_HANGUL_JAMO_EXTENDED_B:
    case UBLOCK_LINEAR_B_IDEOGRAMS:
      return false;
    default:
      return true;
  }
}

bool Character::CanReceiveTextEmphasis(UChar32 c) {
  WTF::Unicode::CharCategory category = WTF::Unicode::Category(c);
  if (category &
      (WTF::Unicode::kSeparator_Space | WTF::Unicode::kSeparator_Line |
       WTF::Unicode::kSeparator_Paragraph | WTF::Unicode::kOther_NotAssigned |
       WTF::Unicode::kOther_Control | WTF::Unicode::kOther_Format))
    return false;

  // Additional word-separator characters listed in CSS Text Level 3 Editor's
  // Draft 3 November 2010.
  if (c == kEthiopicWordspaceCharacter ||
      c == kAegeanWordSeparatorLineCharacter ||
      c == kAegeanWordSeparatorDotCharacter ||
      c == kUgariticWordDividerCharacter ||
      c == kTibetanMarkIntersyllabicTshegCharacter ||
      c == kTibetanMarkDelimiterTshegBstarCharacter)
    return false;

  return true;
}

bool Character::IsEmojiFlagSequenceTag(UChar32 c) {
  // Only allow valid sequences from
  // http://www.unicode.org/reports/tr51/proposed.html#valid-emoji-tag-sequences
  return (c >= kTagDigitZero && c <= kTagDigitNine) ||
         (c >= kTagLatinSmallLetterA && c <= kTagLatinSmallLetterZ) ||
         c == kCancelTag;
}

template <typename CharacterType>
static inline String NormalizeSpacesInternal(const CharacterType* characters,
                                             unsigned length) {
  StringBuilder normalized;
  normalized.ReserveCapacity(length);

  for (unsigned i = 0; i < length; ++i)
    normalized.Append(Character::NormalizeSpaces(characters[i]));

  return normalized.ToString();
}

String Character::NormalizeSpaces(const LChar* characters, unsigned length) {
  return NormalizeSpacesInternal(characters, length);
}

String Character::NormalizeSpaces(const UChar* characters, unsigned length) {
  return NormalizeSpacesInternal(characters, length);
}

bool Character::IsCommonOrInheritedScript(UChar32 character) {
  ICUError status;
  UScriptCode script = uscript_getScript(character, &status);
  return U_SUCCESS(status) &&
         (script == USCRIPT_COMMON || script == USCRIPT_INHERITED);
}

bool Character::IsUnassignedOrPrivateUse(UChar32 character) {
  return WTF::Unicode::Category(character) &
         (WTF::Unicode::kOther_NotAssigned | WTF::Unicode::kOther_PrivateUse);
}

}  // namespace blink
