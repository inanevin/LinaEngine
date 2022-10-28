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

#include "GUI/GImage.hpp"
#include "Core/Screen.hpp"

namespace Lina::Editor
{
    void GImage::ParseFromJSON(json& data)
    {
        const String texture = data.value("Texture", "Resources/Engine/Textures/DefaultLina.png").c_str();
        textureHandle        = TO_SID(texture);

        GNode::ParseFromJSON(data);
    }

    void GImage::Draw(float dt)
    {
        const bool aa            = LinaVG::Config.aaEnabled;
        LinaVG::Config.aaEnabled = aaEnabled;

        LinaVG::DrawImage(textureHandle, LV2(_min), LV2(_max), 0.0f, _drawOrder);

        LinaVG::Config.aaEnabled = aa;

        GNode::Draw(dt);
    }

} // namespace Lina::Editor
