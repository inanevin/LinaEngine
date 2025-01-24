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

#include "Common/SizeDefinitions.hpp"
#include "Common/Data/Queue.hpp"

namespace LinaGX
{
	struct DescriptorSetDesc;
} // namespace LinaGX

namespace Lina
{

	class DescriptorSet
	{
	public:
		DescriptorSet()	 = default;
		~DescriptorSet() = default;

		void Create(const LinaGX::DescriptorSetDesc& desc);
		void Destroy();
		void Allocate(uint32& outIndex);
		void Free(uint32 index);

		inline bool IsEmpty() const
		{
			return m_freeAllocations.size() == m_allocationCount;
		}
		inline bool IsAvailable() const
		{
			return !m_freeAllocations.empty();
		};
		inline uint16 GetGPUHandle() const
		{
			return m_handle;
		}

	private:
		uint16		  m_handle			= 0;
		uint32		  m_allocationCount = 0;
		Queue<uint32> m_freeAllocations;
	};

} // namespace Lina
