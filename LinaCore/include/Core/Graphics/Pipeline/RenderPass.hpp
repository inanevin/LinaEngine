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

#pragma once

#include "Core/Graphics/Data/RenderData.hpp"
#include "Core/Graphics/CommonGraphics.hpp"
#include "Common/Platform/LinaGXIncl.hpp"
#include "Common/Data/Vector.hpp"
#include "Buffer.hpp"

namespace Lina
{
	class ResourceUploadQueue;

	struct RenderPassBuffer
	{
		LinaGX::ResourceTypeHint bufferType	  = LinaGX::ResourceTypeHint::TH_ConstantBuffer;
		String					 debugName	  = "";
		size_t					 size		  = 0;
		bool					 stagingOnly  = false;
		int32					 bindingIndex = -1;
		StringID				 ident		  = 0;
	};

	struct RenderPassDescription
	{
		Vector<RenderPassBuffer>  buffers;
		LinaGX::DescriptorSetDesc setDescription = {};
	};

	class RenderPass
	{
	public:
		struct PerFrameData
		{
			uint16									  descriptorSet	   = 0;
			Vector<Buffer>							  buffers		   = {};
			Vector<LinaGX::RenderPassColorAttachment> colorAttachments = {};
			LinaGX::RenderPassDepthStencilAttachment  depthStencil;
		};

		void Create(const RenderPassDescription& desc);
		void Destroy();
		void BindDescriptors(LinaGX::CommandStream* stream, uint32 frameIndex, uint16 pipelineLayout);
		void Begin(LinaGX::CommandStream* stream, const LinaGX::Viewport& vp, const LinaGX::ScissorsRect& scissors, uint32 frameIndex);
		void End(LinaGX::CommandStream* stream);

		bool CopyBuffers(uint32 frameIndex, LinaGX::CommandStream* copyStream);
		void AddBuffersToUploadQueue(uint32 frameIndex, ResourceUploadQueue& queue);

		void SetColorAttachment(uint32 frameIndex, uint32 index, const LinaGX::RenderPassColorAttachment& att);

		inline void DepthStencilAttachment(uint32 frameIndex, const LinaGX::RenderPassDepthStencilAttachment& att)
		{
			m_pfd[frameIndex].depthStencil = att;
		}

		inline uint16 GetDescriptorSet(uint32 frameIndex)
		{
			return m_pfd[frameIndex].descriptorSet;
		}

		inline Buffer& GetBuffer(uint32 frameIndex, StringID sid)
		{
			return m_pfd[frameIndex].buffers[m_bufferIndices[sid]];
		}

	private:
		PerFrameData			  m_pfd[FRAMES_IN_FLIGHT];
		HashMap<StringID, uint32> m_bufferIndices;
	};
} // namespace Lina
