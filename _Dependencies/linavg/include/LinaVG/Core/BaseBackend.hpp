/*
This file is a part of: LinaVG
https://github.com/inanevin/LinaVG

Author: Inan Evin
http://www.inanevin.com

Copyright (c) [2022-] [Inan Evin]

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
Class: GLBackend



Timestamp: 3/24/2022 11:33:52 PM
*/

#pragma once

#ifndef LinaVGBaseBackend_HPP
#define LinaVGBaseBackend_HPP

// Headers here.
#include "../Core/Common.hpp"

namespace LinaVG
{
    namespace Internal
    {
        class ShaderData;
    }
} // namespace LinaVG
namespace LinaVG::Backend
{

    class BaseBackend
    {
    public:
        BaseBackend()          = default;
        virtual ~BaseBackend() = default;

        static BaseBackend* Get()
        {
            return s_backend;
        }

        static void SetBackend(BaseBackend* ptr)
        {
            s_backend = ptr;
        }

        // Public API, if you want to implement your own backend, it needs to define these exact signatures.
        virtual bool          Initialize()                                                                                 = 0;
        virtual void          Terminate()                                                                                  = 0;
        virtual void          StartFrame(int threadCount)                                                                  = 0;
        virtual void          DrawGradient(GradientDrawBuffer* buf, int thread)                                            = 0;
        virtual void          DrawTextured(TextureDrawBuffer* buf, int thread)                                             = 0;
        virtual void          DrawDefault(DrawBuffer* buf, int thread)                                                     = 0;
        virtual void          DrawSimpleText(SimpleTextDrawBuffer* buf, int thread)                                        = 0;
        virtual void          DrawSDFText(SDFTextDrawBuffer* buf, int thread)                                              = 0;
        virtual void          EndFrame()                                                                                   = 0;
        virtual void          BufferFontTextureAtlas(int width, int height, int offsetX, int offsetY, unsigned char* data) = 0;
        virtual void          BindFontTexture(BackendHandle texture)                                                       = 0;
        virtual void          SaveAPIState()                                                                               = 0;
        virtual void          RestoreAPIState()                                                                            = 0;
        virtual BackendHandle CreateFontTexture(int width, int height)                                                     = 0;

        static BaseBackend* s_backend;
    };

} // namespace LinaVG::Backend

#endif
