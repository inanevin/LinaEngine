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

#include "Core/Graphics/Pipeline/Buffer.hpp"
#include "Common/Platform/LinaGXIncl.hpp"
#include "Common/Data/Streams.hpp"
#include "Common/Serialization/Serialization.hpp"
#include "Core/Application.hpp"

namespace Lina
{
	Atomic<uint64> Buffer::s_usedCPUVisibleGPUMemory = 0;

	void Buffer::Create(uint32 hintFlags, uint32 size, const String& debugName, bool stagingOnly)
	{
		m_size		  = size;
		m_stagingOnly = stagingOnly;
		m_hintFlags	  = hintFlags;

		if (!stagingOnly && LinaGX::GPUInfo.totalCPUVisibleGPUMemorySize - s_usedCPUVisibleGPUMemory.load() > m_size)
		{
			s_usedCPUVisibleGPUMemory.fetch_add(static_cast<uint64>(m_size));
			m_isCPUVisibleGPUResource = true;

			const String		 dbgName = debugName + " (CPUVisibleGPUMem)";
			LinaGX::ResourceDesc desc	 = {
				   .size		  = static_cast<uint64>(m_size),
				   .typeHintFlags = hintFlags,
				   .heapType	  = LinaGX::ResourceHeap::CPUVisibleGPUMemory,
				   .debugName	  = dbgName.c_str(),
			   };

			m_gpu = Application::GetLGX()->CreateResource(desc);
			Application::GetLGX()->MapResource(m_gpu, m_mapped);
			m_residesInGPU = true;
		}
		else
		{
			const String dbgNameStg = debugName + " (Staging)";
			const String dbgNameGPU = debugName + " (GPU)";

			LinaGX::ResourceDesc desc = {
				.size		   = static_cast<uint64>(m_size),
				.typeHintFlags = hintFlags,
				.heapType	   = LinaGX::ResourceHeap::StagingHeap,
				.debugName	   = dbgNameStg.c_str(),
			};

			m_staging = Application::GetLGX()->CreateResource(desc);

			if (!m_stagingOnly)
			{
				desc.heapType  = LinaGX::ResourceHeap::GPUOnly;
				desc.debugName = dbgNameGPU.c_str();
				m_gpu		   = Application::GetLGX()->CreateResource(desc);
				m_residesInGPU = true;
			}
			else
				m_residesInGPU = false;

			Application::GetLGX()->MapResource(m_staging, m_mapped);
		}

		// if (hintFlags & LinaGX::ResourceTypeHint::TH_ReadbackDest)
		// 	Application::GetLGX()->MapResource(m_gpu, m_mappedGPU);
	}

	void Buffer::BufferData(size_t padding, uint8* data, size_t size)
	{
		if (size == 0)
			return;

		LINA_ASSERT(padding + size <= m_size, "Buffer overflow!");
		MEMCPY(m_mapped + padding, data, size);
		m_bufferChanged = true;
	}

	void Buffer::MemsetMapped(int32 v)
	{
		MEMSET(m_mapped, v, m_size);
		m_bufferChanged = true;
	}

	bool Buffer::Copy(LinaGX::CommandStream* stream)
	{
		// no need either we already wrote to gpu mem or memory buffer.
		if (m_isCPUVisibleGPUResource || m_stagingOnly)
			return false;

		if (!m_bufferChanged)
			return false;

		LinaGX::CMDCopyResource* copy = stream->AddCommand<LinaGX::CMDCopyResource>();
		copy->destination			  = m_gpu;
		copy->source				  = m_staging;
		m_bufferChanged				  = false;

		return true;
	}

	void Buffer::Destroy()
	{
		if (m_residesInGPU)
		{
			Application::GetLGX()->UnmapResource(m_gpu);
			Application::GetLGX()->DestroyResource(m_gpu);
		}

		if (!m_isCPUVisibleGPUResource)
		{
			Application::GetLGX()->UnmapResource(m_staging);
			Application::GetLGX()->DestroyResource(m_staging);
		}
	}

	void Buffer::SaveToStream(OStream& stream) const
	{
		stream << m_size;
		stream << m_hintFlags;
		stream << m_stagingOnly;
		stream.WriteRaw(m_mapped, m_size);
	}

	void Buffer::LoadFromStream(IStream& stream)
	{
		stream >> m_size;
		stream >> m_hintFlags;
		stream >> m_stagingOnly;
		Create(m_hintFlags, m_size, "", m_stagingOnly);
		stream.ReadToRaw((void*)m_mapped, m_size);
	}

	void Buffer::BindVertex(LinaGX::CommandStream* stream, uint32 vertexSize)
	{
		LinaGX::CMDBindVertexBuffers* vtx = stream->AddCommand<LinaGX::CMDBindVertexBuffers>();
		vtx->offset						  = 0;
		vtx->slot						  = 0;
		vtx->vertexSize					  = vertexSize;
		vtx->resource					  = GetGPUResource();
	}

	void Buffer::BindIndex(LinaGX::CommandStream* stream, LinaGX::IndexType indexType)
	{
		LinaGX::CMDBindIndexBuffers* index = stream->AddCommand<LinaGX::CMDBindIndexBuffers>();
		index->indexType				   = indexType;
		index->offset					   = 0;
		index->resource					   = GetGPUResource();
	}

	void Buffer::DumpToFile(const String& path)
	{
		OStream stream;
		stream.WriteRaw(m_mapped, static_cast<size_t>(m_size));
		Serialization::SaveToFile(path.c_str(), stream);
		stream.Destroy();
	}

} // namespace Lina
