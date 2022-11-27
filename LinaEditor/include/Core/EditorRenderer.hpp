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

#ifndef EditorRenderer_HPP
#define EditorRenderer_HPP

#include "Data/HashMap.hpp"
#include "Utility/Graphics/TexturePacker.hpp"

namespace Lina
{
    namespace Event
    {
        struct EOnEditorDrawBegin;
        struct EOnEditorDraw;
        struct EOnEditorDrawEnd;
        struct ETick;
        struct EEngineResourcesLoaded;
    } // namespace Event

    namespace Graphics
    {
        class Texture;
    }
} // namespace Lina

namespace Lina::Editor
{
    class EditorRenderer
    {
    public:
        void Initialize();
        void Shutdown();

    private:
        void OnEditorDrawBegin(const Event::EOnEditorDrawBegin& ev);
        void OnEditorDraw(const Event::EOnEditorDraw& ev);
        void OnEditorDrawEnd(const Event::EOnEditorDrawEnd& ev);
        void OnEngineResourcesLoaded(const Event::EEngineResourcesLoaded& ev);

    private:
        Graphics::Texture*    m_iconTexture = nullptr;
        Vector<PackedTexture> m_packedIcons;
    };
} // namespace Lina::Editor

#endif
