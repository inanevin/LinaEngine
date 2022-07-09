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

#ifndef GraphicsEvents_HPP
#define GraphicsEvents_HPP

// Headers here.
#include "Math/Color.hpp"
#include "Math/Quaternion.hpp"

namespace Lina::Event
{
    struct EPreRender
    {
    };
    struct EPostSceneDraw
    {
    };
    struct EPostRender
    {
    };
    struct EGUIRender
    {
    };
    struct ECustomRender
    {
    };
    struct EDrawLine
    {
        Vector3 from      = Vector3();
        Vector3 to        = Vector3();
        Color   color     = Color();
        float   lineWidth = 0.0f;
    };
    struct EDrawBox
    {
        Vector3 position    = Vector3();
        Vector3 halfExtents = Vector3();
        Color   color       = Color();
        float   lineWidth   = 0.0f;
    };
    struct EDrawSphere
    {
        Vector3 position  = Vector3();
        float   radius    = 0.0f;
        Color   color     = Color();
        float   lineWidth = 0.0f;
    };
    struct EDrawHemiSphere
    {
        Vector3 position  = Vector3();
        float   radius    = 0.0f;
        Color   color     = Color();
        float   lineWidth = 0.0f;
        bool    top       = false;
    };
    struct EDrawCapsule
    {
        Vector3 position  = Vector3();
        float   radius    = 0.0f;
        Color   color     = Color();
        float   height    = 0.0f;
        float   lineWidth = 0.0f;
    };
    struct EDrawCircle
    {
        Vector3    position  = Vector3();
        float      radius    = 0.0f;
        Color      color     = Color();
        float      lineWidth = 0.0f;
        bool       half      = false;
        Quaternion rotation  = Quaternion();
    };
    struct ESwapchainRecreated
    {
        void* oldPtr = nullptr;
        void* newPtr = nullptr;
    };
} // namespace Lina::Event

#endif
