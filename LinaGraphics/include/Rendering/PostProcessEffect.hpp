/*
This file is a part of: Lina Engine
https://github.com/inanevin/LinaEngine

Author: Inan Evin
http://www.inanevin.com

Copyright (c) [2018-2020] [Inan Evin]

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
Class: PostProcessEffect



Timestamp: 12/10/2020 6:02:07 PM
*/

#pragma once

#ifndef PostProcessEffect_HPP
#define PostProcessEffect_HPP

// Headers here.
#include "Core/RenderBackendFwd.hpp"
#include "Rendering/Material.hpp"
#include "Rendering/RenderTarget.hpp"
#include "Rendering/Shader.hpp"
#include "Rendering/Texture.hpp"

namespace Lina::Graphics
{

    class PostProcessEffect
    {

    public:
        PostProcessEffect() = default;
        ~PostProcessEffect() = default;

        void      Construct(Shader& shader);
        void      Draw(Texture* screenMap);
        Material& GetMaterial()
        {
            return m_material;
        }
        Texture& GetTexture()
        {
            return m_rtTexture;
        }
        RenderTarget& GetRenderTarget()
        {
            return m_renderTarget;
        }
        SamplerParameters& GetParams()
        {
            return m_samplerParams;
        }

    private:
        RenderDevice* m_renderDevice = nullptr;
        RenderEngine* m_renderEngine = nullptr;

        DrawParams        m_drawParams;
        Texture           m_rtTexture;
        RenderTarget      m_renderTarget;
        SamplerParameters m_samplerParams;
        Material          m_material;
    };
} // namespace Lina::Graphics

#endif
