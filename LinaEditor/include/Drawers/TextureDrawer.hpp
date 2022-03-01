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
Class: TextureDrawer



Timestamp: 1/9/2022 11:54:02 PM
*/

#pragma once

#ifndef TextureDrawer_HPP
#define TextureDrawer_HPP

// Headers here.

namespace Lina
{
    class Vector2;

    namespace Graphics
    {
     class Texture;   
    }
}
namespace Lina::Editor
{
    class TextureDrawer
    {

    public:
        TextureDrawer()  = default;
        ~TextureDrawer() = default;

        static void DrawTextureSettings(Graphics::Texture*& texture, float paneWidth);
        static void DrawTexture(Graphics::Texture* texture, const Vector2& bgMin, const Vector2& bgMax);

    private:
    };
} // namespace Lina

#endif
