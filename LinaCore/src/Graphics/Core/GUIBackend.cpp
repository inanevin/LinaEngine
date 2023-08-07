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
#include "Graphics/Core/GfxManager.hpp"
#include "System/ISystem.hpp"
#include "Resources/Core/CommonResources.hpp"

namespace Lina
{
	GUIBackend::GUIBackend(GfxManager* man)
	{
		m_gfxManager	  = man;
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
	}

	void GUIBackend::BindFontTexture(LinaVG::BackendHandle texture)
	{
		m_boundFontTexture = texture;
	}

	LinaVG::BackendHandle GUIBackend::CreateFontTexture(int width, int height)
	{
		return 0;
	}

	void GUIBackend::SetFrameGUIRenderer(int threadID, GUIRenderer* rend)
	{
		m_frameGUIRenderers[threadID] = rend;
	}

	void GUIBackend::BindTextures()
	{
	}

} // namespace Lina
