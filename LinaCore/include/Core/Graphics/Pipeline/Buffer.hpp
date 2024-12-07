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

#include "Common/Data/String.hpp"

namespace LinaGX
{
	class CommandStream;
	enum class IndexType;
} // namespace LinaGX

namespace Lina
{
	class OStream;
	class IStream;

	class Buffer
	{
	public:
		Buffer()  = default;
		~Buffer() = default;

		void Create(uint32 hintFlags, uint32 size, const String& debugName = "GPUBuffer", bool stagingOnly = false);
		void BufferData(size_t padding, uint8* data, size_t size);
		void MemsetMapped(int32 v);
		bool Copy(LinaGX::CommandStream* stream);
		void Destroy();
		void SaveToStream(OStream& stream) const;
		void LoadFromStream(IStream& stream);
		void BindVertex(LinaGX::CommandStream* stream, uint32 vertexSize);
		void BindIndex(LinaGX::CommandStream* stream, LinaGX::IndexType indexType);
		void DumpToFile(const String& path);

		inline void MarkDirty()
		{
			m_bufferChanged = true;
		}
		inline uint32 GetGPUResource() const
		{
			return m_residesInGPU ? m_gpu : m_staging;
		}

		inline uint8* GetMapped() const
		{
			return m_mapped;
		}

		inline uint8* GetMappedGPU() const
		{
			return m_mappedGPU;
		}

		inline uint32 GetSize() const
		{
			return m_size;
		}

		inline uint32 GetIndirectCount() const
		{
			return m_indirectCount;
		}

		inline void SetIndirectCount(uint32 count)
		{
			m_indirectCount = count;
		}

	private:
		static Atomic<uint64> s_usedCPUVisibleGPUMemory;

		bool   m_stagingOnly			 = false;
		uint32 m_staging				 = 0;
		uint32 m_gpu					 = 0;
		uint32 m_size					 = 0;
		uint8* m_mapped					 = nullptr;
		uint8* m_mappedGPU				 = nullptr;
		uint32 m_hintFlags				 = 0;
		bool   m_isCPUVisibleGPUResource = false;
		bool   m_residesInGPU			 = false;
		bool   m_bufferChanged			 = false;

		uint32 m_indirectCount = 0;
	};
} // namespace Lina
