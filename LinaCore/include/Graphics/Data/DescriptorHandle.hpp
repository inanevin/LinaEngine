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

#ifndef DescriptorHandle_HPP
#define DescriptorHandle_HPP

#include "Core/SizeDefinitions.hpp"

namespace Lina
{
	class DescriptorHandle
	{
	public:
		DescriptorHandle()
		{
			m_cpuHandle = NULL;
			m_gpuHandle = NULL;
			m_heapIndex = 0;
		}

		size_t GetCPUHandle() const
		{
			return m_cpuHandle;
		}
		uint64 GetGPUHandle() const
		{
			return m_gpuHandle;
		}
		uint32 GetHeapIndex() const
		{
			return m_heapIndex;
		}

		void SetCPUHandle(size_t cpuHandle)
		{
			m_cpuHandle = cpuHandle;
		}

		void SetGPUHandle(uint64 gpuHandle)
		{
			m_gpuHandle = gpuHandle;
		}

		void SetHeapIndex(uint32 heapIndex)
		{
			m_heapIndex = heapIndex;
		}

		bool IsValid() const
		{
			return m_cpuHandle != NULL;
		}

		bool IsReferencedByShader() const
		{
			return m_gpuHandle != NULL;
		}

	private:
		size_t m_cpuHandle = NULL;
		uint64 m_gpuHandle = NULL;
		uint32 m_heapIndex = 0;
	};

} // namespace Lina

#endif
