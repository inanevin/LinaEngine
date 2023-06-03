/*
This file is a part of: Lina Engine
https://github.com/inanevin/LinaEngine

Author: Inan Evin
http://www.inanevin.com

Copyright (c) [2018-] [Inan Evin]

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
*/

#pragma once
#ifndef InputMappings_HPP
#define InputMappings_HPP

#ifdef LINA_PLATFORM_WINDOWS
#include "Platform/Win32/Win32WindowsInclude.hpp"
#include <Xinput.h>
#endif

namespace Lina
{
#ifdef LINA_PLATFORM_WINDOWS


#define LINA_MOUSE_0      VK_LBUTTON
#define LINA_MOUSE_1      VK_RBUTTON
#define LINA_MOUSE_2      VK_MBUTTON
#define LINA_MOUSE_3      VK_XBUTTON1
#define LINA_MOUSE_4      VK_XBUTTON2
#define LINA_MOUSE_5      VK_LBUTTON
#define LINA_MOUSE_6      VK_LBUTTON
#define LINA_MOUSE_7      VK_LBUTTON
#define LINA_MOUSE_LAST   LINA_MOUSE_7
#define LINA_MOUSE_LEFT   LINA_MOUSE_0
#define LINA_MOUSE_RIGHT  LINA_MOUSE_1
#define LINA_MOUSE_MIDDLE LINA_MOUSE_2

#define LINA_GAMEPAD_A             VK_PAD_A
#define LINA_GAMEPAD_B             VK_PAD_B
#define LINA_GAMEPAD_X             VK_PAD_X
#define LINA_GAMEPAD_Y             VK_PAD_Y
#define LINA_GAMEPAD_LEFT_BUMPER   VK_PAD_LSHOULDER
#define LINA_GAMEPAD_RIGHT_BUMPER  VK_PAD_RSHOULDER
#define LINA_GAMEPAD_BACK          VK_PAD_BACK
#define LINA_GAMEPAD_START         VK_PAD_START
#define LINA_GAMEPAD_GUIDE         VK_PAD_START
#define LINA_GAMEPAD_LEFT_THUMB    VK_PAD_LTHUMB_PRESS
#define LINA_GAMEPAD_RIGHT_THUMB   VK_PAD_RTHUMB_PRESS
#define LINA_GAMEPAD_DPAD_UP       VK_PAD_DPAD_UP
#define LINA_GAMEPAD_DPAD_RIGHT    VK_PAD_DPAD_RIGHT
#define LINA_GAMEPAD_DPAD_DOWN     VK_PAD_DPAD_DOWN
#define LINA_GAMEPAD_DPAD_LEFT     VK_PAD_DPAD_LEFT
#define LINA_GAMEPAD_LEFT_TRIGGER  4
#define LINA_GAMEPAD_RIGHT_TRIGGER 5
#define LINA_GAMEPAD_LAST          LINA_GAMEPAD_RIGHT_TRIGGER

#define LINA_KEY_UNKNOWN -1
#define LINA_KEY_A       0x41
#define LINA_KEY_B       0x42
#define LINA_KEY_C       0x43
#define LINA_KEY_D       0x44
#define LINA_KEY_E       0x45
#define LINA_KEY_F       0x46
#define LINA_KEY_G       0x47
#define LINA_KEY_H       0x48
#define LINA_KEY_I       0x49
#define LINA_KEY_J       0x4A
#define LINA_KEY_K       0x4B
#define LINA_KEY_L       0x4C
#define LINA_KEY_M       0x4D
#define LINA_KEY_N       0x4E
#define LINA_KEY_O       0x4F
#define LINA_KEY_P       0x50
#define LINA_KEY_Q       0x51
#define LINA_KEY_R       0x52
#define LINA_KEY_S       0x53
#define LINA_KEY_T       0x54
#define LINA_KEY_U       0x55
#define LINA_KEY_V       0x56
#define LINA_KEY_W       0x57
#define LINA_KEY_X       0x58
#define LINA_KEY_Y       0x59
#define LINA_KEY_Z       0x5A

#define LINA_KEY_0 0x30
#define LINA_KEY_1 0x31
#define LINA_KEY_2 0x32
#define LINA_KEY_3 0x33
#define LINA_KEY_4 0x34
#define LINA_KEY_5 0x35
#define LINA_KEY_6 0x36
#define LINA_KEY_7 0x37
#define LINA_KEY_8 0x38
#define LINA_KEY_9 0x39

#define LINA_KEY_RETURN    VK_RETURN
#define LINA_KEY_ESCAPE    VK_ESCAPE
#define LINA_KEY_BACKSPACE VK_BACK
#define LINA_KEY_TAB       VK_TAB
#define LINA_KEY_SPACE     VK_SPACE

#define LINA_KEY_MINUS VK_OEM_MINUS

#define LINA_KEY_GRAVE VK_OEM_3 // 41 // ?

#define LINA_KEY_COMMA    VK_OEM_COMMA
#define LINA_KEY_PERIOD   VK_OEM_PERIOD
#define LINA_KEY_SLASH    VK_DIVIDE
#define LINA_KEY_CAPSLOCK VK_CAPITAL

#define LINA_KEY_F1  VK_F1
#define LINA_KEY_F2  VK_F2
#define LINA_KEY_F3  VK_F3
#define LINA_KEY_F4  VK_F4
#define LINA_KEY_F5  VK_F5
#define LINA_KEY_F6  VK_F6
#define LINA_KEY_F7  VK_F7
#define LINA_KEY_F8  VK_F8
#define LINA_KEY_F9  VK_F9
#define LINA_KEY_F10 VK_F10
#define LINA_KEY_F11 VK_F11
#define LINA_KEY_F12 VK_F12
#define LINA_KEY_F13 VK_F13
#define LINA_KEY_F14 VK_F14
#define LINA_KEY_F15 VK_F15
#define LINA_KEY_F16 VK_F16
#define LINA_KEY_F17 VK_F17
#define LINA_KEY_F18 VK_F18
#define LINA_KEY_F19 VK_F19
#define LINA_KEY_F20 VK_F20
#define LINA_KEY_F21 VK_F21
#define LINA_KEY_F22 VK_F22
#define LINA_KEY_F23 VK_F23
#define LINA_KEY_F24 VK_F24

#define LINA_KEY_LWIN VK_LWIN
#define LINA_KEY_RWIN VK_RWIN

#define LINA_KEY_PRINTSCREEN VK_PRINT
#define LINA_KEY_SCROLLLOCK  VK_SCROLL
#define LINA_KEY_PAUSE       VK_PAUSE
#define LINA_KEY_INSERT      VK_INSERT

#define LINA_KEY_HOME         VK_HOME
#define LINA_KEY_PAGEUP       VK_PRIOR
#define LINA_KEY_DELETE       VK_DELETE
#define LINA_KEY_END          VK_END
#define LINA_KEY_PAGEDOWN     VK_NEXT
#define LINA_KEY_RIGHT        VK_RIGHT
#define LINA_KEY_LEFT         VK_LEFT
#define LINA_KEY_DOWN         VK_DOWN
#define LINA_KEY_UP           VK_UP
#define LINA_KEY_NUMLOCKCLEAR VK_CLEAR

#define LINA_KEY_KP_DECIMAL  VK_DECIMAL
#define LINA_KEY_KP_DIVIDE   VK_DIVIDE
#define LINA_KEY_KP_MULTIPLY VK_MULTIPLY
#define LINA_KEY_KP_MINUS    VK_OEM_MINUS
#define LINA_KEY_KP_PLUS     VK_OEM_PLUS
#define LINA_KEY_KP_ENTER    VK_RETURN
#define LINA_KEY_KP_1        VK_NUMPAD1
#define LINA_KEY_KP_2        VK_NUMPAD2
#define LINA_KEY_KP_3        VK_NUMPAD3
#define LINA_KEY_KP_4        VK_NUMPAD4
#define LINA_KEY_KP_5        VK_NUMPAD5
#define LINA_KEY_KP_6        VK_NUMPAD6
#define LINA_KEY_KP_7        VK_NUMPAD7
#define LINA_KEY_KP_8        VK_NUMPAD8
#define LINA_KEY_KP_9        VK_NUMPAD9
#define LINA_KEY_KP_0        VK_NUMPAD0

#define LINA_KEY_LCTRL  VK_LCONTROL
#define LINA_KEY_LSHIFT VK_LSHIFT
#define LINA_KEY_LALT   VK_LMENU
#define LINA_KEY_LGUI   VK_LMENU
#define LINA_KEY_RCTRL  VK_RCONTROL
#define LINA_KEY_RSHIFT VK_RSHIFT
#define LINA_KEY_RALT   VK_RMENU
#define LINA_KEY_RGUI   VK_RMENU

#else

#define LINA_MOUSE_0      0
#define LINA_MOUSE_1      1
#define LINA_MOUSE_2      2
#define LINA_MOUSE_3      3
#define LINA_MOUSE_4      4
#define LINA_MOUSE_5      5
#define LINA_MOUSE_6      6
#define LINA_MOUSE_7      7
#define LINA_MOUSE_LAST   LINA_MOUSE_7
#define LINA_MOUSE_LEFT   LINA_MOUSE_0
#define LINA_MOUSE_RIGHT  LINA_MOUSE_1
#define LINA_MOUSE_MIDDLE LINA_MOUSE_2

#define LINA_GAMEPAD_A             0
#define LINA_GAMEPAD_B             1
#define LINA_GAMEPAD_X             2
#define LINA_GAMEPAD_Y             3
#define LINA_GAMEPAD_LEFT_BUMPER   4
#define LINA_GAMEPAD_RIGHT_BUMPER  5
#define LINA_GAMEPAD_BACK          6
#define LINA_GAMEPAD_START         7
#define LINA_GAMEPAD_GUIDE         8
#define LINA_GAMEPAD_LEFT_THUMB    9
#define LINA_GAMEPAD_RIGHT_THUMB   10
#define LINA_GAMEPAD_DPAD_UP       11
#define LINA_GAMEPAD_DPAD_RIGHT    12
#define LINA_GAMEPAD_DPAD_DOWN     13
#define LINA_GAMEPAD_DPAD_LEFT     14
#define LINA_GAMEPAD_LEFT_TRIGGER  4
#define LINA_GAMEPAD_RIGHT_TRIGGER 5
#define LINA_GAMEPAD_LAST          LINA_GAMEPAD_RIGHT_TRIGGER

#define LINA_KEY_UNKNOWN -1
#define LINA_KEY_A       65
#define LINA_KEY_B       66
#define LINA_KEY_C       67
#define LINA_KEY_D       68
#define LINA_KEY_E       69
#define LINA_KEY_F       70
#define LINA_KEY_G       71
#define LINA_KEY_H       72
#define LINA_KEY_I       73
#define LINA_KEY_J       74
#define LINA_KEY_K       75
#define LINA_KEY_L       76
#define LINA_KEY_M       77
#define LINA_KEY_N       78
#define LINA_KEY_O       79
#define LINA_KEY_P       80
#define LINA_KEY_Q       81
#define LINA_KEY_R       82
#define LINA_KEY_S       83
#define LINA_KEY_T       84
#define LINA_KEY_U       85
#define LINA_KEY_V       86
#define LINA_KEY_W       87
#define LINA_KEY_X       88
#define LINA_KEY_Y       89
#define LINA_KEY_Z       90

#define LINA_KEY_0 48
#define LINA_KEY_1 49
#define LINA_KEY_2 50
#define LINA_KEY_3 51
#define LINA_KEY_4 52
#define LINA_KEY_5 53
#define LINA_KEY_6 54
#define LINA_KEY_7 55
#define LINA_KEY_8 56
#define LINA_KEY_9 57

#define LINA_KEY_RETURN    257
#define LINA_KEY_ESCAPE    256
#define LINA_KEY_BACKSPACE 259
#define LINA_KEY_TAB       258
#define LINA_KEY_SPACE     32

#define LINA_KEY_MINUS 45
#define LINA_KEY_GRAVE 96

#define LINA_KEY_COMMA    44
#define LINA_KEY_PERIOD   46
#define LINA_KEY_SLASH    47
#define LINA_KEY_CAPSLOCK 280

#define LINA_KEY_F1  290
#define LINA_KEY_F2  291
#define LINA_KEY_F3  292
#define LINA_KEY_F4  293
#define LINA_KEY_F5  294
#define LINA_KEY_F6  295
#define LINA_KEY_F7  296
#define LINA_KEY_F8  297
#define LINA_KEY_F9  298
#define LINA_KEY_F10 299
#define LINA_KEY_F11 300
#define LINA_KEY_F12 301

#define LINA_KEY_PRINTSCREEN LINA_KEY_UNKNOWN
#define LINA_KEY_SCROLLLOCK  281
#define LINA_KEY_PAUSE       284
#define LINA_KEY_INSERT      260

#define LINA_KEY_HOME         268
#define LINA_KEY_PAGEUP       266
#define LINA_KEY_DELETE       261
#define LINA_KEY_END          269
#define LINA_KEY_PAGEDOWN     267
#define LINA_KEY_RIGHT        262
#define LINA_KEY_LEFT         263
#define LINA_KEY_DOWN         264
#define LINA_KEY_UP           265
#define LINA_KEY_NUMLOCKCLEAR 282

#define LINA_KEY_KP_DECIMAL  330
#define LINA_KEY_KP_DIVIDE   331
#define LINA_KEY_KP_MULTIPLY 332
#define LINA_KEY_KP_MINUS    333
#define LINA_KEY_KP_PLUS     334
#define LINA_KEY_KP_ENTER    335
#define LINA_KEY_KP_1        321
#define LINA_KEY_KP_2        322
#define LINA_KEY_KP_3        323
#define LINA_KEY_KP_4        324
#define LINA_KEY_KP_5        325
#define LINA_KEY_KP_6        326
#define LINA_KEY_KP_7        327
#define LINA_KEY_KP_8        328
#define LINA_KEY_KP_9        329
#define LINA_KEY_KP_0        320

#define LINA_KEY_F13 302
#define LINA_KEY_F14 303
#define LINA_KEY_F15 304
#define LINA_KEY_F16 305
#define LINA_KEY_F17 306
#define LINA_KEY_F18 307
#define LINA_KEY_F19 308
#define LINA_KEY_F20 309
#define LINA_KEY_F21 310
#define LINA_KEY_F22 311
#define LINA_KEY_F23 312
#define LINA_KEY_F24 313

#define LINA_KEY_LCTRL  341
#define LINA_KEY_LSHIFT 340
#define LINA_KEY_LALT   342
#define LINA_KEY_LGUI   343
#define LINA_KEY_RCTRL  345
#define LINA_KEY_RSHIFT 344
#define LINA_KEY_RALT   346
#define LINA_KEY_RGUI   347
#endif

    enum InputCode
    {
        KeyUnknown        = LINA_KEY_UNKNOWN,
        KeyA              = LINA_KEY_A,
        KeyB              = LINA_KEY_B,
        KeyC              = LINA_KEY_C,
        KeyD              = LINA_KEY_D,
        KeyE              = LINA_KEY_E,
        KeyF              = LINA_KEY_F,
        KeyG              = LINA_KEY_G,
        KeyH              = LINA_KEY_H,
        KeyI              = LINA_KEY_I,
        KeyJ              = LINA_KEY_J,
        KeyK              = LINA_KEY_K,
        KeyL              = LINA_KEY_L,
        KeyM              = LINA_KEY_M,
        KeyN              = LINA_KEY_N,
        KeyO              = LINA_KEY_O,
        KeyP              = LINA_KEY_P,
        KeyQ              = LINA_KEY_Q,
        KeyR              = LINA_KEY_R,
        KeyS              = LINA_KEY_S,
        KeyT              = LINA_KEY_T,
        KeyU              = LINA_KEY_U,
        KeyV              = LINA_KEY_V,
        KeyW              = LINA_KEY_W,
        KeyX              = LINA_KEY_X,
        KeyY              = LINA_KEY_Y,
        KeyZ              = LINA_KEY_Z,
        KeyAlpha0         = LINA_KEY_0,
        KeyAlpha1         = LINA_KEY_1,
        KeyAlpha2         = LINA_KEY_2,
        KeyAlpha3         = LINA_KEY_3,
        KeyAlpha4         = LINA_KEY_4,
        KeyAlpha5         = LINA_KEY_5,
        KeyAlpha6         = LINA_KEY_6,
        KeyAlpha7         = LINA_KEY_7,
        KeyAlpha8         = LINA_KEY_8,
        KeyAlpha9         = LINA_KEY_9,
        KeyReturn         = LINA_KEY_RETURN,
        KeyEscape         = LINA_KEY_ESCAPE,
        KeyBackspace      = LINA_KEY_BACKSPACE,
        KeyTab            = LINA_KEY_TAB,
        KeySpace          = LINA_KEY_SPACE,
        KeyMinus          = LINA_KEY_MINUS,
        KeyGrave          = LINA_KEY_GRAVE,
        KeyComma          = LINA_KEY_COMMA,
        KeyPeriod         = LINA_KEY_PERIOD,
        KeySlash          = LINA_KEY_SLASH,
        KeyCapslock       = LINA_KEY_CAPSLOCK,
        KeyF1             = LINA_KEY_F1,
        KeyF2             = LINA_KEY_F2,
        KeyF3             = LINA_KEY_F3,
        KeyF4             = LINA_KEY_F4,
        KeyF5             = LINA_KEY_F5,
        KeyF6             = LINA_KEY_F6,
        KeyF7             = LINA_KEY_F7,
        KeyF8             = LINA_KEY_F8,
        KeyF9             = LINA_KEY_F9,
        KeyF10            = LINA_KEY_F10,
        KeyF11            = LINA_KEY_F11,
        KeyF12            = LINA_KEY_F12,
        KeyPrintscreen    = LINA_KEY_PRINTSCREEN,
        KeyScrolllock     = LINA_KEY_SCROLLLOCK,
        KeyPause          = LINA_KEY_PAUSE,
        KeyInsert         = LINA_KEY_INSERT,
        KeyHome           = LINA_KEY_HOME,
        KeyPageup         = LINA_KEY_PAGEUP,
        KeyDelete         = LINA_KEY_DELETE,
        KeyEnd            = LINA_KEY_END,
        KeyPagedown       = LINA_KEY_PAGEDOWN,
        KeyRight          = LINA_KEY_RIGHT,
        KeyLeft           = LINA_KEY_LEFT,
        KeyDown           = LINA_KEY_DOWN,
        KeyUp             = LINA_KEY_UP,
        KeyNumlockClear   = LINA_KEY_NUMLOCKCLEAR,
        KeyKeypadDecimal  = LINA_KEY_KP_DECIMAL,
        KeyKeypadDivide   = LINA_KEY_KP_DIVIDE,
        KeyKeypadMultiply = LINA_KEY_KP_MULTIPLY,
        KeyKeypadMinus    = LINA_KEY_KP_MINUS,
        KeyKeypadPlus     = LINA_KEY_KP_PLUS,
        KeyKeypadEnter    = LINA_KEY_KP_ENTER,
        KeyKeypad1        = LINA_KEY_KP_1,
        KeyKeypad2        = LINA_KEY_KP_2,
        KeyKeypad3        = LINA_KEY_KP_3,
        KeyKeypad4        = LINA_KEY_KP_4,
        KeyKeypad5        = LINA_KEY_KP_5,
        KeyKeypad6        = LINA_KEY_KP_6,
        KeyKeypad7        = LINA_KEY_KP_7,
        KeyKeypad8        = LINA_KEY_KP_8,
        KeyKeypad9        = LINA_KEY_KP_9,
        KeyKeypad0        = LINA_KEY_KP_0,
        KeyF13            = LINA_KEY_F13,
        KeyF14            = LINA_KEY_F14,
        KeyF15            = LINA_KEY_F15,
        KeyF16            = LINA_KEY_F16,
        KeyF17            = LINA_KEY_F17,
        KeyF18            = LINA_KEY_F18,
        KeyF19            = LINA_KEY_F19,
        KeyF20            = LINA_KEY_F20,
        KeyF21            = LINA_KEY_F21,
        KeyF22            = LINA_KEY_F22,
        KeyF23            = LINA_KEY_F23,
        KeyF24            = LINA_KEY_F24,
        KeyLCTRL          = LINA_KEY_LCTRL,
        KeyLSHIFT         = LINA_KEY_LSHIFT,
        KeyLALT           = LINA_KEY_LALT,
        KeyLGUI           = LINA_KEY_LGUI,
        KeyRCTRL          = LINA_KEY_RCTRL,
        KeyRSHIFT         = LINA_KEY_RSHIFT,
        KeyRALT           = LINA_KEY_RALT,
        KeyRGUI           = LINA_KEY_RGUI,

        MouseUnknown    = LINA_KEY_UNKNOWN,
        Mouse0          = LINA_MOUSE_0,
        Mouse1          = LINA_MOUSE_1,
        Mouse2          = LINA_MOUSE_2,
        Mouse3          = LINA_MOUSE_3,
        Mouse4          = LINA_MOUSE_4,
        Mouse5          = LINA_MOUSE_5,
        Mouse6          = LINA_MOUSE_6,
        Mouse7          = LINA_MOUSE_7,
        MouseLast       = LINA_MOUSE_LAST,
        MouseLeft       = LINA_MOUSE_LEFT,
        MouseRight      = LINA_MOUSE_RIGHT,
        MouseMiddle     = LINA_MOUSE_MIDDLE,
        JoystickUnknown = LINA_KEY_UNKNOWN,

        GamepadUnknown   = LINA_KEY_UNKNOWN,
        GamepadA         = LINA_GAMEPAD_A,
        GamepadB         = LINA_GAMEPAD_B,
        GamepadX         = LINA_GAMEPAD_X,
        GamepadY         = LINA_GAMEPAD_Y,
        GamepadLBumper   = LINA_GAMEPAD_LEFT_BUMPER,
        GamepadRBumper   = LINA_GAMEPAD_RIGHT_BUMPER,
        GamepadBack      = LINA_GAMEPAD_BACK,
        GamepadStart     = LINA_GAMEPAD_START,
        GamepadGuide     = LINA_GAMEPAD_GUIDE,
        GamepadLThumb    = LINA_GAMEPAD_LEFT_THUMB,
        GamepadRThumb    = LINA_GAMEPAD_RIGHT_THUMB,
        GamepadDpadUp    = LINA_GAMEPAD_DPAD_UP,
        GamepadDpadRight = LINA_GAMEPAD_DPAD_RIGHT,
        GamepadDpadDown  = LINA_GAMEPAD_DPAD_DOWN,
        GamepadDpadLeft  = LINA_GAMEPAD_DPAD_LEFT,
        GamepadDpadLast  = LINA_GAMEPAD_LAST,
        GamepadLTrigger  = LINA_GAMEPAD_LEFT_TRIGGER,
        GamepadRTrigger  = LINA_GAMEPAD_RIGHT_TRIGGER,
        GamepadLast      = LINA_GAMEPAD_LAST
    };

} // namespace Lina::Input

#endif
