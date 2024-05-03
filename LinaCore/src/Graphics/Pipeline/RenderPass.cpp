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
#include "Core/Graphics/GfxManager.hpp"

namespace Lina
{

	void RenderPass::Create(GfxManager* gfxMan, RenderPassDescriptorType descriptorType)
	{
		m_lgx		 = gfxMan->GetLGX();
		m_gfxManager = gfxMan;
		m_type		 = descriptorType;

		for (int32 i = 0; i < FRAMES_IN_FLIGHT; i++)
		{
			auto& data		   = m_pfd[i];
			data.descriptorSet = m_lgx->CreateDescriptorSet(GfxHelpers::GetSetDescPersistentRenderPass(m_type));

			if (descriptorType == RenderPassDescriptorType::Gui)
				AddRPGUI(data);
			else if (descriptorType == RenderPassDescriptorType::Main)
				AddRPMain(data);
		}
	}

	void RenderPass::AddRPGUI(PerFrameData& data)
	{
		// View
		{
			data.buffers.push_back({});
			auto&		 buffer	 = data.buffers.back();
			const String dbgName = "RP: " + String(RPTypeToString(m_type)) + "ViewData";
			buffer.Create(m_lgx, LinaGX::ResourceTypeHint::TH_ConstantBuffer, sizeof(GPUDataView), dbgName, true);
			GPUDataView dummyViewData = {};

			buffer.BufferData(0, (uint8*)&dummyViewData, sizeof(GPUDataView));
			m_lgx->DescriptorUpdateBuffer({
				.setHandle = data.descriptorSet,
				.binding   = 0,
				.buffers   = {buffer.GetGPUResource()},
			});
		}

		// Materials
		{
			// Should be set outside
		}
	}

	void RenderPass::AddRPMain(PerFrameData& data)
	{
		// View
		{
			data.buffers.push_back({});
			auto&		 buffer	 = data.buffers.back();
			const String dbgName = "RP (" + String(RPTypeToString(m_type)) + "): ViewData";
			buffer.Create(m_lgx, LinaGX::ResourceTypeHint::TH_ConstantBuffer, sizeof(GPUDataView), dbgName, true);
			GPUDataView dummyViewData = {};

			buffer.BufferData(0, (uint8*)&dummyViewData, sizeof(GPUDataView));
			m_lgx->DescriptorUpdateBuffer({
				.setHandle = data.descriptorSet,
				.binding   = 0,
				.buffers   = {buffer.GetGPUResource()},
			});
		}

		// Scene data will be set by the user.
		{
		}

		// GPU objects buffer will be set by the user.
		{
		}

		// Indirect buffer
		{
			data.buffers.push_back({});
			auto&		 buffer	 = data.buffers.back();
			const String dbgName = "RP (" + String(RPTypeToString(m_type)) + "): IndirectBuffer";

			buffer.Create(m_lgx, LinaGX::ResourceTypeHint::TH_IndirectBuffer, m_lgx->GetIndexedIndirectCommandSize() * static_cast<size_t>(250), dbgName, false);
			buffer.MemsetMapped(0);
			// m_lgx->DescriptorUpdateBuffer({
			//     .setHandle = data.descriptorSet,
			//     .binding = 3,
			//     .buffers = {buffer.GetGPUResource()},
			// });
		}

		// Indirect constants
		{
			data.buffers.push_back({});
			auto&		 buffer	 = data.buffers.back();
			const String dbgName = "RP (" + String(RPTypeToString(m_type)) + "): IndirectConstants";

			buffer.Create(m_lgx, LinaGX::ResourceTypeHint::TH_StorageBuffer, sizeof(GPUIndirectConstants0) * 2500, dbgName, false);
			buffer.MemsetMapped(0);
			m_lgx->DescriptorUpdateBuffer({
				.setHandle = data.descriptorSet,
				.binding   = 3,
				.buffers   = {buffer.GetGPUResource()},
			});
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
			m_lgx->DestroyDescriptorSet(data.descriptorSet);
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

	void RenderPass::BindDescriptors(LinaGX::CommandStream* stream, uint32 frameIndex, bool bindGlobalSet)
	{
		LinaGX::CMDBindDescriptorSets* bind = stream->AddCommand<LinaGX::CMDBindDescriptorSets>();

		if (bindGlobalSet)
		{
			bind->descriptorSetHandles = stream->EmplaceAuxMemory<uint16>(m_gfxManager->GetDescriptorSetPersistentGlobal(frameIndex), m_pfd[frameIndex].descriptorSet);
			bind->firstSet			   = 0;
			bind->setCount			   = 2;
		}
		else
		{
			bind->descriptorSetHandles = stream->EmplaceAuxMemory<uint16>(m_pfd[frameIndex].descriptorSet);
			bind->firstSet			   = 1;
			bind->setCount			   = 1;
		}

		bind->layoutSource = LinaGX::DescriptorSetsLayoutSource::CustomLayout;
		bind->customLayout = m_gfxManager->GetPipelineLayoutPersistentRenderPass(frameIndex, m_type);
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

} // namespace Lina
