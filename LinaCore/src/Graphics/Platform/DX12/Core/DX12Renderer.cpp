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

	Renderer::Renderer(const SystemInitializationInfo& initInfo, GfxManager* gfxMan)
		: m_cmdAllocators(20, Microsoft::WRL::ComPtr<ID3D12CommandAllocator>()), m_cmdLists(50, Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList4>()), m_fences(10, Microsoft::WRL::ComPtr<ID3D12Fence>())
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
						//	debugController->EnableDebugLayer();

						//  Enable additional debug layers.
						// dxgiFactoryFlags |= DXGI_CREATE_FACTORY_DEBUG;
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
				D3D12_COMMAND_QUEUE_DESC queueDesc = {};
				queueDesc.Flags					   = D3D12_COMMAND_QUEUE_FLAG_NONE;
				queueDesc.Type					   = D3D12_COMMAND_LIST_TYPE_DIRECT;
				ThrowIfFailed(m_device->CreateCommandQueue(&queueDesc, IID_PPV_ARGS(&m_graphicsQueue)));

				queueDesc.Type = D3D12_COMMAND_LIST_TYPE_COPY;
				ThrowIfFailed(m_device->CreateCommandQueue(&queueDesc, IID_PPV_ARGS(&m_copyQueue)));

				NAME_DX12_OBJECT(m_graphicsQueue, L"Graphics Queue");
				NAME_DX12_OBJECT(m_copyQueue, L"Copy Queue");
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
				m_fenceValueGraphics = 1;

				m_fenceEventGraphics = CreateEvent(nullptr, FALSE, FALSE, nullptr);
				if (m_fenceEventGraphics == nullptr)
				{
					ThrowIfFailed(HRESULT_FROM_WIN32(GetLastError()));
				}
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

		ID3D12InfoQueue1* infoQueue = nullptr;
		if (SUCCEEDED(m_device->QueryInterface<ID3D12InfoQueue1>(&infoQueue)))
		{
			infoQueue->UnregisterMessageCallback(msgCallback);
		}

		m_graphicsQueue.Reset();
		m_copyQueue.Reset();
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
		WaitForFences(m_frameFenceGraphics, m_frames[frameIndex].storedFenceGraphics);
		m_gpuBufferHeap[m_currentFrameIndex]->Reset(m_texturesHeapAllocCount);
		m_gpuSamplerHeap[m_currentFrameIndex]->Reset(m_samplersHeapAllocCount);
	}

	void Renderer::EndFrame(uint32 frameIndex)
	{
		auto& fence = m_fences.GetItemR(m_frameFenceGraphics);
		auto& frame = m_frames[frameIndex];

		m_fenceValueGraphics++;
		frame.storedFenceGraphics = m_fenceValueGraphics;
		m_graphicsQueue->Signal(fence.Get(), m_fenceValueGraphics);
	}

	void Renderer::Join()
	{
		for (int i = 0; i < FRAMES_IN_FLIGHT; i++)
			WaitForFences(m_frameFenceGraphics, m_frames[i].storedFenceGraphics);
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
						auto* texture				= allLoadedTextures[i];
						texture->m_gpuBindlessIndex = i;

						D3D12_CPU_DESCRIPTOR_HANDLE handle;
						handle.ptr		   = alloc.GetCPUHandle() + i * heapIncrement;
						destDescriptors[i] = handle;

						srcDescriptors[i] = {texture->m_descriptor.GetCPUHandle()};
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
						auto& loadedSampler				  = allLoadedSamplers[i];
						loadedSampler->m_gpuBindlessIndex = i;

						D3D12_CPU_DESCRIPTOR_HANDLE handle;
						handle.ptr		   = alloc.GetCPUHandle() + i * increment;
						destDescriptors[i] = handle;

						srcDescriptors[i] = {loadedSampler->m_descriptor.GetCPUHandle()};
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
			mat->m_isDirty[i] = true;

			if (mat->m_buffer[i] == nullptr)
			{
				// Material doesn't exist already

#ifndef LINA_PRODUCTION_BUILD
				const wchar_t* name = FileSystem::CharToWChar(mat->GetPath().c_str());
				mat->m_buffer[i]	= CreateCPUResource(mat->GetTotalAlignedSize(), CPUResourceHint::ConstantBuffer, name);
				delete[] name;
#else
				mat->m_buffer[i] = CreateCPUResource(sz == 0 ? 16 : sz, CPUResourceHint::ConstantBuffer, L"Material Buffer");
#endif
				mat->m_descriptor[i] = m_bufferHeap->GetNewHeapHandle();
			}

			D3D12_CONSTANT_BUFFER_VIEW_DESC desc = {};
			desc.BufferLocation					 = mat->m_buffer[i]->GetGPUPointer();
			desc.SizeInBytes					 = static_cast<UINT>(mat->GetTotalAlignedSize());
			m_device->CreateConstantBufferView(&desc, {mat->m_descriptor[i].GetCPUHandle()});
		}
	}

	void Renderer::DestroyMaterial(Material* mat)
	{
		for (int i = 0; i < FRAMES_IN_FLIGHT; i++)
		{
			DeleteCPUResource(mat->m_buffer[i]);
			m_bufferHeap->FreeHeapHandle(mat->m_descriptor[i]);
		}
	}

	void Renderer::GeneratePipeline(Shader* shader)
	{
		shader->m_pipeline = new DX12Pipeline(this, shader);
	}

	void Renderer::DestroyPipeline(Shader* shader)
	{
		delete shader->m_pipeline;
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
		const bool				  existing	   = txt->m_gpuResource != nullptr;

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
				txt->m_gpuResource = CreateTextureResource(TextureResourceType::Texture2DDefault, txt);

			DX12ResourceTexture* txtResGPU = static_cast<DX12ResourceTexture*>(txt->m_gpuResource);
			auto				 format	   = static_cast<Format>(meta.GetUInt8("Format"_hs));

			m_uploadContext->CopyTextureQueueUp(txtResGPU, txt, req);

			if (!existing)
			{
				txt->m_descriptor = m_textureHeap->GetNewHeapHandle();

				D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
				srvDesc.Shader4ComponentMapping			= D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
				srvDesc.Format							= GetFormat(format);
				srvDesc.ViewDimension					= D3D12_SRV_DIMENSION_TEXTURE2D;
				srvDesc.Texture2D.MipLevels				= txt->GetMipLevels();
				m_device->CreateShaderResourceView(txtResGPU->DX12GetAllocation()->GetResource(), &srvDesc, {txt->m_descriptor.GetCPUHandle()});
			}
		}
	}

	void Renderer::DestroyImage(Texture* txt)
	{
		const TextureResourceType resourceType = txt->GetResourceType();

		if (resourceType == TextureResourceType::Texture2DSwapchain)
		{
			m_rtvHeap->FreeHeapHandle(txt->m_descriptor);
		}
		else if (resourceType == TextureResourceType::Texture2DRenderTargetDepthStencil)
		{
			m_dsvHeap->FreeHeapHandle(txt->m_descriptor);
		}
		else if (resourceType == TextureResourceType::Texture2DDefault || resourceType == TextureResourceType::Texture2DDefaultDynamic)
		{
			m_textureHeap->FreeHeapHandle(txt->m_descriptor);
		}
		else if (resourceType == TextureResourceType::Texture2DRenderTargetColor)
		{
			m_rtvHeap->FreeHeapHandle(txt->m_descriptor);
			m_textureHeap->FreeHeapHandle(txt->m_descriptorSecondary);
		}

		if (txt->m_gpuResource)
			delete txt->m_gpuResource;
	}

	void Renderer::GenerateSampler(TextureSampler* sampler)
	{
		const auto& samplerData = sampler->GetSamplerData();

		D3D12_SAMPLER_DESC desc;

		desc.AddressU		  = GetAddressMode(samplerData.mode);
		desc.AddressV		  = GetAddressMode(samplerData.mode);
		desc.AddressW		  = GetAddressMode(samplerData.mode);
		desc.BorderColor[0]	  = samplerData.borderColor.x;
		desc.BorderColor[1]	  = samplerData.borderColor.y;
		desc.BorderColor[2]	  = samplerData.borderColor.z;
		desc.BorderColor[3]	  = samplerData.borderColor.w;
		desc.ComparisonFunc	  = D3D12_COMPARISON_FUNC_NONE;
		desc.Filter			  = GetFilter(samplerData.minFilter, samplerData.magFilter);
		desc.MinLOD			  = samplerData.minLod;
		desc.MaxLOD			  = samplerData.maxLod;
		desc.MaxAnisotropy	  = samplerData.anisotropy;
		desc.MipLODBias		  = static_cast<FLOAT>(samplerData.mipLodBias);
		sampler->m_descriptor = m_samplerHeap->GetNewHeapHandle();
		m_device->CreateSampler(&desc, {sampler->m_descriptor.GetCPUHandle()});
	}

	void Renderer::DestroySampler(TextureSampler* sampler)
	{
		m_samplerHeap->FreeHeapHandle(sampler->m_descriptor);
	}

	ISwapchain* Renderer::CreateSwapchain(const Vector2i& size, IWindow* window, StringID sid)
	{
		return new DX12Swapchain(this, size, window, sid);
	}

	uint32 Renderer::CreateCommandAllocator(CommandType type)
	{
		const uint32 handle = m_cmdAllocators.AddItem(ComPtr<ID3D12CommandAllocator>());
		auto&		 alloc	= m_cmdAllocators.GetItemR(handle);

		try
		{
			ThrowIfFailed(m_device->CreateCommandAllocator(GetCommandType(type), IID_PPV_ARGS(alloc.GetAddressOf())));
		}
		catch (HrException e)
		{
			LINA_CRITICAL("[Renderer] -> Exception when creating a command allocator! {0}", e.what());
			DX12Exception(e);
		}

		return handle;
	}

	uint32 Renderer::CreateCommandList(CommandType type, uint32 allocatorHandle)
	{
		const uint32 handle	   = m_cmdLists.AddItem(ComPtr<ID3D12GraphicsCommandList4>());
		auto&		 list	   = m_cmdLists.GetItemR(handle);
		auto&		 allocator = m_cmdAllocators.GetItemR(allocatorHandle);

		try
		{
			ThrowIfFailed(m_device->CreateCommandList(0, GetCommandType(type), allocator.Get(), nullptr, IID_PPV_ARGS(list.GetAddressOf())));
			ThrowIfFailed(list->Close());
		}
		catch (HrException e)
		{
			LINA_CRITICAL("[Renderer] -> Exception when creating a command list! {0}", e.what());
			DX12Exception(e);
		}

		return handle;
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

	void Renderer::ReleaseCommanAllocator(uint32 handle)
	{
		m_cmdAllocators.GetItemR(handle).Reset();
		m_cmdAllocators.RemoveItem(handle);
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

	void Renderer::ReleaseCommandList(uint32 handle)
	{
		m_cmdLists.GetItemR(handle).Reset();
		m_cmdLists.RemoveItem(handle);
	}

	void Renderer::ResetCommandList(uint32 cmdAllocatorHandle, uint32 cmdListHandle)
	{
		auto& cmdList	   = m_cmdLists.GetItemR(cmdListHandle);
		auto& cmdAllocator = m_cmdAllocators.GetItemR(cmdAllocatorHandle);

		try
		{
			ThrowIfFailed(cmdAllocator->Reset());
			ThrowIfFailed(cmdList->Reset(cmdAllocator.Get(), nullptr));
		}
		catch (HrException e)
		{
			LINA_CRITICAL("[Renderer] -> Exception when resetting a command list! {0}", e.what());
			DX12Exception(e);
		}
	}

	void Renderer::PrepareCommandList(uint32 cmdListHandle, const Viewport& viewport, const Recti& scissors)
	{
		auto&				  cmdList = m_cmdLists.GetItemR(cmdListHandle);
		ID3D12DescriptorHeap* heaps[] = {m_gpuBufferHeap[m_currentFrameIndex]->GetHeap(), m_gpuSamplerHeap[m_currentFrameIndex]->GetHeap()};
		cmdList->SetGraphicsRootSignature(m_rootSigStandard.Get());
		cmdList->SetDescriptorHeaps(_countof(heaps), heaps);

		// Viewport & scissors.
		{
			D3D12_VIEWPORT vp;
			vp.MinDepth = 0.0f;
			vp.MaxDepth = 1.0f;
			vp.Height	= viewport.height;
			vp.Width	= viewport.width;
			vp.TopLeftX = viewport.x;
			vp.TopLeftY = viewport.y;

			D3D12_RECT sc;
			sc.left	  = static_cast<LONG>(scissors.pos.x);
			sc.top	  = static_cast<LONG>(scissors.pos.y);
			sc.right  = static_cast<LONG>(scissors.size.x);
			sc.bottom = static_cast<LONG>(scissors.size.y);

			cmdList->RSSetViewports(1, &vp);
			cmdList->RSSetScissorRects(1, &sc);
		}

		// Textures & samplers
		{
			cmdList->SetGraphicsRootDescriptorTable(GBB_TxtData, m_gpuBufferHeap[m_currentFrameIndex]->GetHeapGPUStart());
			cmdList->SetGraphicsRootDescriptorTable(GBB_SamplerData, m_gpuSamplerHeap[m_currentFrameIndex]->GetHeapGPUStart());
			cmdList->SetGraphicsRootDescriptorTable(GBB_MatData, {m_gpuBufferHeap[m_currentFrameIndex]->GetOffsetedHandle(m_texturesHeapAllocCount).GetGPUHandle()});
		}
	}

	void Renderer::FinalizeCommandList(uint32 cmdListHandle)
	{
		auto& cmdList = m_cmdLists.GetItemR(cmdListHandle);

		try
		{
			ThrowIfFailed(cmdList->Close());
		}
		catch (HrException e)
		{
			LINA_CRITICAL("[Renderer] -> Exception when closing a command list! {0}", e.what());
			DX12Exception(e);
		}
	}

	void Renderer::ExecuteCommandListsGraphics(const Vector<uint32>& lists)
	{
		const UINT sz = static_cast<UINT>(lists.size());

		Vector<ID3D12CommandList*> _lists;

		for (UINT i = 0; i < sz; i++)
		{
			auto& lst = m_cmdLists.GetItemR(lists[i]);
			_lists.push_back(lst.Get());
		}

		ID3D12CommandList* const* data = _lists.data();
		m_graphicsQueue->ExecuteCommandLists(sz, data);
	}

	void Renderer::ExecuteCommandListsTransfer(const Vector<uint32>& lists)
	{
		const UINT sz = static_cast<UINT>(lists.size());

		Vector<ID3D12CommandList*> _lists;

		for (UINT i = 0; i < sz; i++)
		{
			auto& lst = m_cmdLists.GetItemR(lists[i]);
			_lists.push_back(lst.Get());
		}

		ID3D12CommandList* const* data = _lists.data();
		m_copyQueue->ExecuteCommandLists(sz, data);
	}

	void Renderer::ResourceBarrier(uint32 cmdListHandle, ResourceTransition* transitions, uint32 count)
	{
		auto&							 cmdList = m_cmdLists.GetItemR(cmdListHandle);
		Vector<CD3DX12_RESOURCE_BARRIER> barriers;

		for (uint32 i = 0; i < count; i++)
		{
			ResourceTransition&	 t	 = transitions[i];
			DX12ResourceTexture* res = static_cast<DX12ResourceTexture*>(t.texture->m_gpuResource);

			if (t.type == ResourceTransitionType::SRV2RT)
				barriers.push_back(CD3DX12_RESOURCE_BARRIER::Transition(res->DX12GetAllocation()->GetResource(), D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE, D3D12_RESOURCE_STATE_RENDER_TARGET));
			else if (t.type == ResourceTransitionType::RT2SRV)
				barriers.push_back(CD3DX12_RESOURCE_BARRIER::Transition(res->DX12GetAllocation()->GetResource(), D3D12_RESOURCE_STATE_RENDER_TARGET, D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE));
			else if (t.type == ResourceTransitionType::RT2Present)
				barriers.push_back(CD3DX12_RESOURCE_BARRIER::Transition(res->m_rawResource.Get(), D3D12_RESOURCE_STATE_RENDER_TARGET, D3D12_RESOURCE_STATE_PRESENT));
			else if (t.type == ResourceTransitionType::Present2RT)
				barriers.push_back(CD3DX12_RESOURCE_BARRIER::Transition(res->m_rawResource.Get(), D3D12_RESOURCE_STATE_PRESENT, D3D12_RESOURCE_STATE_RENDER_TARGET));
		}

		cmdList->ResourceBarrier(static_cast<UINT>(barriers.size()), barriers.data());
	}

	void Renderer::BeginRenderPass(uint32 cmdListHandle, Texture* colorTexture)
	{
		auto& cmdList = m_cmdLists.GetItemR(cmdListHandle);

		const float							 cc[]{DEFAULT_CLEAR_CLR.x, DEFAULT_CLEAR_CLR.y, DEFAULT_CLEAR_CLR.z, DEFAULT_CLEAR_CLR.w};
		CD3DX12_CLEAR_VALUE					 clearValue{GetFormat(DEFAULT_RT_FORMAT), cc};
		D3D12_RENDER_PASS_BEGINNING_ACCESS	 renderPassBeginningAccessClear{D3D12_RENDER_PASS_BEGINNING_ACCESS_TYPE_CLEAR, {clearValue}};
		D3D12_RENDER_PASS_ENDING_ACCESS		 renderPassEndingAccessPreserve{D3D12_RENDER_PASS_ENDING_ACCESS_TYPE_PRESERVE, {}};
		D3D12_RENDER_PASS_RENDER_TARGET_DESC renderPassRenderTargetDesc{colorTexture->m_descriptor.GetCPUHandle(), renderPassBeginningAccessClear, renderPassEndingAccessPreserve};

		cmdList->BeginRenderPass(1, &renderPassRenderTargetDesc, nullptr, D3D12_RENDER_PASS_FLAG_NONE);
	}

	void Renderer::BeginRenderPass(uint32 cmdListHandle, Texture* colorTexture, Texture* depthStencilTexture)
	{
		auto& cmdList = m_cmdLists.GetItemR(cmdListHandle);

		const float			cc[]{DEFAULT_CLEAR_CLR.x, DEFAULT_CLEAR_CLR.y, DEFAULT_CLEAR_CLR.z, DEFAULT_CLEAR_CLR.w};
		CD3DX12_CLEAR_VALUE clearValue{GetFormat(DEFAULT_RT_FORMAT), cc};
		CD3DX12_CLEAR_VALUE clearDepth{GetFormat(DEFAULT_DEPTH_FORMAT), 1.0f, 0};

		D3D12_RENDER_PASS_BEGINNING_ACCESS	 colorBegin{D3D12_RENDER_PASS_BEGINNING_ACCESS_TYPE_CLEAR, {clearValue}};
		D3D12_RENDER_PASS_ENDING_ACCESS		 colorEnd{D3D12_RENDER_PASS_ENDING_ACCESS_TYPE_PRESERVE, {}};
		D3D12_RENDER_PASS_RENDER_TARGET_DESC colorDesc{colorTexture->m_descriptor.GetCPUHandle(), colorBegin, colorEnd};

		D3D12_RENDER_PASS_BEGINNING_ACCESS	 depthBegin{D3D12_RENDER_PASS_BEGINNING_ACCESS_TYPE_CLEAR, {clearDepth}};
		D3D12_RENDER_PASS_ENDING_ACCESS		 depthEnd{D3D12_RENDER_PASS_ENDING_ACCESS_TYPE_PRESERVE, {}};
		D3D12_RENDER_PASS_DEPTH_STENCIL_DESC depthDesc{depthStencilTexture->m_descriptor.GetCPUHandle(), depthBegin, depthBegin, depthEnd, depthEnd};

		cmdList->BeginRenderPass(1, &colorDesc, &depthDesc, D3D12_RENDER_PASS_FLAG_NONE);
	}

	void Renderer::EndRenderPass(uint32 cmdListHandle)
	{
		auto& cmdList = m_cmdLists.GetItemR(cmdListHandle);
		cmdList->EndRenderPass();
	}

	void Renderer::BindPipeline(uint32 cmdListHandle, Shader* shader)
	{
		auto&		  cmdList = m_cmdLists.GetItemR(cmdListHandle);
		DX12Pipeline* pl	  = static_cast<DX12Pipeline*>(shader->m_pipeline);
		cmdList->SetPipelineState(pl->GetPipelineState());
	}

	void Renderer::BindMaterials(Material** materials, uint32 materialsSize)
	{
		LOCK_GUARD(test);

		auto		 heap					= m_gpuBufferHeap[m_currentFrameIndex];
		const uint32 currentDescriptorIndex = heap->GetCurrentDescriptorIndex() - m_texturesHeapAllocCount;
		const uint32 heapIncrement			= heap->GetDescriptorSize();
		auto alloc = heap->GetHeapHandleBlock(materialsSize);

		Vector<D3D12_CPU_DESCRIPTOR_HANDLE> srcDescriptors;
		Vector<D3D12_CPU_DESCRIPTOR_HANDLE> destDescriptors;
		srcDescriptors.resize(materialsSize, {});
		destDescriptors.resize(materialsSize, {});

		Taskflow tf;
		tf.for_each_index(0, static_cast<int>(materialsSize), 1, [&](int i) {
			Material* mat			= materials[i];
			mat->m_gpuBindlessIndex = currentDescriptorIndex + i;

			D3D12_CPU_DESCRIPTOR_HANDLE handle;
			handle.ptr		   = alloc.GetCPUHandle() + i * heapIncrement;
			destDescriptors[i] = handle;

			// Update if necessary
			{
				if (true || mat->IsDirty(m_currentFrameIndex))
				{
					uint8* ptr = nullptr;
					size_t sz  = 0;
					mat->GetPropertyBlob(ptr, sz);
					mat->m_buffer[m_currentFrameIndex]->BufferData(ptr, sz);
					delete[] ptr;
					mat->m_isDirty[m_currentFrameIndex] = false;
				}
			}
			srcDescriptors[i] = {mat->m_descriptor[m_currentFrameIndex].GetCPUHandle()};
		});

		m_gfxManager->GetSystem()->GetMainExecutor()->RunAndWait(tf);
		m_device->CopyDescriptors(materialsSize, destDescriptors.data(), NULL, materialsSize, srcDescriptors.data(), NULL, D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
	}

	void Renderer::SetMaterialID(uint32 cmdListHandle, uint32 id)
	{
		auto& cmdList = m_cmdLists.GetItemR(cmdListHandle);
		cmdList->SetGraphicsRoot32BitConstant(GBB_IndirectData, id, 1);
	}

	void Renderer::BindDynamicTextures(Texture** textures, uint32 texturesSize)
	{
		LOCK_GUARD(test);
		auto		 heap					= m_gpuBufferHeap[m_currentFrameIndex];
		const uint32 currentDescriptorIndex = heap->GetCurrentDescriptorIndex();
		const uint32 heapIncrement			= heap->GetDescriptorSize();
		auto		 alloc					= heap->GetHeapHandleBlock(texturesSize);

		Vector<D3D12_CPU_DESCRIPTOR_HANDLE> srcDescriptors;
		Vector<D3D12_CPU_DESCRIPTOR_HANDLE> destDescriptors;
		srcDescriptors.resize(texturesSize, {});
		destDescriptors.resize(texturesSize, {});

		Taskflow tf;
		tf.for_each_index(0, static_cast<int>(texturesSize), 1, [&](int i) {
			auto* texture = textures[i];

			texture->m_gpuBindlessIndex = currentDescriptorIndex + i;

			D3D12_CPU_DESCRIPTOR_HANDLE handle;
			handle.ptr		   = alloc.GetCPUHandle() + i * heapIncrement;
			destDescriptors[i] = handle;

			srcDescriptors[i] = {texture->GetResourceType() == TextureResourceType::Texture2DDefaultDynamic ? texture->m_descriptor.GetCPUHandle() : texture->m_descriptorSecondary.GetCPUHandle()};

			if (texture->GetResourceType() != TextureResourceType::Texture2DRenderTargetColor && texture->GetResourceType() != TextureResourceType::Texture2DDefaultDynamic)
			{
				LINA_ERR("[Renderer] -> You should only bind dynamic textures or color render targets via BindDynamicTextures()!");
			}
		});

		m_gfxManager->GetSystem()->GetMainExecutor()->RunAndWait(tf);
		m_device->CopyDescriptors(texturesSize, destDescriptors.data(), NULL, texturesSize, srcDescriptors.data(), NULL, D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
	}

	void Renderer::BindUniformBuffer(uint32 cmdListHandle, uint32 bufferIndex, IGfxResourceCPU* buf)
	{
		auto& cmdList = m_cmdLists.GetItemR(cmdListHandle);
		cmdList->SetGraphicsRootConstantBufferView(bufferIndex, buf->GetGPUPointer());
	}

	void Renderer::BindObjectBuffer(uint32 cmdListHandle, IGfxResourceGPU* res)
	{
		auto& cmdList = m_cmdLists.GetItemR(cmdListHandle);
		cmdList->SetGraphicsRootShaderResourceView(GBB_ObjData, res->GetGPUPointer());
	}

	void Renderer::BindVertexBuffer(uint32 cmdListHandle, IGfxResourceGPU* buffer, size_t vertexSize, uint32 slot, size_t maxSize)
	{
		auto&					 cmdList = m_cmdLists.GetItemR(cmdListHandle);
		D3D12_VERTEX_BUFFER_VIEW view;
		view.BufferLocation = buffer->GetGPUPointer();
		view.SizeInBytes	= maxSize == 0 ? static_cast<UINT>(buffer->GetSize()) : static_cast<UINT>(maxSize);
		view.StrideInBytes	= static_cast<UINT>(vertexSize);
		cmdList->IASetVertexBuffers(slot, 1, &view);
	}

	void Renderer::BindIndexBuffer(uint32 cmdListHandle, IGfxResourceGPU* buffer, size_t maxSize)
	{
		auto&					cmdList = m_cmdLists.GetItemR(cmdListHandle);
		D3D12_INDEX_BUFFER_VIEW view;
		view.BufferLocation = buffer->GetGPUPointer();
		view.SizeInBytes	= maxSize == 0 ? static_cast<UINT>(buffer->GetSize()) : static_cast<UINT>(maxSize);
		view.Format			= DXGI_FORMAT_R32_UINT;
		cmdList->IASetIndexBuffer(&view);
	}

	void Renderer::DrawInstanced(uint32 cmdListHandle, uint32 vertexCount, uint32 instanceCount, uint32 startVertex, uint32 startInstance)
	{
		auto& cmdList = m_cmdLists.GetItemR(cmdListHandle);
		cmdList->DrawInstanced(vertexCount, instanceCount, startVertex, startInstance);
	}

	void Renderer::DrawIndexedInstanced(uint32 cmdListHandle, uint32 indexCountPerInstance, uint32 instanceCount, uint32 startIndexLocation, uint32 baseVertexLocation, uint32 startInstanceLocation)
	{
		auto& cmdList = m_cmdLists.GetItemR(cmdListHandle);
		cmdList->DrawIndexedInstanced(indexCountPerInstance, instanceCount, startIndexLocation, baseVertexLocation, startInstanceLocation);
	}

	void Renderer::DrawIndexedIndirect(uint32 cmdListHandle, IGfxResourceCPU* indirectBuffer, uint32 count, uint64 indirectOffset)
	{
		auto&			 cmdList = m_cmdLists.GetItemR(cmdListHandle);
		DX12ResourceCPU* buf	 = static_cast<DX12ResourceCPU*>(indirectBuffer);
		cmdList->ExecuteIndirect(m_commandSigStandard.Get(), count, buf->DX12GetAllocation()->GetResource(), indirectOffset, nullptr, 0);
	}

	void Renderer::SetTopology(uint32 cmdListHandle, Topology topology)
	{
		auto& cmdList = m_cmdLists.GetItemR(cmdListHandle);
		cmdList->IASetPrimitiveTopology(GetTopology(topology));
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

	void Renderer::WaitForFences(uint32 fenceHandle, uint64 frameFenceValue)
	{
		auto&		 fence	   = m_fences.GetItemR(fenceHandle);
		const UINT64 lastFence = fence->GetCompletedValue();

		if (lastFence < frameFenceValue)
		{
			try
			{
				HANDLE eventHandle = CreateEvent(nullptr, FALSE, FALSE, nullptr);
				if (eventHandle == nullptr)
				{
					ThrowIfFailed(HRESULT_FROM_WIN32(GetLastError()));
				}
				else
				{
					ThrowIfFailed(fence->SetEventOnCompletion(frameFenceValue, eventHandle));
					WaitForSingleObject(eventHandle, INFINITE);
					CloseHandle(eventHandle);
				}
			}
			catch (HrException e)
			{
				LINA_CRITICAL("[Renderer] -> Exception when waiting for a fence! {0}", e.what());
			}
		}
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

		rt->m_gpuResource			   = CreateTextureResource(TextureResourceType::Texture2DRenderTargetColor, rt);
		rt->m_descriptor			   = m_rtvHeap->GetNewHeapHandle();
		rt->m_descriptorSecondary	   = m_textureHeap->GetNewHeapHandle();
		DX12ResourceTexture* txtResGPU = static_cast<DX12ResourceTexture*>(rt->m_gpuResource);

		D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
		srvDesc.Shader4ComponentMapping			= D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
		srvDesc.Format							= GetFormat(DEFAULT_RT_FORMAT);
		srvDesc.ViewDimension					= D3D12_SRV_DIMENSION_TEXTURE2D;
		srvDesc.Texture2D.MipLevels				= 1;
		m_device->CreateShaderResourceView(txtResGPU->DX12GetAllocation()->GetResource(), &srvDesc, {rt->m_descriptorSecondary.GetCPUHandle()});

		// Create view
		D3D12_RENDER_TARGET_VIEW_DESC desc = {};
		desc.Format						   = GetFormat(DEFAULT_RT_FORMAT);
		desc.ViewDimension				   = D3D12_RTV_DIMENSION_TEXTURE2D;
		desc.Texture2D.MipSlice			   = 0;

		m_device->CreateRenderTargetView(txtResGPU->DX12GetAllocation()->GetResource(), &desc, {rt->m_descriptor.GetCPUHandle()});
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
		rt->m_descriptor = m_rtvHeap->GetNewHeapHandle();

		// Copy swapchain res into descriptor
		DX12Swapchain*		 dx12Swap = static_cast<DX12Swapchain*>(swp);
		DX12ResourceTexture* textRes  = new DX12ResourceTexture();
		rt->m_gpuResource			  = textRes;

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

		m_device->CreateRenderTargetView(textRes->m_rawResource.Get(), &desc, {rt->m_descriptor.GetCPUHandle()});
		rt->m_gpuResource = textRes;

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
		rt->m_descriptor  = m_dsvHeap->GetNewHeapHandle();
		rt->m_gpuResource = CreateTextureResource(TextureResourceType::Texture2DRenderTargetDepthStencil, rt);

		// Create view
		DX12ResourceTexture*		  res			   = static_cast<DX12ResourceTexture*>(rt->m_gpuResource);
		D3D12_DEPTH_STENCIL_VIEW_DESC depthStencilDesc = {};
		depthStencilDesc.Format						   = GetFormat(DEFAULT_DEPTH_FORMAT);
		depthStencilDesc.ViewDimension				   = D3D12_DSV_DIMENSION_TEXTURE2D;
		depthStencilDesc.Flags						   = D3D12_DSV_FLAG_NONE;
		m_device->CreateDepthStencilView(res->DX12GetAllocation()->GetResource(), &depthStencilDesc, {rt->m_descriptor.GetCPUHandle()});

		return rt;
	}

} // namespace Lina

LINA_RESTORE_VC_WARNING()
