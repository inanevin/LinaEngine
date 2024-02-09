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

#include "Core/Graphics/GUI/GUIBackend.hpp"
#include "Core/Graphics/GfxManager.hpp"
#include "Core/Graphics/Resource/Texture.hpp"
#include "Core/Graphics/Resource/Font.hpp"
#include "Core/Graphics/Pipeline/Buffer.hpp"
#include "Core/Resources/ResourceManager.hpp"
#include "Common/System/System.hpp"

namespace Lina
{
	GUIBackend::GUIBackend(GfxManager* man)
	{
		m_resourceManager = man->GetSystem()->CastSubsystem<ResourceManager>(SubsystemType::ResourceManager);
	}

	void GUIBackend::Terminate()
	{
		for (const auto& ft : m_fontTextures)
		{
			m_resourceManager->DestroyUserResource<Texture>(ft.texture);
			delete[] ft.pixels;
		}

		m_fontTextures.clear();
	}

	void GUIBackend::StartFrame(int threadCount)
	{
		const int32 currentSz = static_cast<int32>(m_drawData.size());
		m_buffers.resize(threadCount);
		m_drawData.resize(threadCount);
	}

	void GUIBackend::DrawGradient(LinaVG::GradientDrawBuffer* buf, int threadIndex)
	{
		auto& req				= AddDrawRequest(buf, threadIndex);
		req.materialData.color1 = buf->m_color.start;
		req.materialData.color2 = buf->m_color.end;

		if (buf->m_color.gradientType == LinaVG::GradientType::Horizontal)
		{
			req.materialData.floatPack1 = Vector4(1.0f, 0.0f, 0.0f, 0.0f);
			req.materialData.floatPack2 = Vector4(0.0f, buf->m_isAABuffer ? 1.0f : 0.0f, buf->m_color.radialSize, static_cast<float>(buf->m_drawBufferType));
		}
		else if (buf->m_color.gradientType == LinaVG::GradientType::Vertical)
		{
			req.materialData.floatPack1 = Vector4(0.0f, 1.0f, 0.0f, 0.0f);
			req.materialData.floatPack2 = Vector4(0.0f, buf->m_isAABuffer ? 1.0f : 0.0f, buf->m_color.radialSize, static_cast<float>(buf->m_drawBufferType));
		}
		else if (buf->m_color.gradientType == LinaVG::GradientType::Radial)
		{
			req.materialData.floatPack1 = Vector4(0.0f, 0.0f, 0.5f, 0.5f);
			req.materialData.floatPack2 = Vector4(1.0f, buf->m_isAABuffer ? 1.0f : 0.0f, buf->m_color.radialSize, static_cast<float>(buf->m_drawBufferType));
		}
		else if (buf->m_color.gradientType == LinaVG::GradientType::RadialCorner)
		{
			req.materialData.floatPack1 = Vector4(0.0f, 0.0f, 0.5f, 0.5f);
			req.materialData.floatPack2 = Vector4(1.0f, buf->m_isAABuffer ? 1.0f : 0.0f, buf->m_color.radialSize, static_cast<float>(buf->m_drawBufferType));
		}
	}

	void GUIBackend::DrawTextured(LinaVG::TextureDrawBuffer* buf, int threadIndex)
	{
		auto& req					= AddDrawRequest(buf, threadIndex);
		auto  txt					= m_resourceManager->GetResource<Texture>(buf->m_textureHandle);
		auto  sampler				= m_resourceManager->GetResource<TextureSampler>(txt->GetSamplerSID());
		req.materialData.floatPack1 = Vector4(buf->m_textureUVTiling.x, buf->m_textureUVTiling.y, buf->m_textureUVOffset.x, buf->m_textureUVOffset.y);
		req.materialData.floatPack2 = Vector4(buf->m_isAABuffer, 0.0f, 0.0f, static_cast<float>(buf->m_drawBufferType));
		// req.materialData.diffuse.textureIndex = txt->GetBindlessIndex();
		// req.materialData.diffuse.samplerIndex = sampler->GetBindlessIndex();
		req.materialData.color1 = buf->m_tint;
	}

	void GUIBackend::DrawDefault(LinaVG::DrawBuffer* buf, int threadIndex)
	{
		auto& req = AddDrawRequest(buf, threadIndex);
	}

	void GUIBackend::DrawSimpleText(LinaVG::SimpleTextDrawBuffer* buf, int threadIndex)
	{
		auto& req					  = AddDrawRequest(buf, threadIndex);
		auto  txt					  = m_fontTextures[buf->m_textureHandle].texture;
		auto  sampler				  = m_resourceManager->GetResource<TextureSampler>(DEFAULT_GUI_TEXT_SAMPLER_SID);
		req.materialData.floatPack2.w = static_cast<float>(buf->m_drawBufferType);

		// req.materialData.diffuse.textureIndex = txt->GetBindlessIndex();
		// req.materialData.diffuse.samplerIndex = sampler->GetBindlessIndex();
	}

	void GUIBackend::DrawSDFText(LinaVG::SDFTextDrawBuffer* buf, int threadIndex)
	{
		auto& req					= AddDrawRequest(buf, threadIndex);
		auto  txt					= m_fontTextures[buf->m_textureHandle].texture;
		auto  sampler				= m_resourceManager->GetResource<TextureSampler>(DEFAULT_GUI_TEXT_SAMPLER_SID);
		req.materialData.color1		= buf->m_outlineColor;
		req.materialData.floatPack1 = Vector4(buf->m_thickness, buf->m_softness, buf->m_outlineThickness, buf->m_outlineThickness != 0.0f ? 1.0f : 0.0f);
		req.materialData.floatPack2 = Vector4(buf->m_flipAlpha ? 1.0f : 0.0f, 0.0f, 0.0f, static_cast<float>(buf->m_drawBufferType));
		// req.materialData.diffuse.textureIndex = txt->GetBindlessIndex();
		// req.materialData.diffuse.samplerIndex = sampler->GetBindlessIndex();
	}

	void GUIBackend::BufferFontTextureAtlas(int width, int height, int offsetX, int offsetY, unsigned char* data)
	{
		auto&  ft		   = m_fontTextures[m_boundFontTexture];
		uint32 startOffset = offsetY * ft.width + offsetX;

		for (int i = 0; i < height; i++)
		{
			const uint32 size = width;
			MEMCPY(ft.pixels + startOffset, &data[width * i], size);
			startOffset += ft.width;
		}
	}

	void GUIBackend::BufferEnded()
	{
		auto& ft = m_fontTextures[m_boundFontTexture];
		ft.texture->SetCustomData(ft.pixels, ft.width, ft.height, 1, LinaGX::Format::R8_UNORM);
	}

	void GUIBackend::BindFontTexture(LinaVG::BackendHandle texture)
	{
		m_boundFontTexture = texture;
	}

	LinaVG::BackendHandle GUIBackend::CreateFontTexture(int width, int height)
	{
		const String name		 = "GUI Backend Font Texture " + TO_STRING(m_fontTextures.size());
		FontTexture	 fontTexture = {
			 .texture = m_resourceManager->CreateUserResource<Texture>(name, TO_SID(name)),
			 .width	  = width,
			 .height  = height,
		 };

		fontTexture.pixels = new uint8[width * height];

		m_fontTextures.push_back(fontTexture);
		m_boundFontTexture = static_cast<LinaVG::BackendHandle>(m_fontTextures.size() - 1);
		return m_boundFontTexture;
	}

	GUIBackend::DrawRequest& GUIBackend::AddDrawRequest(LinaVG::DrawBuffer* buf, int threadIndex)
	{
		auto& buffers  = m_buffers[threadIndex];
		auto& drawData = m_drawData[threadIndex];

		drawData.drawRequests.push_back(DrawRequest());
		DrawRequest& req = drawData.drawRequests.back();
		req.indexCount	 = static_cast<uint32>(buf->m_indexBuffer.m_size);
		req.vertexOffset = buffers.vertexCounter;
		req.firstIndex	 = buffers.indexCounter;
		req.clip.pos	 = Vector2ui(buf->clipPosX, buf->clipPosY);
		req.clip.size	 = Vector2ui(buf->clipSizeX, buf->clipSizeY);

		buffers.vertexBuffer->BufferData(buffers.vertexCounter * sizeof(LinaVG::Vertex), (uint8*)buf->m_vertexBuffer.m_data, buf->m_vertexBuffer.m_size * sizeof(LinaVG::Vertex));
		buffers.indexBuffer->BufferData(buffers.indexCounter * sizeof(LinaVG::Index), (uint8*)buf->m_indexBuffer.m_data, buf->m_indexBuffer.m_size * sizeof(LinaVG::Index));
		buffers.indexCounter += req.indexCount;
		buffers.vertexCounter += static_cast<uint32>(buf->m_vertexBuffer.m_size);
		return req;
	}

	Matrix4 GUIBackend::GetProjectionFromSize(const Vector2ui& size)
	{
		float		L	 = static_cast<float>(0.0f);
		float		R	 = static_cast<float>(size.x);
		float		B	 = static_cast<float>(size.y);
		float		T	 = static_cast<float>(0.0);
		const float zoom = 1.0f;
		L *= zoom;
		R *= zoom;
		T *= zoom;
		B *= zoom;
		return Matrix4::Orthographic(L, R, B, T, 0.0f, 1.0f);
	}

} // namespace Lina
