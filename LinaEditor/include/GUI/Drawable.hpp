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

#ifndef Drawable_HPP
#define Drawable_HPP

#include "Math/Rect.hpp"
#include "Utility/StringId.hpp"

namespace Lina
{
    namespace Graphics
    {
        class Swapchain;
    } // namespace Graphics
} // namespace Lina

namespace Lina::Editor
{
    class Drawable
    {
    public:
        Drawable()          = default;
        virtual ~Drawable() = default;

        virtual void Initialize(){};
        virtual void Shutdown(){};
        virtual void Draw() = 0;
        virtual void SyncData(){};
        virtual void UpdateSwapchainInfo(uint32 currentSwapchainID, uint32 hoveredSwapchain, uint32 topMostSwapchain){};

        inline const Rect& GetRect()
        {
            return m_rect;
        }

        inline bool ShouldDestroy()
        {
            return m_shouldDestroy;
        }

    protected:
        bool     m_shouldDestroy = false;
        StringID m_sid           = 0;
        Rect     m_rect          = Rect();
    };
} // namespace Lina::Editor

#endif
