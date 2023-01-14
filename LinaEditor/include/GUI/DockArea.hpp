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

#ifndef DockArea_HPP
#define DockArea_HPP

#include "Drawable.hpp"
#include "Data/Vector.hpp"
#include "Math/Vector.hpp"

namespace Lina
{
    namespace Graphics
    {
        class Swapchain;
        class Window;
        class WindowManager;
    } // namespace Graphics
} // namespace Lina

namespace Lina::Editor
{
    class DockedWindow;
    class EditorGUIManager;

    class DockArea : public Drawable
    {
    public:
        DockArea()          = default;
        virtual ~DockArea() = default;

        virtual void Draw() override;
        virtual void SyncData() override;

        inline void UpdateCurrentSwapchainID(StringID sid) override
        {
            m_shouldDraw = m_swapchainID == sid;
        }

    private:
        friend class EditorGUIManager;

        bool                     m_shouldDraw         = false;
        bool                     m_detached           = false;
        bool                     m_isSwapchainHovered = false;
        bool                     m_isTopMost          = false;
        StringID                 m_swapchainID        = 0;
        Vector<DockArea*>        m_dockAreas;
        Vector<Drawable*>        m_content;
        uint32                   m_selectedContent = 0;
        Graphics::WindowManager* m_windowManager   = nullptr;
        Vector<Vector2>          m_windowPositionsNext;
        Vector<CursorType>       m_cursorsNext;
        bool                     m_draggingMove     = false;
        bool                     m_draggingResize   = false;
        Vector2                  m_mouseDiffOnPress = Vector2::Zero;
    };

} // namespace Lina::Editor

#endif
