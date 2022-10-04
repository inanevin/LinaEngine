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

#include "Core/GUIBackend.hpp"
#include "Core/RenderEngine.hpp"
#include <LinaVG/LinaVG.hpp>

namespace LinaVG::Backend
{

    bool GUIBackend::Initialize()
    {
        return true;
    }

    void GUIBackend::Terminate()
    {
    }

    void GUIBackend::StartFrame()
    {
    }

    void GUIBackend::DrawGradient(GradientDrawBuffer* buf)
    {
    }

    void GUIBackend::DrawTextured(TextureDrawBuffer* buf)
    {
    }

    void GUIBackend::DrawDefault(DrawBuffer* buf)
    {
        auto* mat = Lina::Graphics::RenderEngine::Get()->GetEngineMaterial(Lina::Graphics::EngineShaderType::GUIStandard);
    }

    void GUIBackend::DrawSimpleText(SimpleTextDrawBuffer* buf)
    {
    }
    void GUIBackend::DrawSDFText(SDFTextDrawBuffer* buf)
    {
    }
    void GUIBackend::EndFrame()
    {
    }
    void GUIBackend::BufferFontTextureAtlas(int width, int height, int offsetX, int offsetY, unsigned char* data)
    {
    }
    void GUIBackend::BindFontTexture(BackendHandle texture)
    {
    }
    void GUIBackend::SaveAPIState()
    {
    }
    void GUIBackend::RestoreAPIState()
    {
    }
    BackendHandle GUIBackend::CreateFontTexture(int width, int height)
    {
        return 0;
    }

} // namespace LinaVG::Backend
