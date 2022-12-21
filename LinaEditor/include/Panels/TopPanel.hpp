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

#ifndef TopPanel_HPP
#define TopPanel_HPP

#include "Panel.hpp"
#include "Utility/StringId.hpp"
#include "GUI/CustomWidgets/MenuBar.hpp"
#include "Utility/Graphics/TexturePacker.hpp"

namespace Lina
{
    namespace Graphics
    {
        class Texture;
    }
} // namespace Lina

namespace Lina::Editor
{
    class TopPanel : public Panel
    {
    public:
        TopPanel()          = default;
        virtual ~TopPanel() = default;

        virtual void   Initialize() override;
        virtual void   Shutdown() override;
        virtual void   Draw() override;
        
        inline Vector2 GetCurrentSize()
        {
            return m_currentSize;
        }

    private:
        void DrawFileMenu();
        void DrawLinaLogo();
        void DrawButtons();
        void DrawControls();
        void OnMenuBarItemClicked(uint32 id);

    private:
        MenuBar               m_menuBar;
        Vector2               m_currentSize         = Vector2();
        uint32                m_titleTexture        = 0;
        uint32                m_textAnimationIndex  = 0;
        float                 m_titleAspect         = 0.0f;
        float                 m_lastTextAnimTime    = 0.0f;
        float                 m_fileMenuMaxX        = 0.0f;
        float                 m_titleMaxX           = 0.0f;
        StringID              m_minimizeSid         = 0;
        StringID              m_maximizeSid         = 0;
        StringID              m_closeSid            = 0;
        StringID              m_restoreSid          = 0;
        Graphics::Texture*    m_packedAnim          = nullptr;
        Vector<PackedTexture> m_packedAnimTextures;
    };
} // namespace Lina::Editor

#endif