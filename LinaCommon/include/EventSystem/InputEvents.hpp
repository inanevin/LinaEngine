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

#ifndef InputEvents_HPP
#define InputEvents_HPP

// Headers here.
#include "Core/SizeDefinitions.hpp"

namespace Lina::Event
{
    struct EKeyCallback
    {
        void*  window   = nullptr;
        uint32 key      = 0;
        int    scancode = 0;
        int    action   = 0;
    };

    struct EMouseButtonCallback
    {
        void* window = nullptr;
        int   button = 0;
        int   action = 0;
        int   mods   = 0;
    };
    struct EMouseScrollCallback
    {
        void* window = nullptr;
        int   xoff   = 0;
        int   yoff   = 0;
    };

    struct EMouseMoved
    {
    };

    struct EMouseMovedRaw
    {
        int xDelta = 0;
        int yDelta = 0;
    };

} // namespace Lina::Event

#endif
