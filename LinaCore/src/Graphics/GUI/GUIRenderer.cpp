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
#include "Common/Platform/LinaGXIncl.hpp"

namespace Lina
{

#define MAX_COPY_COMMANDS 50

#define MAX_GUI_VERTICES  5000
#define MAX_GUI_INDICES	  5000
#define MAX_GUI_MATERIALS 100

	void GUIRenderer::Create(GfxManager* gfxManager)
	{
		m_gfxManager = gfxManager;
		m_lgx		 = m_gfxManager->GetLGX();
		m_guiBackend = m_gfxManager->GetGUIBackend();

		for (uint32 i = 0; i < FRAMES_IN_FLIGHT; i++)
		{
			auto& data					 = m_pfd[i];
			data.copyStream				 = m_lgx->CreateCommandStream({LinaGX::CommandType::Transfer, MAX_COPY_COMMANDS, 4000, 1024, 32, "GUIRenderer: Copy Stream"});
			data.copySemaphore.semaphore = m_lgx->CreateUserSemaphore();
			data.guiVertexBuffer.Create(m_lgx, LinaGX::ResourceTypeHint::TH_VertexBuffer, MAX_GUI_VERTICES * sizeof(LinaVG::Vertex), "GUIRenderer Vtx");
			data.guiIndexBuffer.Create(m_lgx, LinaGX::ResourceTypeHint::TH_IndexBuffer, MAX_GUI_INDICES * sizeof(LinaVG::Index), "GUIRenderer Indx");
			data.guiMaterialBuffer.Create(m_lgx, LinaGX::ResourceTypeHint::TH_ConstantBuffer, MAX_GUI_MATERIALS * sizeof(GPUMaterialGUI), "GUIRenderer Materials", true);
		}
	}

	void GUIRenderer::Destroy()
	{
		for (uint32 i = 0; i < FRAMES_IN_FLIGHT; i++)
		{
			auto& data = m_pfd[i];
			m_lgx->DestroyCommandStream(data.copyStream);
			m_lgx->DestroyUserSemaphore(data.copySemaphore.semaphore);
			data.guiVertexBuffer.Destroy();
			data.guiIndexBuffer.Destroy();
			data.guiMaterialBuffer.Destroy();
		}
	}

	void GUIRenderer::Render(LinaGX::CommandStream* stream, uint32 frameIndex, uint32 threadIndex)
	{
		const auto& drawRequests = m_guiBackend->GetDrawData(threadIndex).drawRequests;
	}
	/*

	void GUIBackend::Render(int threadIndex)
	{
		// Lazy-get shader
		if (m_shader == nullptr)
			m_shader = m_resourceManager->GetResource<Shader>("Resources/Core/Shaders/GUIStandard.linashader"_hs);

		if (m_lgx == nullptr)
			m_lgx = m_gfxManager->GetLGX();

		auto& renderData = m_renderData[threadIndex];
		auto& buffers	 = m_buffers[threadIndex];
		auto& drawData	 = m_drawData[threadIndex];

		// Update material data.
		Vector<GPUMaterialGUI> materials;
		materials.resize(drawData.drawRequests.size());
		uint32 i = 0;
		for (const auto& req : drawData.drawRequests)
		{
			materials[i] = req.materialData;
			i++;
		}

		buffers.materialBuffer->BufferData(0, (uint8*)materials.data(), materials.size() * sizeof(GPUMaterialGUI));

		// Check need for copy operation
		if (buffers.materialBuffer->Copy(renderData.copyStream) || buffers.indexBuffer->Copy(renderData.copyStream) || buffers.vertexBuffer->Copy(renderData.copyStream))
		{
			uint64 val = renderData.copySemaphore->value;
			renderData.copySemaphore->value++;

			LinaGX::SubmitDesc desc = {
				.targetQueue	  = m_lgx->GetPrimaryQueue(LinaGX::CommandType::Transfer),
				.streams		  = &renderData.copyStream,
				.streamCount	  = 1,
				.useSignal		  = true,
				.signalCount	  = 1,
				.signalSemaphores = &(renderData.copySemaphore->semaphore),
				.signalValues	  = &val,
				.isMultithreaded  = true,
			};

			m_lgx->CloseCommandStreams(&renderData.copyStream, 1);
			m_lgx->SubmitCommandStreams(desc);
		}

		// Pipeline
		LinaGX::CMDBindPipeline* pipeline = renderData.gfxStream->AddCommand<LinaGX::CMDBindPipeline>();
		pipeline->shader				  = m_shader->GetGPUHandle(renderData.variantPassType);

		// Buffers
		LinaGX::CMDBindVertexBuffers* vtx = renderData.gfxStream->AddCommand<LinaGX::CMDBindVertexBuffers>();
		vtx->offset						  = 0;
		vtx->slot						  = 0;
		vtx->vertexSize					  = sizeof(LinaVG::Vertex);
		vtx->resource					  = buffers.vertexBuffer->GetGPUResource();

		LinaGX::CMDBindIndexBuffers* index = renderData.gfxStream->AddCommand<LinaGX::CMDBindIndexBuffers>();
		index->indexType				   = LinaGX::IndexType::Uint16;
		index->offset					   = 0;
		index->resource					   = buffers.indexBuffer->GetGPUResource();

		// Descriptors
		LinaGX::CMDBindDescriptorSets* bind = renderData.gfxStream->AddCommand<LinaGX::CMDBindDescriptorSets>();
		bind->descriptorSetHandles			= renderData.gfxStream->EmplaceAuxMemory<uint16>(renderData.descriptorSet);
		bind->firstSet						= 2;
		bind->setCount						= 1;
		bind->layoutSource = LinaGX::DescriptorLayoutSource::CustomLayout;
		bind->customLayout = m_shader->GetPipelineLayout();

		// Draw
		Rectui scissorsRect = Rectui(0, 0, renderData.size.x, renderData.size.y);
		for (const auto& req : drawData.drawRequests)
		{
			Rectui currentRect = Rectui(0, 0, renderData.size.x, renderData.size.y);

			if (req.clip.size.x != 0 || req.clip.size.y != 0)
			{
				currentRect.pos	 = req.clip.pos;
				currentRect.size = req.clip.size;
			}

			if (!currentRect.Equals(scissorsRect))
			{
				scissorsRect					 = currentRect;
				LinaGX::CMDSetScissors* scissors = renderData.gfxStream->AddCommand<LinaGX::CMDSetScissors>();
				scissors->x						 = scissorsRect.pos.x;
				scissors->y						 = scissorsRect.pos.y;
				scissors->width					 = scissorsRect.size.x;
				scissors->height				 = scissorsRect.size.y;
			}

			// Draw command.
			LinaGX::CMDDrawIndexedInstanced* draw = renderData.gfxStream->AddCommand<LinaGX::CMDDrawIndexedInstanced>();
			draw->indexCountPerInstance			  = req.indexCount;
			draw->instanceCount					  = 1;
			draw->startInstanceLocation			  = 0;
			draw->startIndexLocation			  = req.firstIndex;
			draw->baseVertexLocation			  = req.vertexOffset;
		}
	}

	 */

} // namespace Lina
