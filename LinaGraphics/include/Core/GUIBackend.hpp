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

#ifndef LinaEditorBackend_HPP
#define LinaEditorBackend_HPP

#include <LinaVG/Backends/BaseBackend.hpp>

namespace LinaVG::Backend
{
    class GUIBackend : public BaseBackend
    {
    public:
        virtual bool          Initialize() override;
        virtual void          Terminate() override;
        virtual void          StartFrame() override;
        virtual void          DrawGradient(GradientDrawBuffer* buf) override;
        virtual void          DrawTextured(TextureDrawBuffer* buf) override;
        virtual void          DrawDefault(DrawBuffer* buf) override;
        virtual void          DrawSimpleText(SimpleTextDrawBuffer* buf) override;
        virtual void          DrawSDFText(SDFTextDrawBuffer* buf) override;
        virtual void          EndFrame() override;
        virtual void          BufferFontTextureAtlas(int width, int height, int offsetX, int offsetY, unsigned char* data) override;
        virtual void          BindFontTexture(BackendHandle texture) override;
        virtual void          SaveAPIState() override;
        virtual void          RestoreAPIState() override;
        virtual BackendHandle CreateFontTexture(int width, int height) override;

    private:
    };

} // namespace LinaVG::Backend

#endif
