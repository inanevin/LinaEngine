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

/*
Class: TexturePanel



Timestamp: 1/4/2022 6:17:52 PM
*/

#pragma once

#ifndef TexturePanel_HPP
#define TexturePanel_HPP

// Headers here.
#include "Panels/EditorPanel.hpp"

namespace Lina
{
    namespace Graphics
    {
        class Texture;
    }
} // namespace Lina

namespace Lina::Editor
{
    class TexturePanel : public EditorPanel
    {

    public:
        TexturePanel()  = default;
        ~TexturePanel() = default;

        virtual void Initialize(const char* id, const char* icon) override;
        virtual void Draw() override;
        void         SetTargetTexture(Graphics::Texture* texture);

    private:
        void DrawTextureSettings();
        void DrawTexture();

    private:
        float              m_leftPaneWidth           = 0.0f;
        float              m_leftPaneMaxWidth        = 0.0f;
        float              m_leftPaneMinWidth        = 0.0f;
        float              m_resizeDividerPressedPos  = 0.0f;
        bool               m_draggingVerticalDivider = false;
        Graphics::Texture* m_targetTexture           = nullptr;
        Vector2            m_rightPaneSize           = Vector2::Zero;
    };
} // namespace Lina::Editor

#endif
