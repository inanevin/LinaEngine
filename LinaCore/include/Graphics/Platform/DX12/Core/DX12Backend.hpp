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

#ifndef D3D12Backend_HPP
#define D3D12Backend_HPP

#include "Graphics/Core/IGfxBackend.hpp"
#include "DX12DescriptorHeap.hpp"
#include "Data/HashMap.hpp"
#include "Core/StringID.hpp"
#include "Core/Common.hpp"

namespace D3D12MA
{
	class Allocator;
}

namespace Lina
{
	class DX12GfxManager;

	class DX12Backend : public IGfxBackend
	{
	public:
		DX12Backend(DX12GfxManager* manager) : m_gfxManager(manager), m_rtvHeap(manager), m_dsvHeap(manager), m_samplerHeap(manager), m_bufferHeap(manager){};
		virtual ~DX12Backend() = default;

		// Inherited via IGfxBackend
		virtual void Initialize(const SystemInitializationInfo& initInfo) override;
		virtual void Shutdown() override;

		inline Microsoft::WRL::ComPtr<IDXGIFactory4> GetFactory()
		{
			return m_factory;
		}

		inline Microsoft::WRL::ComPtr<ID3D12Device> GetDevice()
		{
			return m_device;
		}

		inline Microsoft::WRL::ComPtr<ID3D12CommandQueue> GetGraphicsQueue()
		{
			return m_graphicsQueue;
		}

		inline D3D12MA::Allocator* GetAllocator()
		{
			return m_dx12Allocator;
		}

		inline DX12DescriptorHeap& GetHeapRTV()
		{
			return m_rtvHeap;
		}

		inline  DX12DescriptorHeap& GetHeapDSV()
		{
			return m_dsvHeap;
		}

		inline DX12DescriptorHeap& GetHeapBuffer()
		{
			return m_bufferHeap;
		}

		inline DX12DescriptorHeap& GetHeapSampler()
		{
			return m_samplerHeap;
		}

	private:
		void GetHardwareAdapter(IDXGIFactory1* pFactory, IDXGIAdapter1** ppAdapter, PreferredGPUType gpuType);

	private:
		DX12GfxManager*							   m_gfxManager	   = nullptr;
		D3D12MA::Allocator*						   m_dx12Allocator = nullptr;
		Microsoft::WRL::ComPtr<IDXGIAdapter1>	   m_adapter	   = nullptr;
		Microsoft::WRL::ComPtr<ID3D12Device>	   m_device;
		Microsoft::WRL::ComPtr<ID3D12CommandQueue> m_graphicsQueue;
		Microsoft::WRL::ComPtr<ID3D12CommandQueue> m_copyQueue;
		Microsoft::WRL::ComPtr<IDXGIFactory4>	   m_factory;
		DX12DescriptorHeap						   m_rtvHeap;
		DX12DescriptorHeap						   m_bufferHeap;
		DX12DescriptorHeap						   m_dsvHeap;
		DX12DescriptorHeap						   m_samplerHeap;
	};
} // namespace Lina

#endif