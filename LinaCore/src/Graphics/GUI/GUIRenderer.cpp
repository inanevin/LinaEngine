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

#include "Core/Graphics/GUI/GUIRenderer.hpp"
#include "Core/Graphics/GUI/GUIBackend.hpp"
#include "Core/Graphics/GfxManager.hpp"
#include "Core/Graphics/Resource/Shader.hpp"
#include "Core/Resources/ResourceManager.hpp"
#include "Common/Platform/LinaGXIncl.hpp"
#include "Common/Platform/LinaVGIncl.hpp"
#include "Common/System/System.hpp"
#include "Core/Graphics/Resource/Font.hpp"

namespace Lina
{

#define MAX_COPY_COMMANDS 50
#define MAX_GUI_VERTICES  10000
#define MAX_GUI_INDICES	  8000

	void GUIRenderer::Create(GfxManager* gfxManager, ShaderWriteTargetType writeTargetType)
	{
		m_gfxManager		  = gfxManager;
		m_lgx				  = m_gfxManager->GetLGX();
		m_guiBackend		  = m_gfxManager->GetGUIBackend();
		auto* rm			  = m_gfxManager->GetSystem()->CastSubsystem<ResourceManager>(SubsystemType::ResourceManager);
		m_shader			  = rm->GetResource<Shader>(DEFAULT_SHADER_GUI);
		m_shaderVariantHandle = m_shader->GetGPUHandle(writeTargetType);

		for (uint32 i = 0; i < FRAMES_IN_FLIGHT; i++)
		{
			auto& data					 = m_pfd[i];
			data.copyStream				 = m_lgx->CreateCommandStream({LinaGX::CommandType::Transfer, MAX_COPY_COMMANDS, 4000, 1024, 32, "GUIRenderer: Copy Stream"});
			data.copySemaphore.semaphore = m_lgx->CreateUserSemaphore();
			data.guiVertexBuffer.Create(m_lgx, LinaGX::ResourceTypeHint::TH_VertexBuffer, MAX_GUI_VERTICES * sizeof(LinaVG::Vertex), "GUIRenderer VertexBuffer");
			data.guiIndexBuffer.Create(m_lgx, LinaGX::ResourceTypeHint::TH_IndexBuffer, MAX_GUI_INDICES * sizeof(LinaVG::Index), "GUIRenderer IndexBuffer");
			data.materials.resize(MAX_GUI_MATERIALS);

			for (int32 j = 0; j < MAX_GUI_MATERIALS; j++)
			{
				data.materials[j] = rm->CreateUserResource<Material>("GUIRendererMaterial", 0);
				data.materials[j]->SetShader(DEFAULT_SHADER_GUI);
			}
		}
	}

	void GUIRenderer::Destroy()
	{
		auto* rm = m_gfxManager->GetSystem()->CastSubsystem<ResourceManager>(SubsystemType::ResourceManager);

		for (uint32 i = 0; i < FRAMES_IN_FLIGHT; i++)
		{
			auto& data = m_pfd[i];
			m_lgx->DestroyCommandStream(data.copyStream);
			m_lgx->DestroyUserSemaphore(data.copySemaphore.semaphore);
			data.guiVertexBuffer.Destroy();
			data.guiIndexBuffer.Destroy();

			for (int32 j = 0; j < MAX_GUI_MATERIALS; j++)
				rm->DestroyUserResource<Material>(data.materials[j]);

			data.materials.clear();
		}
	}

	void GUIRenderer::Prepare(uint32 frameIndex, uint32 threadIndex)
	{
		m_guiBackend->Prepare(threadIndex, &m_pfd[frameIndex].guiIndexBuffer, &m_pfd[frameIndex].guiVertexBuffer);
	}

	void GUIRenderer::Render(LinaGX::CommandStream* stream, uint32 frameIndex, uint32 threadIndex, const Vector2ui& size)
	{
		// Will flush buffers and fill draw requests.
		LinaVG::Render(threadIndex);

		const auto& drawRequests = m_guiBackend->GetDrawData(threadIndex).drawRequests;
		auto&		pfd			 = m_pfd[frameIndex];

		// Shader.
		m_shader->Bind(stream, m_shaderVariantHandle);

		// Buffer setup.
		bool copyExists = false;

		if (pfd.guiIndexBuffer.Copy(pfd.copyStream))
			copyExists = true;

		if (pfd.guiVertexBuffer.Copy(pfd.copyStream))
			copyExists = true;

		if (copyExists)
		{
			uint64 val = pfd.copySemaphore.value;
			pfd.copySemaphore.value++;

			m_lgx->CloseCommandStreams(&pfd.copyStream, 1);

			m_lgx->SubmitCommandStreams({
				.targetQueue	  = m_lgx->GetPrimaryQueue(LinaGX::CommandType::Transfer),
				.streams		  = &pfd.copyStream,
				.streamCount	  = 1,
				.useSignal		  = true,
				.signalCount	  = 1,
				.signalSemaphores = &(pfd.copySemaphore.semaphore),
				.signalValues	  = &val,
				.isMultithreaded  = true,
			});
		}

		// Buffer bind.
		pfd.guiVertexBuffer.BindVertex(stream, static_cast<uint32>(sizeof(LinaVG::Vertex)));
		pfd.guiIndexBuffer.BindIndex(stream, LinaGX::IndexType::Uint16);
		LINA_ASSERT(drawRequests.size() < MAX_GUI_MATERIALS, "Requests exceed materials size!");

		Rectui scissorsRect = Rectui(0, 0, size.x, size.y);
		uint32 reqIndex		= 0;
		for (const auto& req : drawRequests)
		{
			Rectui currentRect = Rectui(0, 0, size.x, size.y);

			if (req.clip.size.x != 0 || req.clip.size.y != 0)
			{
				currentRect.pos	 = req.clip.pos;
				currentRect.size = req.clip.size;
			}

			if (!currentRect.Equals(scissorsRect))
			{
				scissorsRect					 = currentRect;
				LinaGX::CMDSetScissors* scissors = stream->AddCommand<LinaGX::CMDSetScissors>();
				scissors->x						 = scissorsRect.pos.x;
				scissors->y						 = scissorsRect.pos.y;
				scissors->width					 = scissorsRect.size.x;
				scissors->height				 = scissorsRect.size.y;
			}

			Material* mat = pfd.materials[reqIndex];

			// Set material data.
			mat->SetBuffer(0, 0, frameIndex, 0, (uint8*)&req.materialData, sizeof(GPUMaterialGUI));
			if (req.hasTextureBind)
			{
				mat->SetSampler(1, 0, req.samplerHandle);
				mat->SetTexture(2, 0, req.textureHandle);
			}

			// Bind.
			mat->Bind(stream, frameIndex, LinaGX::DescriptorSetsLayoutSource::CustomLayout);

			// Draw command.
			LinaGX::CMDDrawIndexedInstanced* draw = stream->AddCommand<LinaGX::CMDDrawIndexedInstanced>();
			draw->indexCountPerInstance			  = req.indexCount;
			draw->instanceCount					  = 1;
			draw->startInstanceLocation			  = 0;
			draw->startIndexLocation			  = req.firstIndex;
			draw->baseVertexLocation			  = req.vertexOffset;

			reqIndex++;
		}
	}

} // namespace Lina
