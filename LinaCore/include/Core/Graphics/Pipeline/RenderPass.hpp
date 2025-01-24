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
#include "Core/Graphics/Pipeline/View.hpp"
#include "Common/Platform/LinaGXIncl.hpp"
#include "Common/Math/Rect.hpp"
#include "Common/Data/Vector.hpp"
#include "Buffer.hpp"

namespace Lina
{
	class ResourceUploadQueue;

	struct RenderPassBuffer
	{
		LinaGX::ResourceTypeHint bufferType		= LinaGX::ResourceTypeHint::TH_ConstantBuffer;
		String					 debugName		= "";
		size_t					 size			= 0;
		bool					 stagingOnly	= false;
		int32					 bindingIndex	= -1;
		StringID				 ident			= 0;
		uint32					 createdOutside = false;
	};

	struct RenderPassDescription
	{
		Vector<RenderPassBuffer>  buffers;
		LinaGX::DescriptorSetDesc setDescription = {};
	};

	class RenderPass
	{
	public:
		struct InstancedDraw
		{
			BufferedGroup<Buffer, FRAMES_IN_FLIGHT> vertexBuffers;
			BufferedGroup<Buffer, FRAMES_IN_FLIGHT> indexBuffers;
			size_t									vertexSize	  = 0;
			uint32									shaderHandle  = 0;
			uint32									baseVertex	  = 0;
			uint32									vertexCount	  = 0;
			uint32									baseIndex	  = 0;
			uint32									indexCount	  = 0;
			uint32									instanceCount = 0;
			uint32									baseInstance  = 0;
			uint32									pushConstant  = 0;
			Recti									clip		  = {};
			bool									useScissors	  = false;
			bool									sortOrder	  = false;
			Vector3									sortPosition  = Vector3::Zero;
			float									_sortDistance = 0.0f;
		};

		struct RenderingData
		{
			Vector<InstancedDraw> drawCalls;
		};

		struct PerFrameData
		{
			uint16									  descriptorSet	   = 0;
			Vector<Buffer>							  buffers		   = {};
			Vector<LinaGX::RenderPassColorAttachment> colorAttachments = {};
			LinaGX::RenderPassDepthStencilAttachment  depthStencil;
		};

		void Create(const RenderPassDescription& desc);
		void Destroy();
		void BindDescriptors(LinaGX::CommandStream* stream, uint32 frameIndex, uint16 pipelineLayout, uint32 firstSet);
		void Begin(LinaGX::CommandStream* stream, uint32 frameIndex);
		void End(LinaGX::CommandStream* stream);

		void AddDrawCall(const InstancedDraw& draw);

		bool CopyBuffers(uint32 frameIndex, LinaGX::CommandStream* copyStream);
		void SyncRender();
		void AddBuffersToUploadQueue(uint32 frameIndex, ResourceUploadQueue& queue);
		void Render(uint32 frameIndex, LinaGX::CommandStream* stream);

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

		inline View& GetView()
		{
			return m_view;
		}

		inline const Vector<InstancedDraw>& GetDrawCallsGPU() const
		{
			return m_gpuData.drawCalls;
		}

		inline const Vector<InstancedDraw>& GetDrawCallsCPU() const
		{
			return m_cpuData.drawCalls;
		}

		inline void SetSize(const Vector2ui& sz)
		{
			m_size = sz;
		}

	private:
		View					  m_view = {};
		PerFrameData			  m_pfd[FRAMES_IN_FLIGHT];
		HashMap<StringID, uint32> m_bufferIndices;
		RenderingData			  m_gpuData		= {};
		RenderingData			  m_cpuData		= {};
		Vector2ui				  m_size		= {};
		Vector<InstancedDraw>	  m_usedDraws	= {};
		Vector<InstancedDraw>	  m_sortedDraws = {};
	};
} // namespace Lina
