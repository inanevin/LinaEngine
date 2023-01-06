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

#include "Panels/DockPanel.hpp"
#include "GUI/GUI.hpp"
#include "Graphics/Core/Screen.hpp"
#include "Graphics/Core/RenderEngine.hpp"

namespace Lina::Editor
{
    void DockPanel::Initialize()
    {
    }

    void DockPanel::Shutdown()
    {
    }

    void DockPanel::Draw()
    {
        auto&         theme  = LGUI->GetTheme();
        const Vector2 screen = Graphics::RenderEngine::Get()->GetScreen().SizeF();
        constexpr const char* name   = "DockPanel";
        LGUI->SetWindowPosition(name, m_pos);
        LGUI->SetWindowSize(name, Vector2(screen.x, screen.y - m_pos.y));
        LGUI->SetAbsoluteDrawOrder(0);

        LGUI->SetWindowColor(name, theme.GetColor(ThemeColor::TopPanelBackground));
        if (LGUI->BeginWindow(name, IMW_MainSwapchain | IMW_NoMove | IMW_NoResize))
        {
            LGUI->EndWindow();
        }
    }

} // namespace Lina::Editor