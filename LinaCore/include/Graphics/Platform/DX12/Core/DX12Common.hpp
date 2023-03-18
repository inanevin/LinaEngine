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

#ifndef D3D12COMMON_HPP
#define D3D12COMMON_HPP

#include "Graphics/Core/CommonGraphics.hpp"
#include "Graphics/Platform/DX12/SDK/d3dx12.h"
#include "Graphics/Platform/DX12/SDK/d3d12shader.h"
#include <dxgi1_6.h>
#include <dxgi1_6.h>
#include <wrl/client.h>

namespace Lina
{
	extern DXGI_FORMAT				  GetFormat(Format format);
	extern D3D12_COMMAND_LIST_TYPE	  GetCommandType(CommandType type);
	extern D3D12_PRIMITIVE_TOPOLOGY	  GetTopology(Topology topology);
	extern D3D12_RESOURCE_STATES	  GetResourceState(ResourceState state);
	extern D3D12_TEXTURE_ADDRESS_MODE GetAddressMode(SamplerAddressMode mode);
	extern D3D12_FILTER				  GetFilter(Filter minFilter, Filter magFilter);

	inline String HrToString(HRESULT hr)
	{
		char s_str[64] = {};
		sprintf_s(s_str, "HRESULT of 0x%08X", static_cast<UINT>(hr));
		return String(s_str);
	}

	class HrException : public std::runtime_error
	{
	public:
		HrException(HRESULT hr) : std::runtime_error(HrToString(hr).c_str()), m_hr(hr)
		{
		}
		HRESULT Error() const
		{
			return m_hr;
		}

	private:
		const HRESULT m_hr;
	};

	inline void ThrowIfFailed(HRESULT hr)
	{
		if (FAILED(hr))
		{
			throw HrException(hr);
		}
	}

	class DescriptorHandle
	{
	public:
		DescriptorHandle()
		{
			m_cpuHandle.ptr = NULL;
			m_gpuHandle.ptr = NULL;
			m_heapIndex		= 0;
		}

		D3D12_CPU_DESCRIPTOR_HANDLE GetCPUHandle() const
		{
			return m_cpuHandle;
		}
		D3D12_GPU_DESCRIPTOR_HANDLE GetGPUHandle() const
		{
			return m_gpuHandle;
		}
		uint32 GetHeapIndex() const
		{
			return m_heapIndex;
		}

		void SetCPUHandle(D3D12_CPU_DESCRIPTOR_HANDLE cpuHandle)
		{
			m_cpuHandle = cpuHandle;
		}
		void SetGPUHandle(D3D12_GPU_DESCRIPTOR_HANDLE gpuHandle)
		{
			m_gpuHandle = gpuHandle;
		}
		void SetHeapIndex(uint32 heapIndex)
		{
			m_heapIndex = heapIndex;
		}

		bool IsValid() const
		{
			return m_cpuHandle.ptr != NULL;
		}
		bool IsReferencedByShader() const
		{
			return m_gpuHandle.ptr != NULL;
		}

	private:
		D3D12_CPU_DESCRIPTOR_HANDLE m_cpuHandle = {};
		D3D12_GPU_DESCRIPTOR_HANDLE m_gpuHandle = {};
		uint32						m_heapIndex = 0;
	};

#ifndef LINA_PRODUCTION_BUILD
#define NAME_DX12_OBJECT(x, NAME) x->SetName(L#NAME)
#else
#define NAME_D3D12_OBJECT(x)
#define NAME_D3D12_OBJECT_INDEXED(x, n)
#endif
} // namespace Lina

#endif