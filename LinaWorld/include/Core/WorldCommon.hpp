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

#ifndef WorldCommon_HPP
#define WorldCommon_HPP

#include "Data/Bitmask.hpp"

namespace Lina::World
{

#define ENTITY_MAX         ((uint32_t)-2)
#define ENTITY_NULL        ((uint32_t)-1)
#define ENTITY_CHUNK_COUNT 25000
#define ENTITY_VEC_SIZE    2000

    enum EntityMask
    {
        Visible   = 1 << 0,
        Static    = 1 << 1,
        Reserved2 = 1 << 2,
        Reserved3 = 1 << 3,
        Reserved4 = 1 << 4,
        Reserved5 = 1 << 5,
        Reserved6 = 1 << 6,
    };

    enum ComponentMask
    {
        ReceiveOnGameStart       = 1 << 0,
        ReceiveOnGameEnd         = 1 << 1,
        ReceiveOnPreTick         = 1 << 2,
        ReceiveOnTick            = 1 << 3,
        ReceiveOnPostTick        = 1 << 4,
        ReceiveOnPostPhysicsTick = 1 << 5,
        Renderable               = 1 << 6,
    };

} // namespace Lina::World

#endif
