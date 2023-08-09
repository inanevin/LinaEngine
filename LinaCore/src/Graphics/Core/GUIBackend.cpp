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
#include "Graphics/Core/GPUBuffer.hpp"
#include "Graphics/Core/GfxManager.hpp"
#include "System/ISystem.hpp"
#include "Resources/Core/CommonResources.hpp"
#include "Platform/LinaGXIncl.hpp"

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
		m_guiRenderData.resize(threadCount);
	}

	void GUIBackend::DrawGradient(LinaVG::GradientDrawBuffer* buf, int thread)
	{
	}

	void GUIBackend::DrawTextured(LinaVG::TextureDrawBuffer* buf, int thread)
	{
	}

	void GUIBackend::DrawDefault(LinaVG::DrawBuffer* buf, int thread)
	{
		AddDrawRequest(buf, m_guiRenderData[thread]);
	}

	void GUIBackend::DrawSimpleText(LinaVG::SimpleTextDrawBuffer* buf, int thread)
	{
	}

	void GUIBackend::DrawSDFText(LinaVG::SDFTextDrawBuffer* buf, int thread)
	{
	}

	void GUIBackend::Render(int threadID)
	{
		auto& data = m_guiRenderData[threadID];

		// Draw
		{
			uint32 i = 0;
			for (const auto& req : data.drawRequests)
			{
				// Material id.
				LinaGX::CMDBindConstants* constants = data.gfxStream->AddCommand<LinaGX::CMDBindConstants>();
				constants->extension				= nullptr;
				constants->offset					= 0;
				constants->stages					= data.gfxStream->EmplaceAuxMemory<LinaGX::ShaderStage>(LinaGX::ShaderStage::Fragment);
				constants->stagesSize				= 1;
				constants->size						= sizeof(uint32);
				constants->data						= data.gfxStream->EmplaceAuxMemory<uint32>(i);

				// Draw command.
				LinaGX::CMDDrawIndexedInstanced* draw = data.gfxStream->AddCommand<LinaGX::CMDDrawIndexedInstanced>();
				draw->extension						  = nullptr;
				draw->indexCountPerInstance			  = req.indexCount;
				draw->instanceCount					  = 1;
				draw->startInstanceLocation			  = 0;
				draw->startIndexLocation			  = req.firstIndex;
				draw->baseVertexLocation			  = req.vertexOffset;

				i++;
			}
		}
	}

	void GUIBackend::EndFrame()
	{
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

	void GUIBackend::Prepare(int threadID, const GUIRenderData& data)
	{
		m_guiRenderData[threadID] = data;
	}

	void GUIBackend::BindTextures()
	{
	}

	GUIBackend::DrawRequest& GUIBackend::AddDrawRequest(LinaVG::DrawBuffer* buf, GUIRenderData& data)
	{
		data.drawRequests.push_back(DrawRequest());
		DrawRequest& req = data.drawRequests.back();
		req.indexCount	 = static_cast<uint32>(buf->m_indexBuffer.m_size);
		req.vertexOffset = data.vertexCounter;
		req.firstIndex	 = data.indexCounter;

		data.indexBuffer->BufferData(data.vertexCounter * sizeof(LinaVG::Index));
		data.indexCounter += req.indexCount;
		data.vertexCounter += static_cast<uint32>(buf->m_vertexBuffer.m_size);

		return req;
	}

} // namespace Lina
