/*
This file is a part of: Lina Engine
https://github.com/inanevin/LinaEngine

Author: Inan Evin
http://www.inanevin.com

Copyright (c) [2018-2020] [Inan Evin]

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

/*
Class: MainLoopEvents



Timestamp: 12/25/2021 12:17:16 PM
*/

#pragma once

#ifndef MainLoopEvents_HPP
#define MainLoopEvents_HPP

// Headers here.
#include "Core/CommonApplication.hpp"

namespace Lina::Event
{
    struct EShutdown
    {
    };
    struct EStartGame
    {
    };
    struct EEndGame
    {
    };
    struct ETick
    {
        float m_deltaTime;
        bool  m_isInPlayMode;
    };
    struct EPreTick
    {
        float m_deltaTime;
        bool  m_isInPlayMode;
    };
    struct EPostTick
    {
        float m_deltaTime;
        bool  m_isInPlayMode;
    };
    struct ERender
    {
    };
    struct EPlayModeChanged
    {
        bool m_playMode;
    };
    struct EPauseModeChanged
    {
        bool m_isPaused;
    };
} // namespace Lina::Event

#endif
