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

#include "Rendering/PostProcessEffect.hpp"
#include "Core/RenderEngineBackend.hpp"
#include "Helpers/DrawParameterHelper.hpp"

namespace Lina::Graphics
{


	void PostProcessEffect::Construct(Shader& shader)
	{
		m_renderEngine= RenderEngineBackend::Get();
		m_renderDevice = m_renderEngine->GetRenderDevice();
		m_samplerParams.m_textureParams.m_pixelFormat = PixelFormat::FORMAT_RGB;
		m_samplerParams.m_textureParams.m_internalPixelFormat = PixelFormat::FORMAT_RGBA16F;
		m_samplerParams.m_textureParams.m_minFilter = m_samplerParams.m_textureParams.m_magFilter = SamplerFilter::FILTER_LINEAR;
		m_samplerParams.m_textureParams.m_wrapS = m_samplerParams.m_textureParams.m_wrapT = SamplerWrapMode::WRAP_CLAMP_EDGE;
		m_rtTexture.ConstructRTTexture(m_renderEngine->GetViewportSize(), m_samplerParams, false);
		m_renderTarget.Construct(m_rtTexture, m_renderEngine->GetViewportSize(), TextureBindMode::BINDTEXTURE_TEXTURE2D, FrameBufferAttachment::ATTACHMENT_COLOR);
		m_drawParams = DrawParameterHelper::GetFullScreenQuad();
		Material::SetMaterialShader(m_material, shader);
	}

	void PostProcessEffect::Draw(Texture* screenMap)
	{
		// Select FBO
		m_renderDevice->SetFBO(m_renderTarget.GetID());

		// Clear color bit.
		m_renderDevice->Clear(true, true, true, Color::White, 0xFF);

		// Setup material & use.
		m_material.SetTexture(MAT_MAP_SCREEN, screenMap);
		
		m_renderEngine->UpdateShaderData(&m_material);
		m_renderDevice->Draw(m_renderEngine->GetScreenQuadVAO(), m_drawParams, 0, 6, true);
	}
}