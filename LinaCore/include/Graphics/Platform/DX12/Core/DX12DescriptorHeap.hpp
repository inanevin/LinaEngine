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
	class Renderer;

	class DX12DescriptorHeap
	{
	public:
		DX12DescriptorHeap(Renderer* renderer, D3D12_DESCRIPTOR_HEAP_TYPE heapType, uint32 numDescriptors, bool isReferencedByShader);
		virtual ~DX12DescriptorHeap();

		ID3D12DescriptorHeap* GetHeap()
		{
			return m_dx12Heap;
		}
		D3D12_DESCRIPTOR_HEAP_TYPE GetHeapType()
		{
			return m_heapType;
		}
		D3D12_CPU_DESCRIPTOR_HANDLE GetHeapCPUStart()
		{
			return m_cpuStart;
		}
		D3D12_GPU_DESCRIPTOR_HANDLE GetHeapGPUStart()
		{
			return m_gpuStart;
		}
		uint32 GetMaxDescriptors()
		{
			return m_maxDescriptors;
		}
		uint32 GetDescriptorSize()
		{
			return m_descriptorSize;
		}

	protected:
		Renderer*					m_renderer			   = nullptr;
		ID3D12DescriptorHeap*		m_dx12Heap			   = nullptr;
		D3D12_DESCRIPTOR_HEAP_TYPE	m_heapType			   = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
		D3D12_CPU_DESCRIPTOR_HANDLE m_cpuStart			   = {};
		D3D12_GPU_DESCRIPTOR_HANDLE m_gpuStart			   = {};
		uint32						m_maxDescriptors	   = 0;
		uint32						m_descriptorSize	   = 0;
		bool						m_isReferencedByShader = false;
	};

} // namespace Lina

#endif