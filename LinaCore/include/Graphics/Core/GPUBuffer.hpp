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

#ifndef GPUBuffer_HPP
#define GPUBuffer_HPP

#include "Platform/LinaGXIncl.hpp"
#include "Data/String.hpp"
namespace LinaGX
{
	class Instance;
}

namespace Lina
{
	class GPUBuffer
	{
	public:
		GPUBuffer()	 = default;
		~GPUBuffer() = default;
		void Create(LinaGX::Instance* lgx, uint32 hintFlags, uint32 size, const String& debugName = "GPUBuffer");
		void BufferData(size_t padding, void* data, size_t size);
		bool Copy(LinaGX::CommandStream* stream);
		void Destroy();

		inline uint32 GetGPUResource() const
		{
			return m_gpu;
		}

	private:
		static uint64 s_usedCPUVisibleGPUMemory;

		LinaGX::Instance* m_lgx						= nullptr;
		uint32			  m_staging					= 0;
		uint32			  m_gpu						= 0;
		uint32			  m_size					= 0;
		uint8*			  m_mapped					= nullptr;
		bool			  m_isCPUVisibleGPUResource = false;
	};
} // namespace Lina

#endif
