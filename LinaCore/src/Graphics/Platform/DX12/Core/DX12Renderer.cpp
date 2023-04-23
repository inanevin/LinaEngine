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

#include "Graphics/Platform/DX12/Core/DX12Renderer.hpp"
#include "Graphics/Platform/DX12/SDK/D3D12MemAlloc.h"
#include "Graphics/Platform/DX12/Core/DX12Swapchain.hpp"
#include "Graphics/Platform/DX12/Core/DX12ResourceCPU.hpp"
#include "Graphics/Platform/DX12/Core/DX12ResourceGPU.hpp"
#include "Graphics/Platform/DX12/Core/DX12ResourceTexture.hpp"
#include "Graphics/Platform/DX12/Core/DX12StagingHeap.hpp"
#include "Graphics/Platform/DX12/Core/DX12GPUHeap.hpp"
#include "Graphics/Platform/DX12/Core/DX12UploadContext.hpp"
#include "Graphics/Platform/DX12/Core/DX12Pipeline.hpp"
#include "Graphics/Platform/DX12/Core/DX12GfxContext.hpp"
#include "Graphics/Core/GfxManager.hpp"
#include "Graphics/Resource/Shader.hpp"
#include "Graphics/Resource/Texture.hpp"
#include "Graphics/Resource/Material.hpp"
#include "Graphics/Resource/TextureSampler.hpp"
#include "FileSystem/FileSystem.hpp"
#include "System/ISystem.hpp"
#include "Resources/Core/ResourceManager.hpp"
#include "Math/Rect.hpp"
#include "Math/Color.hpp"
#include "Log/Log.hpp"
#include "Profiling/Profiler.hpp"
#include "Core/PlatformTime.hpp"
#include "Core/PlatformProcess.hpp"
#include "Core/SystemInfo.hpp"
#include "Math/Math.hpp"

// Debug
#include "Data/Array.hpp"
#include "World/Core/EntityWorld.hpp"

#ifndef LINA_PRODUCTION_BUILD
#include "FileSystem/FileSystem.hpp"
#endif

using Microsoft::WRL::ComPtr;

LINA_DISABLE_VC_WARNING(6387);

namespace Lina
{
	DWORD msgCallback = 0;

	void MessageCallback(D3D12_MESSAGE_CATEGORY messageType, D3D12_MESSAGE_SEVERITY severity, D3D12_MESSAGE_ID messageId, LPCSTR pDesc, void* pContext)
	{
		if (pDesc != NULL)
		{
			if (severity == D3D12_MESSAGE_SEVERITY_ERROR)
			{
				LINA_ERR("[D3D12 Debug Layer] -> {0}", pDesc);
			}
			else if (severity == D3D12_MESSAGE_SEVERITY_WARNING)
			{
				LINA_WARN("[D3D12 Debug Layer] -> {0}", pDesc);
			}
			else if (severity == D3D12_MESSAGE_SEVERITY_CORRUPTION)
			{
				LINA_CRITICAL("[D3D12 Debug Layer] -> {0}", pDesc);
			}
			else if (severity == D3D12_MESSAGE_SEVERITY_MESSAGE)
			{
				LINA_INFO("[D3D12 Debug Layer] -> {0}", pDesc);
			}
		}
	}

	// ********************** BACKEND ********************************
	// ********************** BACKEND ********************************
	// ********************** BACKEND ********************************

	Renderer::Renderer(const SystemInitializationInfo& initInfo, GfxManager* gfxMan) : m_fences(10, Microsoft::WRL::ComPtr<ID3D12Fence>())
	{
		m_gfxManager					 = gfxMan;
		m_resourceManager				 = m_gfxManager->GetSystem()->CastSubsystem<ResourceManager>(SubsystemType::ResourceManager);
		m_vsync							 = initInfo.vsyncMode;
		m_initInfo						 = initInfo;
		m_lastVsyncTimestampMicroseconds = 0;

		try
		{
			{
				UINT dxgiFactoryFlags = 0;
				// Enable the debug layer (requires the Graphics Tools "optional feature").
				// NOTE: Enabling the debug layer after device creation will invalidate the active device.
#ifndef LINA_PRODUCTION_BUILD
				{
					ComPtr<ID3D12Debug> debugController;
					if (SUCCEEDED(D3D12GetDebugInterface(IID_PPV_ARGS(&debugController))))
					{
						debugController->EnableDebugLayer();

						// Enable additional debug layers.
						dxgiFactoryFlags |= DXGI_CREATE_FACTORY_DEBUG;
					}
					else
					{
						LINA_ERR("[Gfx Backend] -> Failed enabling debug layers!");
					}
				}
#endif

				ThrowIfFailed(CreateDXGIFactory2(dxgiFactoryFlags, IID_PPV_ARGS(&m_factory)));

				ComPtr<IDXGIFactory5> factory5;
				HRESULT				  facres	   = m_factory.As(&factory5);
				BOOL				  allowTearing = FALSE;
				if (SUCCEEDED(facres))
				{
					facres = factory5->CheckFeatureSupport(DXGI_FEATURE_PRESENT_ALLOW_TEARING, &allowTearing, sizeof(allowTearing));
				}

				m_allowTearing = SUCCEEDED(facres) && allowTearing;
			}

			// Choose gpu & create device
			{
				if (initInfo.preferredGPUType == PreferredGPUType::CPU)
				{
					ComPtr<IDXGIAdapter> warpAdapter;
					ThrowIfFailed(m_factory->EnumWarpAdapter(IID_PPV_ARGS(&warpAdapter)));

					ThrowIfFailed(D3D12CreateDevice(warpAdapter.Get(), D3D_FEATURE_LEVEL_11_0, IID_PPV_ARGS(&m_device)));
				}
				else
				{
					GetHardwareAdapter(m_factory.Get(), &m_adapter, initInfo.preferredGPUType);

					ThrowIfFailed(D3D12CreateDevice(m_adapter.Get(), D3D_FEATURE_LEVEL_11_0, IID_PPV_ARGS(&m_device)));
				}
			}

#ifndef LINA_PRODUCTION_BUILD
			// Dbg callback
			{
				ID3D12InfoQueue1* infoQueue = nullptr;
				if (SUCCEEDED(m_device->QueryInterface<ID3D12InfoQueue1>(&infoQueue)))
				{
					infoQueue->RegisterMessageCallback(&MessageCallback, D3D12_MESSAGE_CALLBACK_IGNORE_FILTERS, nullptr, &msgCallback);
				}
			}
#endif

			// Describe and create the command queues.
			{
				m_contextCompute  = new DX12GfxContext(this, CommandType::Compute);
				m_contextTransfer = new DX12GfxContext(this, CommandType::Transfer);
				m_contextGraphics = new DX12GfxContext(this, CommandType::Graphics);
			}

			// Heaps
			{
				for (int i = 0; i < FRAMES_IN_FLIGHT; i++)
				{
					m_gpuBufferHeap[i]	= new DX12GPUHeap(this, D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV, 100);
					m_gpuSamplerHeap[i] = new DX12GPUHeap(this, D3D12_DESCRIPTOR_HEAP_TYPE_SAMPLER, 100);
					NAME_DX12_OBJECT(m_gpuBufferHeap[i]->GetHeap(), L"Linear GPU Buffer Heap");
					NAME_DX12_OBJECT(m_gpuSamplerHeap[i]->GetHeap(), L"Linear GPU Sampler Heap");
				}

				m_bufferHeap  = new DX12StagingHeap(this, D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV, 1024);
				m_textureHeap = new DX12StagingHeap(this, D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV, 1024);
				m_dsvHeap	  = new DX12StagingHeap(this, D3D12_DESCRIPTOR_HEAP_TYPE_DSV, 512);
				m_rtvHeap	  = new DX12StagingHeap(this, D3D12_DESCRIPTOR_HEAP_TYPE_RTV, 512);
				m_samplerHeap = new DX12StagingHeap(this, D3D12_DESCRIPTOR_HEAP_TYPE_SAMPLER, 100);

				NAME_DX12_OBJECT(m_bufferHeap->GetHeap(), L"Buffer Heap");
				NAME_DX12_OBJECT(m_textureHeap->GetHeap(), L"Texture Heap");
				NAME_DX12_OBJECT(m_dsvHeap->GetHeap(), L"DSV Heap");
				NAME_DX12_OBJECT(m_rtvHeap->GetHeap(), L"RTV Heap");
				NAME_DX12_OBJECT(m_samplerHeap->GetHeap(), L"Sampler Heap");
			}

			// Allocator
			{
				D3D12MA::ALLOCATOR_DESC allocatorDesc;
				allocatorDesc.pDevice			   = m_device.Get();
				allocatorDesc.PreferredBlockSize   = 0;
				allocatorDesc.Flags				   = D3D12MA::ALLOCATOR_FLAG_NONE;
				allocatorDesc.pAdapter			   = m_adapter.Get();
				allocatorDesc.pAllocationCallbacks = NULL;
				ThrowIfFailed(D3D12MA::CreateAllocator(&allocatorDesc, &m_dx12Allocator));
			}

			// DXC
			{
				HRESULT hr = DxcCreateInstance(CLSID_DxcLibrary, IID_PPV_ARGS(&m_idxcLib));
				if (FAILED(hr))
				{
					LINA_CRITICAL("[D3D12 Gpu Storage] -> Failed to create DXC library!");
				}
			}

			// Standard Pipeline Root Signature
			{

				CD3DX12_DESCRIPTOR_RANGE1 descRange[3] = {};

				// Textures & Samplers & Materials
				descRange[0].Init(D3D12_DESCRIPTOR_RANGE_TYPE_SRV, UINT_MAX, 0, 0, D3D12_DESCRIPTOR_RANGE_FLAG_DESCRIPTORS_VOLATILE);
				descRange[1].Init(D3D12_DESCRIPTOR_RANGE_TYPE_SAMPLER, UINT_MAX, 0, 0, D3D12_DESCRIPTOR_RANGE_FLAG_DESCRIPTORS_VOLATILE);
				descRange[2].Init(D3D12_DESCRIPTOR_RANGE_TYPE_SRV, UINT_MAX, 0, 2, D3D12_DESCRIPTOR_RANGE_FLAG_DESCRIPTORS_VOLATILE);

				CD3DX12_ROOT_PARAMETER1 rp[GBB_Max] = {};

				// Global constant buffer
				rp[GBB_GlobalData].InitAsConstantBufferView(0, 0, D3D12_ROOT_DESCRIPTOR_FLAG_NONE, D3D12_SHADER_VISIBILITY_VERTEX);

				// Indirect constant
				rp[GBB_IndirectData].InitAsConstants(2, 1, 0, D3D12_SHADER_VISIBILITY_ALL);

				// Scene data
				rp[GBB_SceneData].InitAsConstantBufferView(2, 0, D3D12_ROOT_DESCRIPTOR_FLAG_NONE, D3D12_SHADER_VISIBILITY_VERTEX);

				// View data
				rp[GBB_ViewData].InitAsConstantBufferView(3, 0, D3D12_ROOT_DESCRIPTOR_FLAG_NONE, D3D12_SHADER_VISIBILITY_VERTEX);

				// Object data
				rp[GBB_ObjData].InitAsShaderResourceView(0, 1, D3D12_ROOT_DESCRIPTOR_FLAG_DATA_VOLATILE, D3D12_SHADER_VISIBILITY_VERTEX);

				// Material Data
				rp[GBB_MatData].InitAsDescriptorTable(1, &descRange[2], D3D12_SHADER_VISIBILITY_ALL);

				// Texture data
				rp[GBB_TxtData].InitAsDescriptorTable(1, &descRange[0], D3D12_SHADER_VISIBILITY_ALL);

				// Sampler data
				rp[GBB_SamplerData].InitAsDescriptorTable(1, &descRange[1], D3D12_SHADER_VISIBILITY_ALL);

				// CD3DX12_STATIC_SAMPLER_DESC samplerDesc[1] = {};
				// samplerDesc[0].Init(0, D3D12_FILTER_ANISOTROPIC);

				CD3DX12_VERSIONED_ROOT_SIGNATURE_DESC rootSig(GBB_Max, rp, 0, NULL, D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT);
				ComPtr<ID3DBlob>					  signatureBlob = nullptr;
				ComPtr<ID3DBlob>					  errorBlob		= nullptr;

				HRESULT hr = D3D12SerializeVersionedRootSignature(&rootSig, &signatureBlob, &errorBlob);

				if (FAILED(hr))
				{
					LINA_CRITICAL("[Shader Compiler] -> Failed serializing root signature! {0}", (char*)errorBlob->GetBufferPointer());
				}

				hr = m_device->CreateRootSignature(0, signatureBlob->GetBufferPointer(), signatureBlob->GetBufferSize(), IID_PPV_ARGS(&m_rootSigStandard));

				if (FAILED(hr))
				{
					LINA_CRITICAL("[Shader Compiler] -> Failed creating root signature!");
				}
			}

			// Indirect command signature
			{
				D3D12_INDIRECT_ARGUMENT_DESC argumentDescs[2]	  = {};
				argumentDescs[0].Type							  = D3D12_INDIRECT_ARGUMENT_TYPE_CONSTANT;
				argumentDescs[0].Constant.RootParameterIndex	  = 1;
				argumentDescs[0].Constant.DestOffsetIn32BitValues = 0;
				argumentDescs[0].Constant.Num32BitValuesToSet	  = 2;
				argumentDescs[1].Type							  = D3D12_INDIRECT_ARGUMENT_TYPE_DRAW_INDEXED;

				D3D12_COMMAND_SIGNATURE_DESC commandSignatureDesc = {};
				commandSignatureDesc.pArgumentDescs				  = argumentDescs;
				commandSignatureDesc.NumArgumentDescs			  = _countof(argumentDescs);
				commandSignatureDesc.ByteStride					  = sizeof(DrawIndexedIndirectCommand);

				ThrowIfFailed(m_device->CreateCommandSignature(&commandSignatureDesc, m_rootSigStandard.Get(), IID_PPV_ARGS(&m_commandSigStandard)));
			}

			// Sycnronization resources
			{
				m_frameFenceGraphics = CreateFence();
				m_frameFenceTransfer = CreateFence();
				m_fenceValueGraphics = 1;
				m_fenceValueTransfer = 1;
			}

			m_uploadContext = new DX12UploadContext(this);
		}
		catch (HrException e)
		{
			LINA_CRITICAL("[Renderer] -> Exception when pre-initializating renderer! {0}", e.what());
			DX12Exception(e);
		}
	}

	void Renderer::Shutdown()
	{
		ReleaseFence(m_frameFenceGraphics);
		ReleaseFence(m_frameFenceTransfer);

		ID3D12InfoQueue1* infoQueue = nullptr;
		if (SUCCEEDED(m_device->QueryInterface<ID3D12InfoQueue1>(&infoQueue)))
		{
			infoQueue->UnregisterMessageCallback(msgCallback);
		}

		delete m_contextCompute;
		delete m_contextTransfer;
		delete m_contextGraphics;

		m_dx12Allocator->Release();
		m_commandSigStandard.Reset();
		m_rootSigStandard.Reset();
		m_idxcLib.Reset();
		m_device.Reset();

		for (int i = 0; i < FRAMES_IN_FLIGHT; i++)
		{
			m_gpuBufferHeap[i]->Reset();
			m_gpuSamplerHeap[i]->Reset();
			delete m_gpuBufferHeap[i];
			delete m_gpuSamplerHeap[i];
		}

		delete m_uploadContext;
		delete m_bufferHeap;
		delete m_textureHeap;
		delete m_dsvHeap;
		delete m_samplerHeap;
		delete m_rtvHeap;
	}

	int64 CalculateRunningAverageRT(int64 deltaMicroseconds)
	{
		static uint32			historyIndex = 0;
		static Array<int64, 11> dtHistory	 = {{IDEAL_RT, IDEAL_RT, IDEAL_RT, IDEAL_RT, IDEAL_RT, IDEAL_RT, IDEAL_RT, IDEAL_RT, IDEAL_RT, IDEAL_RT, IDEAL_RT}};
		dtHistory[historyIndex]				 = deltaMicroseconds;
		historyIndex						 = (historyIndex + 1) % 11;
		linatl::quick_sort(dtHistory.begin(), dtHistory.end());
		int64 mean = 0;
		for (uint32 i = 2; i < 9; i++)
			mean += dtHistory[i];
		return mean / 7;
	}

	void Renderer::WaitForSwapchains(ISwapchain* swapchain)
	{
		DX12Swapchain* dx12Swap = static_cast<DX12Swapchain*>(swapchain);

		DWORD result = WaitForSingleObjectEx(dx12Swap->DX12GetWaitHandle(), 10000, true);
		if (FAILED(result))
		{
			LINA_ERR("[Renderer] -> Waiting on swapchain failed!");
		}

		DXGI_FRAME_STATISTICS FrameStatistics;
		dx12Swap->GetPtr()->GetFrameStatistics(&FrameStatistics);

		// Average refresh rate calculation
		{
			if (m_previousRefreshCount == 0)
				m_previousPresentRefreshCount = FrameStatistics.PresentRefreshCount;

			if (FrameStatistics.PresentRefreshCount > m_previousPresentRefreshCount)
			{
				m_previousPresentRefreshCount = FrameStatistics.PresentRefreshCount;

				// LINA_WARN("VSYNC - Present Count {0}", FrameStatistics.PresentCount);
				static uint32 vsyncCounter	  = 0;
				static int64  lastVsyncCycles = PlatformTime::GetCPUCycles();
				const int64	  deltaVsync	  = PlatformTime::GetDeltaMicroseconds64(lastVsyncCycles, FrameStatistics.SyncQPCTime.QuadPart);
				lastVsyncCycles				  = FrameStatistics.SyncQPCTime.QuadPart;

				if (deltaVsync > 0 && vsyncCounter % 100 == 0)
					m_averageVsyncMicroseconds = CalculateRunningAverageRT(deltaVsync);

				vsyncCounter++;
				m_lastVsyncTimestampMicroseconds = (FrameStatistics.SyncQPCTime.QuadPart * 1000000ll) / PlatformTime::GetFrequency();
				m_nextVsyncTimestampMicroseconds = m_lastVsyncTimestampMicroseconds + m_averageVsyncMicroseconds;
			}
		}
	}

	bool Renderer::Present(ISwapchain* swp)
	{
		DX12Swapchain* dx12Swap = static_cast<DX12Swapchain*>(swp);

		try
		{
			UINT   flags	= m_allowTearing ? DXGI_PRESENT_ALLOW_TEARING : 0;
			uint32 interval = 0;

			if (m_vsync == VsyncMode::EveryVBlank)
			{
				interval = 1;
				flags	 = 0;
			}
			else if (m_vsync == VsyncMode::EverySecondVBlank)
			{
				interval = 2;
				flags	 = 0;
			}

			ThrowIfFailed(dx12Swap->GetPtr()->Present(interval, flags));

			DXGI_FRAME_STATISTICS FrameStatistics;
			dx12Swap->GetPtr()->GetFrameStatistics(&FrameStatistics);

			if (FrameStatistics.PresentCount > m_previousPresentCount)
			{
				if (m_previousRefreshCount > 0 && (FrameStatistics.PresentRefreshCount - m_previousRefreshCount) > (FrameStatistics.PresentCount - m_previousPresentCount))
				{
					++m_glitchCount;
					interval = 0;
					// LINA_WARN("glitch {0}", m_glitchCount);
				}
			}

			m_previousPresentCount = FrameStatistics.PresentCount;
			m_previousRefreshCount = FrameStatistics.SyncRefreshCount;
		}
		catch (HrException& e)
		{
			LINA_CRITICAL("[Renderer] -> Present engine error! {0}", e.what());
			DX12Exception(e);
			return false;
		}

		return true;
	}

	void Renderer::BeginFrame(uint32 frameIndex)
	{

		m_currentFrameIndex = frameIndex;
		LINA_TRACE("1s");
		m_contextGraphics->WaitForFences(m_frameFenceGraphics, m_frames[frameIndex].storedFenceGraphics);
		LINA_TRACE("2");
		m_contextTransfer->WaitForFences(m_frameFenceTransfer, m_frames[frameIndex].storedFenceTransfer);
		LINA_TRACE("4");

		m_gpuBufferHeap[m_currentFrameIndex]->Reset(m_texturesHeapAllocCount);
		m_gpuSamplerHeap[m_currentFrameIndex]->Reset(m_samplersHeapAllocCount);
	}

	void Renderer::EndFrame(uint32 frameIndex)
	{
		auto& frame = m_frames[frameIndex];
		m_fenceValueGraphics++;
		m_fenceValueTransfer++;
		frame.storedFenceGraphics = m_fenceValueGraphics;
		frame.storedFenceTransfer = m_fenceValueTransfer;
		m_contextGraphics->Signal(m_frameFenceGraphics, m_fenceValueGraphics);
		m_contextTransfer->Signal(m_frameFenceTransfer, m_fenceValueTransfer);

	}

	void Renderer::Join()
	{
		for (int i = 0; i < FRAMES_IN_FLIGHT; i++)
		{
			m_contextGraphics->WaitForFences(m_frameFenceGraphics, m_frames[i].storedFenceGraphics);
			m_contextTransfer->WaitForFences(m_frameFenceTransfer, m_frames[i].storedFenceTransfer);
		}
	}

	void Renderer::ResetResources()
	{
		// Copy textures and samplers
		{
			for (int i = 0; i < FRAMES_IN_FLIGHT; i++)
			{
				m_gpuBufferHeap[i]->Reset();
				m_gpuSamplerHeap[i]->Reset();

				// Bind global textures
				{
					// Strip only default 2D resources.
					const auto& allLoadedTextures = m_resourceManager->GetAllResourcesRaw<Texture>(false);

					// Prep
					m_texturesHeapAllocCount						  = static_cast<uint32>(allLoadedTextures.size());
					auto								targetHeap	  = m_gpuBufferHeap[i];
					const uint32						heapIncrement = targetHeap->GetDescriptorSize();
					auto								alloc		  = targetHeap->GetHeapHandleBlock(m_texturesHeapAllocCount);
					Vector<D3D12_CPU_DESCRIPTOR_HANDLE> srcDescriptors;
					Vector<D3D12_CPU_DESCRIPTOR_HANDLE> destDescriptors;
					srcDescriptors.resize(m_texturesHeapAllocCount, {});
					destDescriptors.resize(m_texturesHeapAllocCount, {});

					// Copy descriptors.
					Taskflow tf;
					tf.for_each_index(0, static_cast<int>(m_texturesHeapAllocCount), 1, [&](int i) {
						auto* texture = allLoadedTextures[i];
						texture->SetBindlessIndex(i);

						D3D12_CPU_DESCRIPTOR_HANDLE handle;
						handle.ptr		   = alloc.GetCPUHandle() + i * heapIncrement;
						destDescriptors[i] = handle;

						srcDescriptors[i] = {texture->GetDescriptor().GetCPUHandle()};
					});

					m_gfxManager->GetSystem()->GetMainExecutor()->RunAndWait(tf);
					m_device->CopyDescriptors(m_texturesHeapAllocCount, destDescriptors.data(), NULL, m_texturesHeapAllocCount, srcDescriptors.data(), NULL, D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
				}

				// Bind global samplers
				{
					const auto& allLoadedSamplers = m_resourceManager->GetAllResourcesRaw<TextureSampler>(true);

					auto targetHeap			 = m_gpuSamplerHeap[i];
					m_samplersHeapAllocCount = static_cast<uint32>(m_samplerHeap->GetActiveHandleCount());
					const uint32 increment	 = targetHeap->GetDescriptorSize();
					auto		 alloc		 = targetHeap->GetHeapHandleBlock(m_samplersHeapAllocCount);

					Vector<D3D12_CPU_DESCRIPTOR_HANDLE> srcDescriptors;
					Vector<D3D12_CPU_DESCRIPTOR_HANDLE> destDescriptors;
					srcDescriptors.resize(m_samplersHeapAllocCount, {});
					destDescriptors.resize(m_samplersHeapAllocCount, {});

					Taskflow tf;
					tf.for_each_index(0, static_cast<int>(m_samplersHeapAllocCount), 1, [&](int i) {
						auto& loadedSampler = allLoadedSamplers[i];
						loadedSampler->SetBindlessIndex(i);

						D3D12_CPU_DESCRIPTOR_HANDLE handle;
						handle.ptr		   = alloc.GetCPUHandle() + i * increment;
						destDescriptors[i] = handle;

						srcDescriptors[i] = {loadedSampler->GetDescriptor().GetCPUHandle()};
					});
					m_gfxManager->GetSystem()->GetMainExecutor()->RunAndWait(tf);

					m_device->CopyDescriptors(m_samplersHeapAllocCount, destDescriptors.data(), NULL, m_samplersHeapAllocCount, srcDescriptors.data(), NULL, D3D12_DESCRIPTOR_HEAP_TYPE_SAMPLER);
				}
			}
		}
	}

	void Renderer::OnWindowResized(IWindow* window, StringID sid, const Recti& rect)
	{
		Join();
		m_fenceValueGraphics = m_frames[m_currentFrameIndex].storedFenceGraphics;
		m_fenceValueTransfer = m_frames[m_currentFrameIndex].storedFenceTransfer;

		if (rect.size.x == 0 || rect.size.y == 0)
			return;

		Event ev;
		ev.pParams[0] = window;
		ev.iParams[0] = rect.size.x;
		ev.iParams[1] = rect.size.y;
		ev.iParams[2] = sid;
		m_gfxManager->GetSystem()->DispatchEvent(EVS_WindowResized, ev);
	}

	void Renderer::OnVsyncChanged(VsyncMode mode)
	{
		Join();
		m_vsync				 = mode;
		m_fenceValueGraphics = m_frames[m_currentFrameIndex].storedFenceGraphics;
		m_fenceValueTransfer = m_frames[m_currentFrameIndex].storedFenceTransfer;
		Event ev;
		ev.iParams[0] = static_cast<uint32>(mode);
		m_gfxManager->GetSystem()->DispatchEvent(EVS_VsyncModeChanged, ev);
	}

	void Renderer::GetHardwareAdapter(IDXGIFactory1* pFactory, IDXGIAdapter1** ppAdapter, PreferredGPUType gpuType)
	{
		*ppAdapter = nullptr;

		ComPtr<IDXGIAdapter1> adapter;

		ComPtr<IDXGIFactory6> factory6;
		if (SUCCEEDED(pFactory->QueryInterface(IID_PPV_ARGS(&factory6))))
		{
			for (UINT adapterIndex = 0; SUCCEEDED(factory6->EnumAdapterByGpuPreference(adapterIndex, gpuType == PreferredGPUType::Discrete ? DXGI_GPU_PREFERENCE_HIGH_PERFORMANCE : DXGI_GPU_PREFERENCE_UNSPECIFIED, IID_PPV_ARGS(&adapter))); ++adapterIndex)
			{
				DXGI_ADAPTER_DESC1 desc;
				adapter->GetDesc1(&desc);

				if (desc.Flags & DXGI_ADAPTER_FLAG_SOFTWARE)
					continue;

				// Check to see whether the adapter supports Direct3D 12, but don't create the
				// actual device yet.
				if (SUCCEEDED(D3D12CreateDevice(adapter.Get(), D3D_FEATURE_LEVEL_11_0, _uuidof(ID3D12Device), nullptr)))
				{
					char* buf = FileSystem::WCharToChar(desc.Description);
					LINA_TRACE("[D3D12 Backend] -> Selected hardware adapter: {0}, Dedicated Video Memory: {1}", buf, desc.DedicatedVideoMemory * 0.000001);
					delete[] buf;
					break;
				}
			}
		}

		if (adapter.Get() == nullptr)
		{
			for (UINT adapterIndex = 0; SUCCEEDED(pFactory->EnumAdapters1(adapterIndex, &adapter)); ++adapterIndex)
			{
				DXGI_ADAPTER_DESC1 desc;
				adapter->GetDesc1(&desc);

				if (desc.Flags & DXGI_ADAPTER_FLAG_SOFTWARE)
					continue;

				// Check to see whether the adapter supports Direct3D 12, but don't create the
				// actual device yet.
				if (SUCCEEDED(D3D12CreateDevice(adapter.Get(), D3D_FEATURE_LEVEL_11_0, _uuidof(ID3D12Device), nullptr)))
				{
					char* buf = FileSystem::WCharToChar(desc.Description);
					LINA_TRACE("[D3D12 Backend] -> Selected hardware adapter: {0}, Dedicated Video Memory: {1} MB", buf, desc.DedicatedVideoMemory * 0.000001);
					delete[] buf;
					break;
				}
			}
		}

		*ppAdapter = adapter.Detach();
	}

	void Renderer::DX12Exception(HrException e)
	{
		if (e.Error() == DXGI_ERROR_DEVICE_REMOVED || e.Error() == DXGI_ERROR_DEVICE_RESET)
		{
			HRESULT reason = m_device->GetDeviceRemovedReason();

#ifndef LINA_PRODUCTION_BUILD
			wchar_t outString[100];
			size_t	size = 100;
			swprintf_s(outString, size, L"Device removed! DXGI_ERROR code: 0x%X\n", reason);
			char* reas = FileSystem::WCharToChar(outString);
			LINA_CRITICAL("[Renderer] -> Device removal reason: {0}", reas);
			delete[] reas;
#endif

			try
			{
				Join();
			}
			catch (HrException&)
			{
				LINA_CRITICAL("[Renderer] -> Can't even wait for GPU!");
			}

			m_failed = true;
		}
		else
		{
		}
	}

	// ********************** RESOURCES ********************************
	// ********************** RESOURCES ********************************
	// ********************** RESOURCES ********************************

	LPCWSTR macros[5] = {{L"LINA_PASS_OPAQUE="}, {L"LINA_PASS_SHADOWS="}, {L"LINA_PIPELINE_STANDARD="}, {L"LINA_PIPELINE_NOVERTEX="}, {L"LINA_PIPELINE_GUI="}};

	void Renderer::GenerateMaterial(Material* mat)
	{
		for (int i = 0; i < FRAMES_IN_FLIGHT; i++)
		{
			mat->SetIsDirty(i, true);

			if (mat->GetGfxResource(i) == nullptr)
			{
				// Material doesn't exist already

#ifndef LINA_PRODUCTION_BUILD
				const wchar_t* name = FileSystem::CharToWChar(mat->GetPath().c_str());
				mat->SetGfxResource(i, CreateCPUResource(mat->GetTotalAlignedSize(), CPUResourceHint::ConstantBuffer, name));
				delete[] name;
#else
				mat->SetGfxResource(i, CreateCPUResource(sz == 0 ? 16 : sz, CPUResourceHint::ConstantBuffer, L"Material Buffer"));
#endif
				mat->SetDescriptor(i, m_bufferHeap->GetNewHeapHandle());
			}

			D3D12_CONSTANT_BUFFER_VIEW_DESC desc = {};
			desc.BufferLocation					 = mat->GetGfxResource(i)->GetGPUPointer();
			desc.SizeInBytes					 = static_cast<UINT>(mat->GetTotalAlignedSize());
			m_device->CreateConstantBufferView(&desc, {mat->GetDescriptor(i).GetCPUHandle()});
		}
	}

	void Renderer::DestroyMaterial(Material* mat)
	{
		for (int i = 0; i < FRAMES_IN_FLIGHT; i++)
		{
			DeleteCPUResource(mat->GetGfxResource(i));
			m_bufferHeap->FreeHeapHandle(mat->GetDescriptor(i));
		}
	}

	void Renderer::GeneratePipeline(Shader* shader)
	{
		shader->SetPipeline(new DX12Pipeline(this, shader));
	}

	void Renderer::DestroyPipeline(Shader* shader)
	{
		delete shader->GetPipeline();
	}

	void Renderer::CompileShader(const char* path, const HashMap<ShaderStage, String>& stages, HashMap<ShaderStage, ShaderByteCode>& outCompiledCode)
	{
		try
		{
			Microsoft::WRL::ComPtr<IDxcCompiler3> idxcCompiler;

			HRESULT hr = DxcCreateInstance(CLSID_DxcCompiler, IID_PPV_ARGS(&idxcCompiler));
			if (FAILED(hr))
			{
				LINA_CRITICAL("[Shader Compile] -> Failed to create DXC compiler");
			}

			ComPtr<IDxcUtils> utils;
			hr = DxcCreateInstance(CLSID_DxcUtils, IID_PPV_ARGS(utils.GetAddressOf()));

			if (FAILED(hr))
			{
				LINA_CRITICAL("[Shader Compile] -> Failed to create DXC utils");
			}

			wchar_t* pathw = FileSystem::CharToWChar(path);

			// IDxcCompilerArgs* pCompilerArgs;
			// DxcCreateInstance(CLSID_DxcCompilerArgs, IID_PPV_ARGS(&pCompilerArgs));
			// const wchar_t* arg = L"/O3";
			// pCompilerArgs->AddArguments(&arg, 1);

			for (auto& [stage, title] : stages)
			{
				UINT32					 codePage = CP_UTF8;
				ComPtr<IDxcBlobEncoding> sourceBlob;
				ThrowIfFailed(m_idxcLib->CreateBlobFromFile(pathw, &codePage, &sourceBlob));

				wchar_t* entry = FileSystem::CharToWChar(title.c_str());

				const wchar_t* targetProfile = L"vs_6_0";
				if (stage == ShaderStage::Fragment)
					targetProfile = L"ps_6_0";
				else if (stage == ShaderStage::Compute)
					targetProfile = L"cs_6_0";
				else if (stage == ShaderStage::Geometry)
					targetProfile = L"gs_6_0";
				else if (stage == ShaderStage::TesellationControl || stage == ShaderStage::TesellationEval)
					targetProfile = L"hs_6_0";

				DxcBuffer sourceBuffer;
				sourceBuffer.Ptr	  = sourceBlob->GetBufferPointer();
				sourceBuffer.Size	  = sourceBlob->GetBufferSize();
				sourceBuffer.Encoding = 0;

				Vector<LPCWSTR> arguments;

				arguments.push_back(L"-E");
				arguments.push_back(entry);
				arguments.push_back(L"-T");
				arguments.push_back(targetProfile);
				arguments.push_back(DXC_ARG_WARNINGS_ARE_ERRORS); //-WX
				arguments.push_back(L"-HV 2021");				  //-WX

				// We will still get reflection info, just with a smaller binary.
				arguments.push_back(L"-Qstrip_debug");
				arguments.push_back(L"-Qstrip_reflect");

#ifndef LINA_PRODUCTION_BUILD
				arguments.push_back(DXC_ARG_DEBUG); //-Zi
#else
				arguments.push_back(DXC_ARG_OPTIMIZATION_LEVEL3);
#endif

				for (int i = 0; i < 5; i++)
				{
					arguments.push_back(L"-D");
					arguments.push_back(macros[i]);
				}

				ComPtr<IDxcResult> result;
				ThrowIfFailed(idxcCompiler->Compile(&sourceBuffer, arguments.data(), static_cast<uint32>(arguments.size()), &m_includeInterface, IID_PPV_ARGS(result.GetAddressOf())));

				ComPtr<IDxcBlobUtf8> errors;
				ComPtr<IDxcBlobWide> outputName;
				result->GetOutput(DXC_OUT_ERRORS, IID_PPV_ARGS(errors.GetAddressOf()), nullptr);

				if (errors && errors->GetStringLength() > 0)
				{
					LINA_ERR("[Shader Compile Error] -> {0} {1}", path, (char*)errors->GetStringPointer());
				}

				// ComPtr<IDxcBlob> reflectionData;
				// result->GetOutput(DXC_OUT_REFLECTION, IID_PPV_ARGS(reflectionData.GetAddressOf()), nullptr);
				// DxcBuffer reflectionBuffer;
				// reflectionBuffer.Ptr	  = reflectionData->GetBufferPointer();
				// reflectionBuffer.Size	  = reflectionData->GetBufferSize();
				// reflectionBuffer.Encoding = 0;
				// ComPtr<ID3D12ShaderReflection> shaderReflection;
				// utils->CreateReflection(&reflectionBuffer, IID_PPV_ARGS(shaderReflection.GetAddressOf()));

				result->GetStatus(&hr);

				if (FAILED(hr))
				{
					if (result)
					{
						ComPtr<IDxcBlobEncoding> errorsBlob;
						hr = result->GetErrorBuffer(&errorsBlob);
						if (SUCCEEDED(hr) && errorsBlob)
						{
							LINA_ERR("[D3D12GpUStorage]-> Shader compilation failed: {0} {1}", path, (const char*)errorsBlob->GetBufferPointer());
						}
					}
				}

				ComPtr<IDxcBlob> code;
				result->GetResult(&code);

				const SIZE_T sz = code->GetBufferSize();
				auto&		 bc = outCompiledCode[stage];
				bc.dataSize		= static_cast<uint32>(sz);
				bc.data			= new uint8[sz];
				MEMCPY(bc.data, code->GetBufferPointer(), sz);

				delete[] entry;
			}

			delete[] pathw;
		}
		catch (HrException e)
		{
			LINA_CRITICAL("[Renderer] -> Exception when compiling shader! {0} {1}", path, e.what());
			DX12Exception(e);
		}
	}

	void Renderer::GenerateImage(Texture* txt, ImageGenerateRequest req)
	{
		const TextureResourceType resourceType = txt->GetResourceType();
		const bool				  existing	   = txt->GetGfxResource() != nullptr;

		if (existing && resourceType != TextureResourceType::Texture2DDefaultDynamic)
		{
			LINA_ERR("[Renderer] -> Re-generating images for textures is only supported for those textures of type Texture2DDefaultDynamic!");
			return;
		}

		auto&		meta = txt->GetMetadata();
		const auto& ext	 = txt->GetExtent();

		// Render target types will be implemented via CreateRenderTargetXXX functions.
		if (resourceType == TextureResourceType::Texture2DDefault || resourceType == TextureResourceType::Texture2DDefaultDynamic)
		{
			if (!existing)
				txt->SetGfxResource(CreateTextureResource(TextureResourceType::Texture2DDefault, txt));

			DX12ResourceTexture* txtResGPU = static_cast<DX12ResourceTexture*>(txt->GetGfxResource());
			auto				 format	   = static_cast<Format>(meta.GetUInt8("Format"_hs));

			m_uploadContext->CopyTextureQueueUp(txtResGPU, txt, req);

			if (!existing)
			{
				txt->SetDescriptor(m_textureHeap->GetNewHeapHandle());

				D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
				srvDesc.Shader4ComponentMapping			= D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
				srvDesc.Format							= GetFormat(format);
				srvDesc.ViewDimension					= D3D12_SRV_DIMENSION_TEXTURE2D;
				srvDesc.Texture2D.MipLevels				= txt->GetMipLevels();
				m_device->CreateShaderResourceView(txtResGPU->DX12GetAllocation()->GetResource(), &srvDesc, {txt->GetDescriptor().GetCPUHandle()});
			}
		}
	}

	void Renderer::DestroyImage(Texture* txt)
	{
		const TextureResourceType resourceType = txt->GetResourceType();

		if (resourceType == TextureResourceType::Texture2DSwapchain)
		{
			m_rtvHeap->FreeHeapHandle(txt->GetDescriptor());
		}
		else if (resourceType == TextureResourceType::Texture2DRenderTargetDepthStencil)
		{
			m_dsvHeap->FreeHeapHandle(txt->GetDescriptor());
		}
		else if (resourceType == TextureResourceType::Texture2DDefault || resourceType == TextureResourceType::Texture2DDefaultDynamic)
		{
			m_textureHeap->FreeHeapHandle(txt->GetDescriptor());
		}
		else if (resourceType == TextureResourceType::Texture2DRenderTargetColor)
		{
			m_rtvHeap->FreeHeapHandle(txt->GetDescriptor());
			m_textureHeap->FreeHeapHandle(txt->GetDescriptorSecondary());
		}

		if (txt->GetGfxResource())
			delete txt->GetGfxResource();
	}

	void Renderer::GenerateSampler(TextureSampler* sampler)
	{
		const auto& samplerData = sampler->GetSamplerData();

		D3D12_SAMPLER_DESC desc;

		desc.AddressU		= GetAddressMode(samplerData.mode);
		desc.AddressV		= GetAddressMode(samplerData.mode);
		desc.AddressW		= GetAddressMode(samplerData.mode);
		desc.BorderColor[0] = samplerData.borderColor.x;
		desc.BorderColor[1] = samplerData.borderColor.y;
		desc.BorderColor[2] = samplerData.borderColor.z;
		desc.BorderColor[3] = samplerData.borderColor.w;
		desc.ComparisonFunc = D3D12_COMPARISON_FUNC_NONE;
		desc.Filter			= GetFilter(samplerData.minFilter, samplerData.magFilter);
		desc.MinLOD			= samplerData.minLod;
		desc.MaxLOD			= samplerData.maxLod;
		desc.MaxAnisotropy	= samplerData.anisotropy;
		desc.MipLODBias		= static_cast<FLOAT>(samplerData.mipLodBias);
		sampler->SetDescriptor(m_samplerHeap->GetNewHeapHandle());
		m_device->CreateSampler(&desc, {sampler->GetDescriptor().GetCPUHandle()});
	}

	void Renderer::DestroySampler(TextureSampler* sampler)
	{
		m_samplerHeap->FreeHeapHandle(sampler->GetDescriptor());
	}

	ISwapchain* Renderer::CreateSwapchain(const Vector2i& size, IWindow* window, StringID sid)
	{
		return new DX12Swapchain(this, size, window, sid);
	}

	uint32 Renderer::CreateFence()
	{
		const uint32 handle = m_fences.AddItem(ComPtr<ID3D12Fence>());
		auto&		 fence	= m_fences.GetItemR(handle);

		try
		{
			ThrowIfFailed(m_device->CreateFence(0, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(&fence)));
		}
		catch (HrException e)
		{
			LINA_CRITICAL("[Renderer] -> Exception when creating a fence! {0}", e.what());
			DX12Exception(e);
		}

		return handle;
	}
	IGfxResourceCPU* Renderer::CreateCPUResource(size_t size, CPUResourceHint hint, const wchar_t* name)
	{
		return new DX12ResourceCPU(this, hint, size, name);
	}

	IGfxResourceGPU* Renderer::CreateGPUResource(size_t size, GPUResourceType type, bool requireJoinBeforeUpdating, const wchar_t* name)
	{
		return new DX12ResourceGPU(this, type, requireJoinBeforeUpdating, size, name);
	}

	IGfxResourceTexture* Renderer::CreateTextureResource(TextureResourceType type, Texture* texture)
	{
		return new DX12ResourceTexture(this, texture, type);
	}

	void Renderer::DeleteCPUResource(IGfxResourceCPU* res)
	{
		delete res;
	}

	void Renderer::DeleteGPUResource(IGfxResourceGPU* res)
	{
		delete res;
	}

	IUploadContext* Renderer::CreateUploadContext()
	{
		return new DX12UploadContext(this);
	}

	uint32 Renderer::GetNextBackBuffer(ISwapchain* swp)
	{
		DX12Swapchain* dx12Swap = static_cast<DX12Swapchain*>(swp);
		return static_cast<uint32>(dx12Swap->GetPtr()->GetCurrentBackBufferIndex());
	}

	void Renderer::ReleaseFence(uint32 handle)
	{
		m_fences.GetItemR(handle).Reset();
		m_fences.RemoveItem(handle);
	}

	Texture* Renderer::CreateRenderTargetColor(const String& path, const Vector2i& size)
	{
		// Generate image data.
		const StringID sid = TO_SID(path);
		Extent3D	   ext = Extent3D{
				  .width  = static_cast<uint32>(size.x),
				  .height = static_cast<uint32>(size.y),
				  .depth  = 1,
		  };

		UserGeneratedTextureData textureData = UserGeneratedTextureData{
			.resourceType  = TextureResourceType::Texture2DRenderTargetColor,
			.path		   = path,
			.sid		   = sid,
			.extent		   = ext,
			.format		   = DEFAULT_RT_FORMAT,
			.targetSampler = DEFAULT_SAMPLER_SID,
			.tiling		   = ImageTiling::Optimal,
		};

		Texture* rt = new Texture(m_resourceManager, textureData);
		GenerateImage(rt, {});

		rt->SetGfxResource(CreateTextureResource(TextureResourceType::Texture2DRenderTargetColor, rt));
		rt->SetDescriptor(m_rtvHeap->GetNewHeapHandle());
		rt->SetDescriptorSecondary(m_textureHeap->GetNewHeapHandle());
		DX12ResourceTexture* txtResGPU = static_cast<DX12ResourceTexture*>(rt->GetGfxResource());

		D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
		srvDesc.Shader4ComponentMapping			= D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
		srvDesc.Format							= GetFormat(DEFAULT_RT_FORMAT);
		srvDesc.ViewDimension					= D3D12_SRV_DIMENSION_TEXTURE2D;
		srvDesc.Texture2D.MipLevels				= 1;
		m_device->CreateShaderResourceView(txtResGPU->DX12GetAllocation()->GetResource(), &srvDesc, {rt->GetDescriptorSecondary().GetCPUHandle()});

		// Create view
		D3D12_RENDER_TARGET_VIEW_DESC desc = {};
		desc.Format						   = GetFormat(DEFAULT_RT_FORMAT);
		desc.ViewDimension				   = D3D12_RTV_DIMENSION_TEXTURE2D;
		desc.Texture2D.MipSlice			   = 0;

		m_device->CreateRenderTargetView(txtResGPU->DX12GetAllocation()->GetResource(), &desc, {rt->GetDescriptor().GetCPUHandle()});
		return rt;
	}

	Texture* Renderer::CreateRenderTargetSwapchain(ISwapchain* swp, uint32 bufferIndex, const String& path)
	{
		// Generate image data.
		const StringID sid = TO_SID(path);

		UserGeneratedTextureData data = UserGeneratedTextureData{
			.resourceType				   = TextureResourceType::Texture2DSwapchain,
			.path						   = path,
			.sid						   = sid,
			.createPixelBuffer			   = false,
			.destroyPixelBufferAfterUpload = false,
		};

		Texture* rt = new Texture(m_resourceManager, data);
		GenerateImage(rt, {});

		// alloc descriptor
		rt->SetDescriptor(m_rtvHeap->GetNewHeapHandle());

		// Copy swapchain res into descriptor
		DX12Swapchain*		 dx12Swap = static_cast<DX12Swapchain*>(swp);
		DX12ResourceTexture* textRes  = new DX12ResourceTexture();
		rt->SetGfxResource(textRes);

		try
		{
			ThrowIfFailed(dx12Swap->GetPtr()->GetBuffer(bufferIndex, IID_PPV_ARGS(&textRes->m_rawResource)));
		}
		catch (HrException e)
		{
			LINA_CRITICAL("[Renderer] -> Exception while creating render target for swapchain! {0}", e.what());
			DX12Exception(e);
		}

		D3D12_RENDER_TARGET_VIEW_DESC desc = {};
		desc.Format						   = GetFormat(DEFAULT_RT_FORMAT);
		desc.ViewDimension				   = D3D12_RTV_DIMENSION_TEXTURE2D;

		m_device->CreateRenderTargetView(textRes->m_rawResource.Get(), &desc, {rt->GetDescriptor().GetCPUHandle()});
		rt->SetGfxResource(textRes);

		return rt;
	}

	Texture* Renderer::CreateRenderTargetDepthStencil(const String& path, const Vector2i& size)
	{
		// Generate image data.
		const StringID sid = TO_SID(path);
		Extent3D	   ext = Extent3D{
				  .width  = static_cast<uint32>(size.x),
				  .height = static_cast<uint32>(size.y),
				  .depth  = 1,
		  };

		UserGeneratedTextureData textureData = UserGeneratedTextureData{
			.resourceType  = TextureResourceType::Texture2DRenderTargetDepthStencil,
			.path		   = path,
			.sid		   = sid,
			.extent		   = ext,
			.format		   = DEFAULT_DEPTH_FORMAT,
			.targetSampler = DEFAULT_SAMPLER_SID,
			.tiling		   = ImageTiling::Optimal,
		};

		Texture* rt = new Texture(m_resourceManager, textureData);
		GenerateImage(rt, {});

		// Alloc descriptor & resource
		rt->SetDescriptor(m_dsvHeap->GetNewHeapHandle());
		rt->SetGfxResource(CreateTextureResource(TextureResourceType::Texture2DRenderTargetDepthStencil, rt));

		// Create view
		DX12ResourceTexture*		  res			   = static_cast<DX12ResourceTexture*>(rt->GetGfxResource());
		D3D12_DEPTH_STENCIL_VIEW_DESC depthStencilDesc = {};
		depthStencilDesc.Format						   = GetFormat(DEFAULT_DEPTH_FORMAT);
		depthStencilDesc.ViewDimension				   = D3D12_DSV_DIMENSION_TEXTURE2D;
		depthStencilDesc.Flags						   = D3D12_DSV_FLAG_NONE;
		m_device->CreateDepthStencilView(res->DX12GetAllocation()->GetResource(), &depthStencilDesc, {rt->GetDescriptor().GetCPUHandle()});

		return rt;
	}

} // namespace Lina

LINA_RESTORE_VC_WARNING()
