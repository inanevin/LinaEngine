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

#ifndef D3D12DescriptorHeap_HPP
#define D3D12DescriptorHeap_HPP

#include "Core/SizeDefinitions.hpp"
#include "Data/IDList.hpp"
#include "Graphics/Platform/DX12/Core/DX12Common.hpp"

namespace Lina
{
	
	class DX12GfxManager;

	class DX12DescriptorHeap
	{
	public:
		DX12DescriptorHeap(DX12GfxManager* manager);
		~DX12DescriptorHeap();

		void Create(D3D12_DESCRIPTOR_HEAP_TYPE type, D3D12_DESCRIPTOR_HEAP_FLAGS flags, uint32 maxSize);

		DescriptorHandle Allocate();
		void			 Free(DescriptorHandle& handle);
		void			 Free(uint32 id);

		inline uint32 GetIncrementSize() const
		{
			return m_incrementSize;
		}

	private:
		DX12GfxManager*								 m_gfxManager = nullptr;
		Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> m_heap;
		uint32										 m_maxSize		 = 0;
		uint32										 m_incrementSize = 0;
		D3D12_DESCRIPTOR_HEAP_TYPE					 m_type			 = {};
		D3D12_CPU_DESCRIPTOR_HANDLE					 m_cpuStart		 = {};
		D3D12_GPU_DESCRIPTOR_HANDLE					 m_gpuStart		 = {};
		D3D12_DESCRIPTOR_HEAP_FLAGS					 m_flags		 = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
		IDList<int32>								 m_freeIndices;
		bool										 m_isGpuVisible = false;
	};

} // namespace Lina

#endif