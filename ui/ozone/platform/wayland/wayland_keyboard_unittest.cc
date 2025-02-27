// Copyright 2017 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include <linux/input.h>
#include <wayland-server.h>

#include "testing/gmock/include/gmock/gmock.h"
#include "testing/gtest/include/gtest/gtest.h"
#include "ui/events/event.h"
#include "ui/ozone/platform/wayland/fake_server.h"
#include "ui/ozone/platform/wayland/wayland_test.h"
#include "ui/ozone/platform/wayland/wayland_window.h"
#include "ui/ozone/test/mock_platform_window_delegate.h"

#if BUILDFLAG(USE_XKBCOMMON)
#include "base/memory/free_deleter.h"
#include "base/memory/shared_memory.h"
#include "ui/events/keycodes/scoped_xkb.h"  // nogncheck
#endif

using ::testing::SaveArg;
using ::testing::_;

namespace ui {

class WaylandKeyboardTest : public WaylandTest {
 public:
  WaylandKeyboardTest() {}

  void SetUp() override {
    WaylandTest::SetUp();

    wl_seat_send_capabilities(server.seat()->resource(),
                              WL_SEAT_CAPABILITY_KEYBOARD);

    Sync();

    keyboard = server.seat()->keyboard.get();
    ASSERT_TRUE(keyboard);

#if BUILDFLAG(USE_XKBCOMMON)
    // Set up XKB bits and set the keymap to the client.
    xkb_context_.reset(xkb_context_new(XKB_CONTEXT_NO_FLAGS));
    xkb_keymap_.reset(xkb_keymap_new_from_names(
        xkb_context_.get(), nullptr /*names*/, XKB_KEYMAP_COMPILE_NO_FLAGS));
    xkb_state_.reset(xkb_state_new(xkb_keymap_.get()));

    std::unique_ptr<char, base::FreeDeleter> keymap_string(
        xkb_keymap_get_as_string(xkb_keymap_.get(), XKB_KEYMAP_FORMAT_TEXT_V1));
    DCHECK(keymap_string.get());
    size_t keymap_size = strlen(keymap_string.get()) + 1;
    base::SharedMemory shared_keymap;
    bool rv = shared_keymap.CreateAndMapAnonymous(keymap_size);
    DCHECK(rv);
    memcpy(shared_keymap.memory(), keymap_string.get(), keymap_size);
    wl_keyboard_send_keymap(keyboard->resource(),
                            WL_KEYBOARD_KEYMAP_FORMAT_XKB_V1,
                            shared_keymap.handle().GetHandle(), keymap_size);
#endif
  }

 protected:
  wl::MockKeyboard* keyboard;

 private:
#if BUILDFLAG(USE_XKBCOMMON)
  // The Xkb state used for the keyboard.
  std::unique_ptr<xkb_context, ui::XkbContextDeleter> xkb_context_;
  std::unique_ptr<xkb_keymap, ui::XkbKeymapDeleter> xkb_keymap_;
  std::unique_ptr<xkb_state, ui::XkbStateDeleter> xkb_state_;
#endif

  DISALLOW_COPY_AND_ASSIGN(WaylandKeyboardTest);
};

ACTION_P(CloneEvent, ptr) {
  *ptr = Event::Clone(*arg0);
}

TEST_P(WaylandKeyboardTest, Keypress) {
  struct wl_array empty;
  wl_array_init(&empty);
  wl_keyboard_send_enter(keyboard->resource(), 1, surface->resource(), &empty);
  wl_array_release(&empty);

  wl_keyboard_send_key(keyboard->resource(), 2, 0, 30 /* a */,
                       WL_KEYBOARD_KEY_STATE_PRESSED);

  std::unique_ptr<Event> event;
  EXPECT_CALL(delegate, DispatchEvent(_)).WillOnce(CloneEvent(&event));

  Sync();
  ASSERT_TRUE(event);
  ASSERT_TRUE(event->IsKeyEvent());

  auto* key_event = event->AsKeyEvent();
  EXPECT_EQ(ui::VKEY_A, key_event->key_code());
  EXPECT_EQ(ET_KEY_PRESSED, key_event->type());

  wl_keyboard_send_leave(keyboard->resource(), 3, surface->resource());

  Sync();

  wl_keyboard_send_key(keyboard->resource(), 3, 0, 30 /* a */,
                       WL_KEYBOARD_KEY_STATE_PRESSED);
  EXPECT_CALL(delegate, DispatchEvent(_)).Times(0);
}

TEST_P(WaylandKeyboardTest, AltModifierKeypress) {
  struct wl_array empty;
  wl_array_init(&empty);
  wl_keyboard_send_enter(keyboard->resource(), 1, surface->resource(), &empty);
  wl_array_release(&empty);

  // Alt
  wl_keyboard_send_key(keyboard->resource(), 2, 0, 56 /* left Alt */,
                       WL_KEYBOARD_KEY_STATE_PRESSED);

  std::unique_ptr<Event> event;
  EXPECT_CALL(delegate, DispatchEvent(_)).WillOnce(CloneEvent(&event));

  Sync();
  ASSERT_TRUE(event);
  ASSERT_TRUE(event->IsKeyEvent());

  auto* key_event = event->AsKeyEvent();

  EXPECT_EQ(ui::EF_ALT_DOWN, key_event->flags());
  EXPECT_EQ(ui::VKEY_MENU, key_event->key_code());
  EXPECT_EQ(ET_KEY_PRESSED, key_event->type());
}

TEST_P(WaylandKeyboardTest, ControlModifierKeypress) {
  struct wl_array empty;
  wl_array_init(&empty);
  wl_keyboard_send_enter(keyboard->resource(), 1, surface->resource(), &empty);
  wl_array_release(&empty);

  // Control
  wl_keyboard_send_key(keyboard->resource(), 2, 0, 29 /* left Control */,
                       WL_KEYBOARD_KEY_STATE_PRESSED);

  std::unique_ptr<Event> event;
  EXPECT_CALL(delegate, DispatchEvent(_)).WillOnce(CloneEvent(&event));

  Sync();
  ASSERT_TRUE(event);
  ASSERT_TRUE(event->IsKeyEvent());

  auto* key_event = event->AsKeyEvent();

  EXPECT_EQ(ui::EF_CONTROL_DOWN, key_event->flags());
  EXPECT_EQ(ui::VKEY_CONTROL, key_event->key_code());
  EXPECT_EQ(ET_KEY_PRESSED, key_event->type());
}

TEST_P(WaylandKeyboardTest, ShiftModifierKeypress) {
  struct wl_array empty;
  wl_array_init(&empty);
  wl_keyboard_send_enter(keyboard->resource(), 1, surface->resource(), &empty);
  wl_array_release(&empty);

  // Shift
  wl_keyboard_send_key(keyboard->resource(), 2, 0, 42 /* left Shift */,
                       WL_KEYBOARD_KEY_STATE_PRESSED);

  std::unique_ptr<Event> event;
  EXPECT_CALL(delegate, DispatchEvent(_)).WillOnce(CloneEvent(&event));

  Sync();
  ASSERT_TRUE(event);
  ASSERT_TRUE(event->IsKeyEvent());

  auto* key_event = event->AsKeyEvent();

  EXPECT_EQ(ui::EF_SHIFT_DOWN, key_event->flags());
  EXPECT_EQ(ui::VKEY_SHIFT, key_event->key_code());
  EXPECT_EQ(ET_KEY_PRESSED, key_event->type());
}

TEST_P(WaylandKeyboardTest, ControlShiftModifiers) {
  struct wl_array empty;
  wl_array_init(&empty);
  wl_array_init(&empty);
  wl_keyboard_send_enter(keyboard->resource(), 1, surface->resource(), &empty);
  wl_array_release(&empty);

  // Pressing control.
  wl_keyboard_send_key(keyboard->resource(), 2, 0, 29 /* Control */,
                       WL_KEYBOARD_KEY_STATE_PRESSED);

  std::unique_ptr<Event> event;
  EXPECT_CALL(delegate, DispatchEvent(_)).WillOnce(CloneEvent(&event));
  Sync();

  wl_keyboard_send_modifiers(keyboard->resource(), 3, 4 /* mods_depressed*/,
                             0 /* mods_latched */, 0 /* mods_locked */,
                             0 /* group */);
  Sync();

  // Pressing shift (with control key still held down).
  wl_keyboard_send_key(keyboard->resource(), 4, 0, 42 /* Shift */,
                       WL_KEYBOARD_KEY_STATE_PRESSED);

  std::unique_ptr<Event> event2;
  EXPECT_CALL(delegate, DispatchEvent(_)).WillOnce(CloneEvent(&event2));
  Sync();

  wl_keyboard_send_modifiers(keyboard->resource(), 5, 5 /* mods_depressed*/,
                             0 /* mods_latched */, 0 /* mods_locked */,
                             0 /* group */);
  Sync();

  // Sending a reguard keypress, eg 'a'.
  wl_keyboard_send_key(keyboard->resource(), 6, 0, 30 /* a */,
                       WL_KEYBOARD_KEY_STATE_PRESSED);

  std::unique_ptr<Event> event3;
  EXPECT_CALL(delegate, DispatchEvent(_)).WillOnce(CloneEvent(&event3));
  Sync();

  ASSERT_TRUE(event3);
  ASSERT_TRUE(event3->IsKeyEvent());

  auto* key_event3 = event3->AsKeyEvent();

  EXPECT_EQ(ui::EF_CONTROL_DOWN | ui::EF_SHIFT_DOWN, key_event3->flags());
  EXPECT_EQ(ui::VKEY_A, key_event3->key_code());
  EXPECT_EQ(ET_KEY_PRESSED, key_event3->type());
}

INSTANTIATE_TEST_CASE_P(XdgVersionV5Test,
                        WaylandKeyboardTest,
                        ::testing::Values(kXdgShellV5));
INSTANTIATE_TEST_CASE_P(XdgVersionV6Test,
                        WaylandKeyboardTest,
                        ::testing::Values(kXdgShellV6));

}  // namespace ui
