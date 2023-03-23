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

#include "Graphics/Core/GUIBackend.hpp"
#include "Graphics/Core/GUIRenderer.hpp"
#include "Graphics/Resource/Texture.hpp"
#include "Graphics/Resource/Font.hpp"
#include "Graphics/Platform/RendererIncl.hpp"
#include "Graphics/Core/GfxManager.hpp"
#include "System/ISystem.hpp"
#include "Resources/Core/CommonResources.hpp"

namespace Lina
{
	GUIBackend::GUIBackend(GfxManager* man)
	{
		m_gfxManager	  = man;
		m_renderer		  = m_gfxManager->GetRenderer();
		m_resourceManager = m_gfxManager->GetSystem()->CastSubsystem<ResourceManager>(SubsystemType::ResourceManager);
	}

	bool Lina::GUIBackend::Initialize()
	{
		return true;
	}

	void GUIBackend::Terminate()
	{
		for (auto [sid, txt] : m_fontTextures)
			delete txt;

		m_fontTextures.clear();
	}

	void GUIBackend::StartFrame(int threadCount)
	{
		m_frameGUIRenderers.resize(threadCount);
	}

	void GUIBackend::DrawGradient(LinaVG::GradientDrawBuffer* buf, int thread)
	{
		m_frameGUIRenderers[thread]->FeedGradient(buf);
	}

	void GUIBackend::DrawTextured(LinaVG::TextureDrawBuffer* buf, int thread)
	{
		m_frameGUIRenderers[thread]->FeedTextured(buf);
	}

	void GUIBackend::DrawDefault(LinaVG::DrawBuffer* buf, int thread)
	{
		m_frameGUIRenderers[thread]->FeedDefault(buf);
	}

	void GUIBackend::DrawSimpleText(LinaVG::SimpleTextDrawBuffer* buf, int thread)
	{
		m_frameGUIRenderers[thread]->FeedSimpleText(buf);
	}

	void GUIBackend::DrawSDFText(LinaVG::SDFTextDrawBuffer* buf, int thread)
	{
		m_frameGUIRenderers[thread]->FeedSDFText(buf);
	}

	void GUIBackend::EndFrame()
	{
		m_frameGUIRenderers.clear();
	}

	void GUIBackend::BufferFontTextureAtlas(int width, int height, int offsetX, int offsetY, unsigned char* data)
	{
		auto*		   txt		   = m_fontTextures[m_boundFontTexture];
		auto*		   pixels	   = txt->GetPixels();
		const uint32   bufSize	   = width * height;
		const Extent3D ext		   = txt->GetExtent();
		uint32		   startOffset = offsetY * ext.width + offsetX;
		m_textureDirtyStatus[txt]  = true;

		for (int i = 0; i < height; i++)
		{
			const uint32 size = width;
			MEMCPY(pixels + startOffset, &data[width * i], size);
			startOffset += ext.width;
		}
	}

	void GUIBackend::BindFontTexture(LinaVG::BackendHandle texture)
	{
		m_boundFontTexture = texture;
	}

	LinaVG::BackendHandle GUIBackend::CreateFontTexture(int width, int height)
	{
		const String   path = "GUIBackendFont_" + TO_STRING(m_fontTextureCounter++);
		const StringID sid	= TO_SID(path);

		Extent3D extent = Extent3D{
			.width	= static_cast<uint32>(width),
			.height = static_cast<uint32>(height),
		};

		UserGeneratedTextureData textureData = UserGeneratedTextureData{
			.resourceType				   = TextureResourceType::Texture2DDefaultDynamic,
			.path						   = path,
			.sid						   = sid,
			.extent						   = extent,
			.format						   = Format::R8_UNORM,
			.targetSampler				   = DEFAULT_GUI_TEXT_SAMPLER_SID,
			.tiling						   = ImageTiling::Linear,
			.channels					   = 1,
			.createPixelBuffer			   = true,
			.destroyPixelBufferAfterUpload = false,
		};

		Texture* txt			  = new Texture(m_resourceManager, textureData);
		m_textureDirtyStatus[txt] = false;
		m_fontTextures[sid]		  = txt;
		m_boundFontTexture		  = sid;
		return sid;
	}

	void GUIBackend::OnResourceBatchLoaded(const Event& ev)
	{
		ResourceLoadTask* task = static_cast<ResourceLoadTask*>(ev.pParams[0]);

		for (const auto& ident : task->identifiers)
		{
			if (ident.tid == GetTypeID<Font>())
			{
				CheckFontTexturesForUpload();
				break;
			}
		}
	}

	void GUIBackend::SetFrameGUIRenderer(int threadID, GUIRenderer* rend)
	{
		m_frameGUIRenderers[threadID] = rend;
	}

	void GUIBackend::BindTextures()
	{
		const uint32	 texturesSize = static_cast<uint32>(m_renderReadyFontTextures.size());
		Vector<Texture*> textures;
		textures.reserve(texturesSize);

		for (auto& [sid, txt] : m_renderReadyFontTextures)
			textures.push_back(txt);

		m_renderer->BindDynamicTextures(textures.data(), texturesSize);
	}

	void GUIBackend::CheckFontTexturesForUpload()
	{
		m_renderer->Join();
		
		for (auto& [txt, isDirty] : m_textureDirtyStatus)
		{
			if (isDirty)
				txt->Upload();
		}

		m_renderReadyFontTextures = m_fontTextures;
	}

} // namespace Lina
