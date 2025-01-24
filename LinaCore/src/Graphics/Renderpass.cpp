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

#include "Core/Graphics/GPUBuffer.hpp"

namespace Lina
{
	uint64 GPUBuffer::s_usedCPUVisibleGPUMemory = 0;

	void GPUBuffer::Create(LinaGX::Instance* lgx, uint32 hintFlags, uint32 size, const String& debugName, bool stagingOnly)
	{
		m_size		  = size;
		m_lgx		  = lgx;
		m_stagingOnly = stagingOnly;

		if (!m_stagingOnly && LinaGX::GPUInfo.totalCPUVisibleGPUMemorySize - s_usedCPUVisibleGPUMemory > m_size)
		{
			s_usedCPUVisibleGPUMemory += static_cast<uint64>(m_size);
			m_isCPUVisibleGPUResource = true;

			LinaGX::ResourceDesc desc = {
				.size		   = static_cast<uint64>(m_size),
				.typeHintFlags = hintFlags,
				.heapType	   = LinaGX::ResourceHeap::CPUVisibleGPUMemory,
				.debugName	   = debugName.c_str(),
			};

			m_gpu = m_lgx->CreateResource(desc);
			m_lgx->MapResource(m_gpu, m_mapped);
		}
		else
		{
			LinaGX::ResourceDesc desc = {
				.size		   = static_cast<uint64>(m_size),
				.typeHintFlags = hintFlags,
				.heapType	   = LinaGX::ResourceHeap::StagingHeap,
				.debugName	   = debugName.c_str(),
			};

			m_staging = m_lgx->CreateResource(desc);

			if (!m_stagingOnly)
			{
				desc.heapType = LinaGX::ResourceHeap::GPUOnly;
				m_gpu		  = m_lgx->CreateResource(desc);
			}

			m_lgx->MapResource(m_staging, m_mapped);
		}
	}

	void GPUBuffer::BufferData(size_t padding, void* data, size_t size)
	{
		LINA_ASSERT(padding + size < m_size, "Buffer overflow!");
		MEMCPY(m_mapped + padding, data, size);
	}

	bool GPUBuffer::Copy(LinaGX::CommandStream* stream)
	{
		// no need either we already wrote to gpu mem or memory buffer.
		if (m_isCPUVisibleGPUResource || m_stagingOnly)
			return false;

		LinaGX::CMDCopyResource* copy = stream->AddCommand<LinaGX::CMDCopyResource>();
		copy->destination			  = m_gpu;
		copy->source				  = m_staging;

		return true;
	}

	void GPUBuffer::Destroy()
	{
		if (m_isCPUVisibleGPUResource && !m_stagingOnly)
		{
			m_lgx->UnmapResource(m_gpu);
			m_lgx->DestroyResource(m_gpu);
		}
		else
		{
			m_lgx->UnmapResource(m_staging);
			m_lgx->DestroyResource(m_staging);

			if (!m_stagingOnly)
				m_lgx->DestroyResource(m_gpu);
		}
	}

} // namespace Lina
