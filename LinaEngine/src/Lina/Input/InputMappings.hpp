/*
Author: Inan Evin
www.inanevin.com

Copyright 2018 Inan Evin

Licensed under the Apache License, Version 2.0 (the "License"); you may not use this file except in compliance with the License. You may obtain a copy of the License at

http://www.apache.org/licenses/LICENSE-2.0

Unless required by applicable law or agreed to in writing, software distributed under the License is distributed on an "AS IS" BASIS, 
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied. See the License for the specific language governing permissions 
and limitations under the License.

Class: InputMappings
Timestamp: 1/6/2019 4:33:31 AM

*/

#pragma once
#ifndef InputMappings_HPP
#define InputMappings_HPP

namespace LinaEngine
{

#define NUM_KEY_STATES 380
#define NUM_MOUSE_STATES 8

#ifdef LLF_INPUTANDWINDOW_SDL

#define LINA_MOUSE_1         1
#define LINA_MOUSE_2         2
#define LINA_MOUSE_3         3
#define LINA_MOUSE_4         4
#define LINA_MOUSE_5         5
/*#define LINA_MOUSE_6         5
#define LINA_MOUSE_7         6
#define LINA_MOUSE_8         7
#define LINA_MOUSE_LAST      LINA_MOUSE_8*/
#define LINA_MOUSE_LEFT      LINA_MOUSE_1
#define LINA_MOUSE_RIGHT     LINA_MOUSE_2
#define LINA_MOUSE_MIDDLE    LINA_MOUSE_3

#define LINA_JOYSTICK_1             0
#define LINA_JOYSTICK_2             1
#define LINA_JOYSTICK_3             2
#define LINA_JOYSTICK_4             3
#define LINA_JOYSTICK_5             4
#define LINA_JOYSTICK_6             5
#define LINA_JOYSTICK_7             6
#define LINA_JOYSTICK_8             7
#define LINA_JOYSTICK_9             8
#define LINA_JOYSTICK_10            9
#define LINA_JOYSTICK_11            10
#define LINA_JOYSTICK_12            11
#define LINA_JOYSTICK_13            12
#define LINA_JOYSTICK_14            13
#define LINA_JOYSTICK_15            14
#define LINA_JOYSTICK_16            15
#define LINA_JOYSTICK_LAST          LINA_JOYSTICK_16

#define LINA_GAMEPAD_A               0
#define LINA_GAMEPAD_B               1
#define LINA_GAMEPAD_X               2
#define LINA_GAMEPAD_Y               3
#define LINA_GAMEPAD_LEFT_BUMPER     9
#define LINA_GAMEPAD_RIGHT_BUMPER    10
#define LINA_GAMEPAD_BACK            4
#define LINA_GAMEPAD_START           6
#define LINA_GAMEPAD_GUIDE           5
#define LINA_GAMEPAD_LEFT_THUMB      7
#define LINA_GAMEPAD_RIGHT_THUMB     8
#define LINA_GAMEPAD_DPAD_UP         11
#define LINA_GAMEPAD_DPAD_RIGHT      12
#define LINA_GAMEPAD_DPAD_DOWN       13
#define LINA_GAMEPAD_DPAD_LEFT       14
#define LINA_GAMEPAD_LAST            GLFW_GAMEPAD_DPAD_LEFT

#define LINA_GAMEPAD_CROSS       LINA_GAMEPAD_A
#define LINA_GAMEPAD_CIRCLE      LINA_GAMEPAD_B
#define LINA_GAMEPAD_SQUARE      LINA_GAMEPAD_X
#define LINA_GAMEPAD_TRIANGLE    LINA_GAMEPAD_Y

#define LINA_GAMEPAD_LEFT_X        0
#define LINA_GAMEPAD_LEFT_Y        1
#define LINA_GAMEPAD_RIGHT_X       2
#define LINA_GAMEPAD_RIGHT_Y       3
#define LINA_GAMEPAD_LEFT_TRIGGER  4
#define LINA_GAMEPAD_RIGHT_TRIGGER 5
#define LINA_GAMEPAD_LAST          LINA_GAMEPAD_RIGHT_TRIGGER

#define LINA_KEY_UNKNOWN 0
#define LINA_KEY_A 4
#define LINA_KEY_B 5
#define LINA_KEY_C 6
#define LINA_KEY_D 7
#define LINA_KEY_E 8
#define LINA_KEY_F 9
#define LINA_KEY_G 10
#define LINA_KEY_H 11
#define LINA_KEY_I 12
#define LINA_KEY_J 13
#define LINA_KEY_K 14
#define LINA_KEY_L 15
#define LINA_KEY_M 16
#define LINA_KEY_N 17
#define LINA_KEY_O 18
#define LINA_KEY_P 19
#define LINA_KEY_Q 20
#define LINA_KEY_R 21
#define LINA_KEY_S 22
#define LINA_KEY_T 23
#define LINA_KEY_U 24
#define LINA_KEY_V 25
#define LINA_KEY_W 26
#define LINA_KEY_X 27
#define LINA_KEY_Y 28
#define LINA_KEY_Z 29

#define LINA_KEY_1 30
#define LINA_KEY_2 31
#define LINA_KEY_3 32
#define LINA_KEY_4 33
#define LINA_KEY_5 34
#define LINA_KEY_6 35
#define LINA_KEY_7 36
#define LINA_KEY_8 37
#define LINA_KEY_9 38
#define LINA_KEY_0 39

#define LINA_KEY_RETURN 40
#define LINA_KEY_ESCAPE 41
#define LINA_KEY_BACKSPACE 42
#define LINA_KEY_TAB 43
#define LINA_KEY_SPACE 44

#define LINA_KEY_MINUS 45
#define LINA_KEY_EQUALS 46
#define LINA_KEY_LEFTBRACKET 47
#define LINA_KEY_RIGHTBRACKET 48
#define LINA_KEY_BACKSLASH 49

#define LINA_KEY_SEMICOLON 51
#define LINA_KEY_APOSTROPHE 52
#define LINA_KEY_GRAVE 53

#define LINA_KEY_COMMA 54
#define LINA_KEY_PERIOD 55
#define LINA_KEY_SLASH 56
#define LINA_KEY_CAPSLOCK 57

#define LINA_KEY_F1 58
#define LINA_KEY_F2 59
#define LINA_KEY_F3 60
#define LINA_KEY_F4 61
#define LINA_KEY_F5 62
#define LINA_KEY_F6 63
#define LINA_KEY_F7 64
#define LINA_KEY_F8 65
#define LINA_KEY_F9 66
#define LINA_KEY_F10 67
#define LINA_KEY_F11 68
#define LINA_KEY_F12 69

#define LINA_KEY_PRINTSCREEN 70
#define LINA_KEY_SCROLLLOCK 71
#define LINA_KEY_PAUSE 72
#define LINA_KEY_INSERT 73

#define LINA_KEY_HOME 74
#define LINA_KEY_PAGEUP 75
#define LINA_KEY_DELETE 76
#define LINA_KEY_END 77
#define LINA_KEY_PAGEDOWN 78
#define LINA_KEY_RIGHT 79
#define LINA_KEY_LEFT 80
#define LINA_KEY_DOWN 81
#define LINA_KEY_UP 82
#define LINA_KEY_NUMLOCKCLEAR 83

#define LINA_KEY_KP_DECIMAL 220
#define LINA_KEY_KP_DIVIDE 84
#define LINA_KEY_KP_MULTIPLY 85
#define LINA_KEY_KP_MINUS 86
#define LINA_KEY_KP_PLUS 87
#define LINA_KEY_KP_ENTER 88
#define LINA_KEY_KP_1 89
#define LINA_KEY_KP_2 90
#define LINA_KEY_KP_3 91
#define LINA_KEY_KP_4 92
#define LINA_KEY_KP_5 93
#define LINA_KEY_KP_6 94
#define LINA_KEY_KP_7 95
#define LINA_KEY_KP_8 96
#define LINA_KEY_KP_9 97
#define LINA_KEY_KP_0 98


#define LINA_KEY_F13 104
#define LINA_KEY_F14 105
#define LINA_KEY_F15 106
#define LINA_KEY_F16 107
#define LINA_KEY_F17 108
#define LINA_KEY_F18 109
#define LINA_KEY_F19 110
#define LINA_KEY_F20 111
#define LINA_KEY_F21 112
#define LINA_KEY_F22 113
#define LINA_KEY_F23 114
#define LINA_KEY_F24 115

#define LINA_KEY_LCTRL 224
#define LINA_KEY_LSHIFT 225
#define LINA_KEY_LALT 226 
#define LINA_KEY_LGUI 227 
#define LINA_KEY_RCTRL 228
#define LINA_KEY_RSHIFT 229
#define LINA_KEY_RALT 230
#define LINA_KEY_RGUI 231 


#elif LLF_INPUTANDWINDOW_GLFW

#define LINA_MOUSE_1         0
#define LINA_MOUSE_2         1
#define LINA_MOUSE_3         2
#define LINA_MOUSE_4         3
#define LINA_MOUSE_5         4
#define LINA_MOUSE_6         5
#define LINA_MOUSE_7         6
#define LINA_MOUSE_8         7
#define LINA_MOUSE_LAST      LINA_MOUSE_8
#define LINA_MOUSE_LEFT      LINA_MOUSE_1
#define LINA_MOUSE_RIGHT     LINA_MOUSE_2
#define LINA_MOUSE_MIDDLE    LINA_MOUSE_3

#define LINA_JOYSTICK_1             0
#define LINA_JOYSTICK_2             1
#define LINA_JOYSTICK_3             2
#define LINA_JOYSTICK_4             3
#define LINA_JOYSTICK_5             4
#define LINA_JOYSTICK_6             5
#define LINA_JOYSTICK_7             6
#define LINA_JOYSTICK_8             7
#define LINA_JOYSTICK_9             8
#define LINA_JOYSTICK_10            9
#define LINA_JOYSTICK_11            10
#define LINA_JOYSTICK_12            11
#define LINA_JOYSTICK_13            12
#define LINA_JOYSTICK_14            13
#define LINA_JOYSTICK_15            14
#define LINA_JOYSTICK_16            15
#define LINA_JOYSTICK_LAST          LINA_JOYSTICK_16

#define LINA_GAMEPAD_A               0
#define LINA_GAMEPAD_B               1
#define LINA_GAMEPAD_X               2
#define LINA_GAMEPAD_Y               3
#define LINA_GAMEPAD_LEFT_BUMPER     4
#define LINA_GAMEPAD_RIGHT_BUMPER    5
#define LINA_GAMEPAD_BACK            6
#define LINA_GAMEPAD_START           7
#define LINA_GAMEPAD_GUIDE           8
#define LINA_GAMEPAD_LEFT_THUMB      9
#define LINA_GAMEPAD_RIGHT_THUMB     10
#define LINA_GAMEPAD_DPAD_UP         11
#define LINA_GAMEPAD_DPAD_RIGHT      12
#define LINA_GAMEPAD_DPAD_DOWN       13
#define LINA_GAMEPAD_DPAD_LEFT       14
#define LINA_GAMEPAD_LAST            GLFW_GAMEPAD_DPAD_LEFT

#define LINA_GAMEPAD_CROSS       LINA_GAMEPAD_A
#define LINA_GAMEPAD_CIRCLE      LINA_GAMEPAD_B
#define LINA_GAMEPAD_SQUARE      LINA_GAMEPAD_X
#define LINA_GAMEPAD_TRIANGLE    LINA_GAMEPAD_Y

#define LINA_GAMEPAD_LEFT_X        0
#define LINA_GAMEPAD_LEFT_Y        1
#define LINA_GAMEPAD_RIGHT_X       2
#define LINA_GAMEPAD_RIGHT_Y       3
#define LINA_GAMEPAD_LEFT_TRIGGER  4
#define LINA_GAMEPAD_RIGHT_TRIGGER 5
#define LINA_GAMEPAD_LAST          LINA_GAMEPAD_RIGHT_TRIGGER

#define LINA_KEY_UNKNOWN -1
#define LINA_KEY_A 65
#define LINA_KEY_B 66
#define LINA_KEY_C 67
#define LINA_KEY_D 68
#define LINA_KEY_E 69
#define LINA_KEY_F 70
#define LINA_KEY_G 71
#define LINA_KEY_H 72
#define LINA_KEY_I 73
#define LINA_KEY_J 74
#define LINA_KEY_K 75
#define LINA_KEY_L 76
#define LINA_KEY_M 77
#define LINA_KEY_N 78
#define LINA_KEY_O 79
#define LINA_KEY_P 80
#define LINA_KEY_Q 81
#define LINA_KEY_R 82
#define LINA_KEY_S 83
#define LINA_KEY_T 84
#define LINA_KEY_U 85
#define LINA_KEY_V 86
#define LINA_KEY_W 87
#define LINA_KEY_X 88
#define LINA_KEY_Y 89
#define LINA_KEY_Z 90

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

#define LINA_KEY_RETURN 257
#define LINA_KEY_ESCAPE 256
#define LINA_KEY_BACKSPACE 259
#define LINA_KEY_TAB 258
#define LINA_KEY_SPACE 32

#define LINA_KEY_MINUS 45
#define LINA_KEY_EQUALS 61
#define LINA_KEY_LEFTBRACKET 91
#define LINA_KEY_RIGHTBRACKET 93
#define LINA_KEY_BACKSLASH 92


#define LINA_KEY_SEMICOLON 59
#define LINA_KEY_APOSTROPHE 39
#define LINA_KEY_GRAVE 96

#define LINA_KEY_COMMA 44
#define LINA_KEY_PERIOD 46
#define LINA_KEY_SLASH 47
#define LINA_KEY_CAPSLOCK 280

#define LINA_KEY_F1 290
#define LINA_KEY_F2 291
#define LINA_KEY_F3 292
#define LINA_KEY_F4 293
#define LINA_KEY_F5 294
#define LINA_KEY_F6 295
#define LINA_KEY_F7 296
#define LINA_KEY_F8 297
#define LINA_KEY_F9 298
#define LINA_KEY_F10 299
#define LINA_KEY_F11 300
#define LINA_KEY_F12 301

#define LINA_KEY_PRINTSCREEN LINA_KEY_UNKNOWN
#define LINA_KEY_SCROLLLOCK 281
#define LINA_KEY_PAUSE 284
#define LINA_KEY_INSERT 260

#define LINA_KEY_HOME 268
#define LINA_KEY_PAGEUP 266
#define LINA_KEY_DELETE 261
#define LINA_KEY_END 269
#define LINA_KEY_PAGEDOWN 267
#define LINA_KEY_RIGHT 262
#define LINA_KEY_LEFT 263
#define LINA_KEY_DOWN 264
#define LINA_KEY_UP 265
#define LINA_KEY_NUMLOCKCLEAR 282

#define LINA_KEY_KP_DECIMAL 330
#define LINA_KEY_KP_DIVIDE 331
#define LINA_KEY_KP_MULTIPLY 332
#define LINA_KEY_KP_MINUS 333
#define LINA_KEY_KP_PLUS 334
#define LINA_KEY_KP_ENTER 335
#define LINA_KEY_KP_1 321
#define LINA_KEY_KP_2 322
#define LINA_KEY_KP_3 323
#define LINA_KEY_KP_4 324
#define LINA_KEY_KP_5 325
#define LINA_KEY_KP_6 326
#define LINA_KEY_KP_7 327
#define LINA_KEY_KP_8 328
#define LINA_KEY_KP_9 329
#define LINA_KEY_KP_0 320

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

#define LINA_KEY_LCTRL 341
#define LINA_KEY_LSHIFT 340
#define LINA_KEY_LALT 342
#define LINA_KEY_LGUI 343
#define LINA_KEY_RCTRL 345
#define LINA_KEY_RSHIFT 344
#define LINA_KEY_RALT 346
#define LINA_KEY_RGUI 347


#endif

struct LinaKey
{
	const static int Unknown = LINA_KEY_UNKNOWN;
	const static int A = LINA_KEY_A;
	const static int B = LINA_KEY_B;
	const static int C = LINA_KEY_C;
	const static int D = LINA_KEY_D;
	const static int E = LINA_KEY_E;
	const static int F = LINA_KEY_F;
	const static int G = LINA_KEY_G;
	const static int H = LINA_KEY_H;
	const static int I = LINA_KEY_I;
	const static int J = LINA_KEY_J;
	const static int K = LINA_KEY_K;
	const static int L = LINA_KEY_L;
	const static int M = LINA_KEY_M;
	const static int N = LINA_KEY_N;
	const static int O = LINA_KEY_O;
	const static int P = LINA_KEY_P;
	const static int Q = LINA_KEY_Q;
	const static int R = LINA_KEY_R;
	const static int S = LINA_KEY_S;
	const static int T = LINA_KEY_T;
	const static int U = LINA_KEY_U;
	const static int V = LINA_KEY_V;
	const static int W = LINA_KEY_W;
	const static int X = LINA_KEY_X;
	const static int Y = LINA_KEY_Y;
	const static int Z = LINA_KEY_Z;
	const static int Alpha0 = LINA_KEY_0;
	const static int Alpha1 = LINA_KEY_1;
	const static int Alpha2 = LINA_KEY_2;
	const static int Alpha3 = LINA_KEY_3;
	const static int Alpha4 = LINA_KEY_4;
	const static int Alpha5 = LINA_KEY_5;
	const static int Alpha6 = LINA_KEY_6;
	const static int Alpha7 = LINA_KEY_7;
	const static int Alpha8 = LINA_KEY_8;
	const static int Alpha8 = LINA_KEY_9;
	const static int Return = LINA_KEY_RETURN;
	const static int Escape = LINA_KEY_ESCAPE;
	const static int Backspace = LINA_KEY_BACKSPACE;
	const static int Tab = LINA_KEY_TAB;
	const static int Space = LINA_KEY_SPACE;
	const static int Minus = LINA_KEY_MINUS;
	const static int Equals = LINA_KEY_EQUALS;
	const static int Leftbracket = LINA_KEY_LEFTBRACKET;
	const static int Rightbracket = LINA_KEY_RIGHTBRACKET;
	const static int Backslash = LINA_KEY_BACKSLASH;
	const static int Semicolon = LINA_KEY_SEMICOLON;
	const static int Apostrophe = LINA_KEY_APOSTROPHE;
	const static int Grave = LINA_KEY_GRAVE;
	const static int Comma = LINA_KEY_COMMA;
	const static int Period = LINA_KEY_PERIOD;
	const static int Slash = LINA_KEY_SLASH;
	const static int Capslock = LINA_KEY_CAPSLOCK;
	const static int F1 = LINA_KEY_F1;
	const static int F2 = LINA_KEY_F2;
	const static int F3 = LINA_KEY_F3;
	const static int F4 = LINA_KEY_F4;
	const static int F5 = LINA_KEY_F5;
	const static int F6 = LINA_KEY_F6;
	const static int F7 = LINA_KEY_F7;
	const static int F8 = LINA_KEY_F8;
	const static int F9 = LINA_KEY_F9;
	const static int F10 = LINA_KEY_F10;
	const static int F11 = LINA_KEY_F11;
	const static int F12 = LINA_KEY_F12;
	const static int Printscreen = LINA_KEY_PRINTSCREEN;
	const static int Scrolllock = LINA_KEY_SCROLLLOCK;
	const static int Pause = LINA_KEY_PAUSE;
	const static int Insert = LINA_KEY_INSERT;
	const static int Home = LINA_KEY_HOME;
	const static int Pageup = LINA_KEY_PAGEUP;
	const static int Delete = LINA_KEY_DELETE;
	const static int End = LINA_KEY_END;
	const static int Pagedown = LINA_KEY_PAGEDOWN;
	const static int Right = LINA_KEY_RIGHT;
	const static int Left = LINA_KEY_LEFT;
	const static int Down = LINA_KEY_DOWN;
	const static int Up = LINA_KEY_UP;
	const static int NumlockClear = LINA_KEY_NUMLOCKCLEAR;
	const static int KeypadDecimal = LINA_KEY_KP_DECIMAL;
	const static int KeypadDivide = LINA_KEY_KP_DIVIDE;
	const static int KeypadMultiply = LINA_KEY_KP_MULTIPLY;
	const static int KeypadMinus = LINA_KEY_KP_MINUS;
	const static int KeypadPlus = LINA_KEY_KP_PLUS;
	const static int KeypadEnter = LINA_KEY_KP_ENTER;
	const static int Keypad1 = LINA_KEY_KP_1; 
	const static int Keypad2 = LINA_KEY_KP_2; 
	const static int Keypad3 = LINA_KEY_KP_3; 
	const static int Keypad4 = LINA_KEY_KP_4; 
	const static int Keypad5 = LINA_KEY_KP_5; 
	const static int Keypad6 = LINA_KEY_KP_6; 
	const static int Keypad7 = LINA_KEY_KP_7; 
	const static int Keypad8 = LINA_KEY_KP_8; 
	const static int Keypad9 = LINA_KEY_KP_9; 
	const static int Keypad0 = LINA_KEY_KP_0; 
	const static int F13 = LINA_KEY_F13; 
	const static int F14 = LINA_KEY_F14; 
	const static int F15 = LINA_KEY_F15; 
	const static int F16 = LINA_KEY_F16; 
	const static int F17 = LINA_KEY_F17; 
	const static int F18 = LINA_KEY_F18; 
	const static int F19 = LINA_KEY_F19; 
	const static int F20 = LINA_KEY_F20; 
	const static int F21 = LINA_KEY_F21; 
	const static int F22 = LINA_KEY_F22; 
	const static int F23= LINA_KEY_F23; 
	const static int F24 = LINA_KEY_F24; 
	const static int LCTRL = LINA_KEY_LCTRL;
	const static int LSHIFT = LINA_KEY_LSHIFT;
	const static int LALT = LINA_KEY_LALT;
	const static int LGUI = LINA_KEY_LGUI;
	const static int RCTRL = LINA_KEY_RCTRL;
	const static int RSHIFT = LINA_KEY_RSHIFT;
	const static int RALT = LINA_KEY_RALT;
	const static int RGUI = LINA_KEY_RGUI;
};
	
struct LinaMouse
{
const static int  Mouse1 = LINA_MOUSE_1;       
const static int  Mouse2 = LINA_MOUSE_2;       
const static int  Mouse3 = LINA_MOUSE_3;       
const static int  Mouse4 = LINA_MOUSE_4;       
const static int  Mouse5 = LINA_MOUSE_5;       
const static int  Mouse6 = LINA_MOUSE_6;       
const static int  Mouse7 = LINA_MOUSE_7;       
const static int  Mouse8 = LINA_MOUSE_8;       
const static int  MouseLast = LINA_MOUSE_LAST;    
const static int  MouseLeft = LINA_MOUSE_LEFT;     
const static int  MouseRight = LINA_MOUSE_RIGHT;    
const static int  MouseMiddle = LINA_MOUSE_MIDDLE;   
};

struct LinaJoystick
{

};

struct LinaGamepad
{

};
}


#endif