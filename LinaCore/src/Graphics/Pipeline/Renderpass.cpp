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

#include "Core/Graphics/Pipeline/Renderpass.hpp"
#include "Core/Graphics/Utility/GfxHelpers.hpp"
#include "Core/Graphics/GfxManager.hpp"

namespace Lina
{
	void Renderpass::Create(GfxManager* gfxMan, LinaGX::Instance* lgx, RenderPassDescriptorType descriptorType)
	{
		m_lgx		 = lgx;
		m_gfxManager = gfxMan;
		m_type		 = descriptorType;

		GPUDataView dummyViewData = {};

		for (int32 i = 0; i < FRAMES_IN_FLIGHT; i++)
		{
			auto& data		   = m_pfd[i];
			data.descriptorSet = lgx->CreateDescriptorSet(GfxHelpers::GetSetDescPersistentRenderPass(m_type));

			data.viewDataBuffer.Create(lgx, LinaGX::ResourceTypeHint::TH_ConstantBuffer, sizeof(GPUDataView), "RP Scene Data", true);
			data.viewDataBuffer.BufferData(0, &dummyViewData, sizeof(GPUDataView));
			m_lgx->DescriptorUpdateBuffer({
				.setHandle = data.descriptorSet,
				.binding   = 0,
				.buffers   = {data.viewDataBuffer.GetGPUResource()},
			});
		}
	}

	void Renderpass::Destroy()
	{
		for (int32 i = 0; i < FRAMES_IN_FLIGHT; i++)
		{
			auto& data = m_pfd[i];
			data.viewDataBuffer.Destroy();
			m_lgx->DestroyDescriptorSet(data.descriptorSet);
		}
	}

	void Renderpass::SetColorAttachment(uint32 index, const LinaGX::RenderPassColorAttachment& att)
	{
		LINA_ASSERT(index < static_cast<uint32>(m_colorAttachments.size() + 1), "");

		if (index < static_cast<uint32>(m_colorAttachments.size()))
			m_colorAttachments[index] = att;
		else
			m_colorAttachments.push_back(att);
	}

	void Renderpass::BindDescriptors(LinaGX::CommandStream* stream, uint32 frameIndex, bool bindGlobalSet)
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

	void Renderpass::Begin(LinaGX::CommandStream* stream, const LinaGX::Viewport& vp, const LinaGX::ScissorsRect& scissors)
	{
		LinaGX::CMDBeginRenderPass* rp = stream->AddCommand<LinaGX::CMDBeginRenderPass>();
		rp->colorAttachmentCount	   = static_cast<uint32>(m_colorAttachments.size());
		rp->colorAttachments		   = stream->EmplaceAuxMemory<LinaGX::RenderPassColorAttachment>(m_colorAttachments.data(), m_colorAttachments.size() * sizeof(LinaGX::RenderPassColorAttachment));
		rp->depthStencilAttachment	   = m_depthStencil;
		rp->viewport				   = vp;
		rp->scissors				   = scissors;
	}

	void Renderpass::End(LinaGX::CommandStream* stream)
	{
		LinaGX::CMDEndRenderPass* end = stream->AddCommand<LinaGX::CMDEndRenderPass>();
	}

} // namespace Lina
