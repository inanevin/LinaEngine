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
#include "Math/Vector.hpp"
#include "Grid.hpp"

namespace Lina
{
    namespace Graphics
    {
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

        inline void UpdateSwapchainInfo(uint32 currentSwapchainID, uint32 hoveredSwapchain, uint32 topMostSwapchain) override
        {
            m_currentSwapchainID = currentSwapchainID;
        }

    private:
        void DrawGrid();

    private:
        friend class EditorGUIManager;

        bool                     m_detached           = false;
        Recti                    m_gridRect           = Recti();
        uint32                   m_hoveredSwapchainID = 0;
        uint32                   m_topMostSwapchainID = 0;
        Vector<DockArea*>        m_dockAreas;
        Graphics::WindowManager* m_windowManager      = nullptr;
        Graphics::Swapchain*     m_swapchain          = nullptr;
        uint32                   m_currentSwapchainID = 0;
        Vector<Row>              m_rows;
    };

} // namespace Lina::Editor

#endif
