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

#include "Core/Graphics/Pipeline/RenderPass.hpp"
#include "Core/Graphics/Utility/GfxHelpers.hpp"
#include "Core/Graphics/ResourceUploadQueue.hpp"
#include "Core/Application.hpp"

namespace Lina
{
	void RenderPass::Create(const RenderPassDescription& desc, LinaGX::Window* window)
	{
		m_window = window;

		for (int32 i = 0; i < FRAMES_IN_FLIGHT; i++)
		{
			auto& data		   = m_pfd[i];
			data.descriptorSet = Application::GetLGX()->CreateDescriptorSet(desc.setDescription);

			for (const auto& b : desc.buffers)
			{
				if (b.createdOutside)
					continue;

				m_bufferIndices[b.ident] = static_cast<uint32>(data.buffers.size());
				data.buffers.push_back({});
				auto& buffer = data.buffers.back();
				buffer.Create(b.bufferType, static_cast<uint32>(b.size), b.debugName, b.stagingOnly);
				buffer.MemsetMapped(0);

				if (b.bindingIndex != -1)
				{
					Application::GetLGX()->DescriptorUpdateBuffer({
						.setHandle = data.descriptorSet,
						.binding   = static_cast<uint32>(b.bindingIndex),
						.buffers   = {buffer.GetGPUResource()},
					});
				}
			}
		}
	}

	void RenderPass::Destroy()
	{
		for (int32 i = 0; i < FRAMES_IN_FLIGHT; i++)
		{
			auto& data = m_pfd[i];

			for (auto& b : data.buffers)
				b.Destroy();

			data.buffers.clear();
			Application::GetLGX()->DestroyDescriptorSet(data.descriptorSet);
		}
	}

	void RenderPass::SetColorAttachment(uint32 frameIndex, uint32 index, const LinaGX::RenderPassColorAttachment& att)
	{
		auto& colorAttachments = m_pfd[frameIndex].colorAttachments;

		LINA_ASSERT(index < static_cast<uint32>(colorAttachments.size() + 1), "");

		if (index < static_cast<uint32>(colorAttachments.size()))
			colorAttachments[index] = att;
		else
			colorAttachments.push_back(att);
	}

	void RenderPass::BindDescriptors(LinaGX::CommandStream* stream, uint32 frameIndex, uint16 pipelineLayout, uint32 firstSet)
	{
		LinaGX::CMDBindDescriptorSets* bind = stream->AddCommand<LinaGX::CMDBindDescriptorSets>();
		bind->descriptorSetHandles			= stream->EmplaceAuxMemory<uint16>(m_pfd[frameIndex].descriptorSet);
		bind->firstSet						= firstSet;
		bind->setCount						= 1;
		bind->layoutSource					= LinaGX::DescriptorSetsLayoutSource::CustomLayout;
		bind->customLayout					= pipelineLayout;
	}

	void RenderPass::Begin(LinaGX::CommandStream* stream, const LinaGX::Viewport& vp, const LinaGX::ScissorsRect& scissors, uint32 frameIndex)
	{
		auto& colorAttachments = m_pfd[frameIndex].colorAttachments;

		LinaGX::CMDBeginRenderPass* rp = stream->AddCommand<LinaGX::CMDBeginRenderPass>();
		rp->colorAttachmentCount	   = static_cast<uint32>(colorAttachments.size());
		rp->colorAttachments		   = stream->EmplaceAuxMemory<LinaGX::RenderPassColorAttachment>(colorAttachments.data(), colorAttachments.size() * sizeof(LinaGX::RenderPassColorAttachment));
		rp->depthStencilAttachment	   = m_pfd[frameIndex].depthStencil;
		rp->viewport				   = vp;
		rp->scissors				   = scissors;
	}

	void RenderPass::End(LinaGX::CommandStream* stream)
	{
		LinaGX::CMDEndRenderPass* end = stream->AddCommand<LinaGX::CMDEndRenderPass>();
	}

	void RenderPass::Prepare(uint32 frameIndex, ResourceUploadQueue& queue)
	{
		auto& pfd = m_pfd[frameIndex];
		for (auto& buffer : pfd.buffers)
			queue.AddBufferRequest(&buffer);

		m_renderingData.drawCalls.resize(0);

		// go through calls in gpu data
		// for every call, for every instance, push a draw argument.
	}

	void RenderPass::Render(uint32 frameIndex, LinaGX::CommandStream* stream)
	{
		uint16	bound		   = 0;
		bool	isBound		   = false;
		Buffer* boundVtxBuffer = nullptr;

		auto checkPipeline = [&](uint32 handle) {
			if (!isBound || bound != handle)
			{
				LinaGX::CMDBindPipeline* pipeline = stream->AddCommand<LinaGX::CMDBindPipeline>();
				pipeline->shader				  = handle;
				isBound							  = true;
				bound							  = handle;
			}
		};

		auto checkBuffers = [&](Buffer* vtxBuffer, Buffer* idxBuffer, size_t vtxSize) {
			if (boundVtxBuffer == nullptr || boundVtxBuffer != vtxBuffer)
			{
				vtxBuffer->BindVertex(stream, static_cast<uint32>(vtxSize));
				idxBuffer->BindIndex(stream, LinaGX::IndexType::Uint16);
				boundVtxBuffer = vtxBuffer;
			}
		};

		bool scissorsWasSet = false;

		for (const InstancedDraw& draw : m_renderingData.drawCalls)
		{
			checkPipeline(draw.shaderHandle);

			bool	indexedInstanced = false;
			Buffer* indexBuffer		 = draw.indexBuffers[frameIndex];
			Buffer* vertexBuffer	 = draw.vertexBuffers[frameIndex];
			if (indexBuffer != nullptr && vertexBuffer != nullptr)
			{
				checkBuffers(vertexBuffer, indexBuffer, draw.vertexSize);
				indexedInstanced = true;
			}

			if (draw.useScissors)
			{
				LinaGX::CMDSetScissors* sc = stream->AddCommand<LinaGX::CMDSetScissors>();
				sc->x					   = draw.clip.pos.x < 0 ? 0 : draw.clip.pos.x;
				sc->y					   = draw.clip.pos.y < 0 ? 0 : draw.clip.pos.y;
				sc->width				   = draw.clip.size.x <= 0 ? static_cast<uint32>(m_window->GetSize().x) : draw.clip.size.x;
				sc->height				   = draw.clip.size.y <= 0 ? static_cast<uint32>(m_window->GetSize().y) : draw.clip.size.y;
				scissorsWasSet			   = true;
			}
			else if (scissorsWasSet)
			{
				LinaGX::CMDSetScissors* sc = stream->AddCommand<LinaGX::CMDSetScissors>();
				sc->x					   = 0;
				sc->y					   = 0;
				sc->width				   = m_window->GetSize().x;
				sc->height				   = m_window->GetSize().y;
			}

			LinaGX::CMDBindConstants* pushConstants = stream->AddCommand<LinaGX::CMDBindConstants>();
			pushConstants->data						= stream->EmplaceAuxMemory<uint32>(draw.pushConstant);
			pushConstants->size						= sizeof(uint32);
			pushConstants->stagesSize				= 1;
			pushConstants->stages					= stream->EmplaceAuxMemory<LinaGX::ShaderStage>(LinaGX::ShaderStage::Vertex);

			if (indexedInstanced)
			{
				LinaGX::CMDDrawIndexedInstanced* d = stream->AddCommand<LinaGX::CMDDrawIndexedInstanced>();
				d->baseVertexLocation			   = draw.baseVertex;
				d->indexCountPerInstance		   = draw.indexCount;
				d->instanceCount				   = draw.instanceCount;
				d->startIndexLocation			   = draw.baseIndex;
				d->startInstanceLocation		   = draw.baseInstance;
			}
			else
			{

				LinaGX::CMDDrawInstanced* d = stream->AddCommand<LinaGX::CMDDrawInstanced>();
				d->instanceCount			= draw.instanceCount;
				d->startInstanceLocation	= draw.baseInstance;
				d->startVertexLocation		= draw.baseVertex;
				d->vertexCountPerInstance	= draw.vertexCount;
			}
		}
	}

	bool RenderPass::CopyBuffers(uint32 frameIndex, LinaGX::CommandStream* copyStream)
	{
		auto& pfd = m_pfd[frameIndex];

		bool copyExists = false;

		for (auto& buffer : pfd.buffers)
		{
			if (buffer.Copy(copyStream))
				copyExists = true;
		}

		return copyExists;
	}

	void RenderPass::SyncRender()
	{
		m_gpuDrawData = m_cpuDrawData;
		m_cpuDrawData = {};
	}

} // namespace Lina
