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
#include "Graphics/Platform/DX12/Core/DX12CPUResource.hpp"
#include "Graphics/Platform/DX12/Core/DX12GPUResource.hpp"
#include "Graphics/Platform/DX12/Core/DX12GfxTextureResource.hpp"
#include "Graphics/Platform/DX12/Core/DX12StagingHeap.hpp"
#include "Graphics/Platform/DX12/Core/DX12GPUHeap.hpp"
#include "Graphics/Platform/DX12/Core/DX12UploadContext.hpp"
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

#ifndef LINA_PRODUCTION_BUILD
#include "FileSystem/FileSystem.hpp"
#endif

using Microsoft::WRL::ComPtr;

namespace Lina
{
	DWORD msgCallback = 0;

	struct Test
	{
		uint32 totalPresentCount;
	};

	HashMap<uint32, Test> presents;
	uint32				  totalPresentCount = 0;

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

	void Renderer::PreInitialize(const SystemInitializationInfo& initInfo, GfxManager* gfxMan)
	{
		m_gfxManager	  = gfxMan;
		m_resourceManager = m_gfxManager->GetSystem()->CastSubsystem<ResourceManager>(SubsystemType::ResourceManager);
		m_resourceManager->AddListener(this);
		m_loadedTextures.reserve(200);
		m_loadedRTs.reserve(16);
		m_loadedSamplers.reserve(50);
		m_vsync = initInfo.vsyncMode;

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

						//  Enable additional debug layers.
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
				D3D12_COMMAND_QUEUE_DESC queueDesc = {};
				queueDesc.Flags					   = D3D12_COMMAND_QUEUE_FLAG_NONE;
				queueDesc.Type					   = D3D12_COMMAND_LIST_TYPE_DIRECT;
				ThrowIfFailed(m_device->CreateCommandQueue(&queueDesc, IID_PPV_ARGS(&m_graphicsQueue)));

				queueDesc.Type = D3D12_COMMAND_LIST_TYPE_COPY;
				ThrowIfFailed(m_device->CreateCommandQueue(&queueDesc, IID_PPV_ARGS(&m_copyQueue)));

				NAME_DX12_OBJECT(m_graphicsQueue, "Graphics Queue");
				NAME_DX12_OBJECT(m_copyQueue, "Copy Queue");
			}

			// Heaps
			{
				for (int i = 0; i < FRAMES_IN_FLIGHT; i++)
				{
					m_gpuBufferHeap[i]	= new DX12GPUHeap(this, D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV, 100);
					m_gpuSamplerHeap[i] = new DX12GPUHeap(this, D3D12_DESCRIPTOR_HEAP_TYPE_SAMPLER, 100);
					NAME_DX12_OBJECT(m_gpuBufferHeap[i]->GetHeap(), "Linear GPU Buffer Heap");
					NAME_DX12_OBJECT(m_gpuSamplerHeap[i]->GetHeap(), "Linear GPU Sampler Heap");
				}

				m_bufferHeap  = new DX12StagingHeap(this, D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV, 10);
				m_textureHeap = new DX12StagingHeap(this, D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV, 1024);
				m_dsvHeap	  = new DX12StagingHeap(this, D3D12_DESCRIPTOR_HEAP_TYPE_DSV, 10);
				m_rtvHeap	  = new DX12StagingHeap(this, D3D12_DESCRIPTOR_HEAP_TYPE_RTV, 10);
				m_samplerHeap = new DX12StagingHeap(this, D3D12_DESCRIPTOR_HEAP_TYPE_SAMPLER, 100);

				NAME_DX12_OBJECT(m_bufferHeap->GetHeap(), "Buffer Heap");
				NAME_DX12_OBJECT(m_textureHeap->GetHeap(), "Texture Heap");
				NAME_DX12_OBJECT(m_dsvHeap->GetHeap(), "DSV Heap");
				NAME_DX12_OBJECT(m_rtvHeap->GetHeap(), "RTV Heap");
				NAME_DX12_OBJECT(m_samplerHeap->GetHeap(), "Sampler Heap");
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
		}
		catch (HrException e)
		{
			LINA_CRITICAL("[Renderer] -> Exception when pre-initializating renderer! {0}", e.what());
		}
	}

	void Renderer::Initialize(const SystemInitializationInfo& initInfo)
	{
		try
		{
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

				CD3DX12_DESCRIPTOR_RANGE1 descRange[2] = {};

				// Textures & Samplers
				descRange[0].Init(D3D12_DESCRIPTOR_RANGE_TYPE_SRV, UINT_MAX, 0, 0, D3D12_DESCRIPTOR_RANGE_FLAG_DESCRIPTORS_VOLATILE);
				descRange[1].Init(D3D12_DESCRIPTOR_RANGE_TYPE_SAMPLER, UINT_MAX, 0, 0, D3D12_DESCRIPTOR_RANGE_FLAG_DESCRIPTORS_VOLATILE);

				CD3DX12_ROOT_PARAMETER1 rp[8] = {};

				// Global constant buffer
				rp[0].InitAsConstantBufferView(0, 0, D3D12_ROOT_DESCRIPTOR_FLAG_NONE, D3D12_SHADER_VISIBILITY_VERTEX);

				// Indirect constant
				rp[1].InitAsConstants(1, 1, 0, D3D12_SHADER_VISIBILITY_VERTEX);

				// Scene data
				rp[2].InitAsConstantBufferView(2, 0, D3D12_ROOT_DESCRIPTOR_FLAG_NONE, D3D12_SHADER_VISIBILITY_VERTEX);

				// View data
				rp[3].InitAsConstantBufferView(3, 0, D3D12_ROOT_DESCRIPTOR_FLAG_NONE, D3D12_SHADER_VISIBILITY_VERTEX);

				// Object data
				rp[4].InitAsShaderResourceView(0, 1, D3D12_ROOT_DESCRIPTOR_FLAG_DATA_VOLATILE, D3D12_SHADER_VISIBILITY_VERTEX);

				// Material Data
				rp[5].InitAsConstantBufferView(4, 0, D3D12_ROOT_DESCRIPTOR_FLAG_NONE, D3D12_SHADER_VISIBILITY_ALL);

				// Texture data
				rp[6].InitAsDescriptorTable(1, &descRange[0], D3D12_SHADER_VISIBILITY_ALL);

				// Sampler data
				rp[7].InitAsDescriptorTable(1, &descRange[1], D3D12_SHADER_VISIBILITY_ALL);

				// CD3DX12_STATIC_SAMPLER_DESC samplerDesc[1] = {};
				// samplerDesc[0].Init(0, D3D12_FILTER_ANISOTROPIC);

				CD3DX12_VERSIONED_ROOT_SIGNATURE_DESC rootSig(8, rp, 0, NULL, D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT);
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
				argumentDescs[0].Constant.Num32BitValuesToSet	  = 1;
				argumentDescs[1].Type							  = D3D12_INDIRECT_ARGUMENT_TYPE_DRAW_INDEXED;

				D3D12_COMMAND_SIGNATURE_DESC commandSignatureDesc = {};
				commandSignatureDesc.pArgumentDescs				  = argumentDescs;
				commandSignatureDesc.NumArgumentDescs			  = _countof(argumentDescs);
				commandSignatureDesc.ByteStride					  = sizeof(DrawIndexedIndirectCommand);

				auto sa = sizeof(D3D12_DRAW_INDEXED_ARGUMENTS);
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
			LINA_CRITICAL("[Renderer] -> Exception when initializating renderer! {0}", e.what());
		}
	}

	void Renderer::Shutdown()
	{
		m_resourceManager->RemoveListener(this);

		delete m_uploadContext;
		ReleaseFence(m_frameFenceGraphics);

		ID3D12InfoQueue1* infoQueue = nullptr;
		if (SUCCEEDED(m_device->QueryInterface<ID3D12InfoQueue1>(&infoQueue)))
		{
			infoQueue->UnregisterMessageCallback(msgCallback);
		}

		m_graphicsQueue.Reset();
		m_copyQueue.Reset();
		m_dx12Allocator->Release();
		m_device.Reset();

		for (int i = 0; i < FRAMES_IN_FLIGHT; i++)
		{
			m_gpuBufferHeap[i]->Reset();
			m_gpuSamplerHeap[i]->Reset();
			delete m_gpuBufferHeap[i];
			delete m_gpuSamplerHeap[i];
		}

		delete m_bufferHeap;
		delete m_textureHeap;
		delete m_dsvHeap;
		delete m_samplerHeap;
		delete m_rtvHeap;
	}

	uint64 lastStuff   = 0;
	double diff		   = 0;
	double lastAppTime = 0.0;

	void Renderer::WaitForPresentation(ISwapchain* swapchain)
	{
		DX12Swapchain* dx12Swap = static_cast<DX12Swapchain*>(swapchain);
		DWORD		   result	= WaitForSingleObjectEx(dx12Swap->DX12GetWaitHandle(), 1000, true);
		if (FAILED(result))
		{
			LINA_ERR("[Renderer] -> Waiting on swapchain failed!");
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
			totalPresentCount++;

			Test test;
			test.totalPresentCount = totalPresentCount;
			uint32 pid			   = 0;
			dx12Swap->GetPtr()->GetLastPresentCount(&pid);
			presents[pid] = test;
		}
		catch (HrException& e)
		{
			LINA_CRITICAL("[Renderer] -> Present engine error! {0}", e.what());

			if (e.Error() == DXGI_ERROR_DEVICE_REMOVED || e.Error() == DXGI_ERROR_DEVICE_RESET)
			{
				try
				{
					Join();
				}
				catch (HrException&)
				{
					LINA_CRITICAL("[Renderer] -> Can't even wait for GPU!");
				}

				// TODO: Device removal: re-create dx12 resources?
			}
			else
			{
			}

			return false;
		}

		return true;
	}

	void Renderer::BeginFrame(uint32 frameIndex)
	{
		m_currentFrameIndex = frameIndex;

		WaitForFences(m_frameFenceGraphics, m_frames[frameIndex].storedFenceGraphics);

		const uint32 txtSize	 = static_cast<uint32>(m_loadedTextures.size());
		const uint32 samplerSize = static_cast<uint32>(m_loadedSamplers.size());
		m_gpuBufferHeap[m_currentFrameIndex]->Reset(txtSize);
		m_gpuSamplerHeap[m_currentFrameIndex]->Reset(samplerSize);
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
					auto texturesHeap = m_gpuBufferHeap[i];

					const uint32 txtHeapIncrement = texturesHeap->GetDescriptorSize();
					const uint32 texturesSize	  = static_cast<uint32>(m_loadedTextures.size());
					auto		 alloc			  = texturesHeap->GetHeapHandleBlock(texturesSize);

					Vector<D3D12_CPU_DESCRIPTOR_HANDLE> srcDescriptors;
					Vector<D3D12_CPU_DESCRIPTOR_HANDLE> destDescriptors;
					srcDescriptors.resize(texturesSize, {});
					destDescriptors.resize(texturesSize, {});

					Taskflow tf;
					tf.for_each_index(0, static_cast<int>(texturesSize), 1, [&](int i) {
						auto& loadedTexture		  = m_loadedTextures[i];
						loadedTexture.shaderIndex = i;

						D3D12_CPU_DESCRIPTOR_HANDLE handle;
						handle.ptr		   = alloc.GetCPUHandle().ptr + i * txtHeapIncrement;
						destDescriptors[i] = handle;

						auto& txtGenData  = m_textures.GetItemR(loadedTexture.idListIndex);
						srcDescriptors[i] = txtGenData.descriptor.GetCPUHandle();
					});

					m_gfxManager->GetSystem()->GetMainExecutor()->RunAndWait(tf);
					m_device->CopyDescriptors(texturesSize, destDescriptors.data(), NULL, texturesSize, srcDescriptors.data(), NULL, D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
				}

				// Bind global samplers
				{
					auto samplersHeap = m_gpuSamplerHeap[i];

					const uint32 increment	  = samplersHeap->GetDescriptorSize();
					const uint32 samplersSize = static_cast<uint32>(m_loadedSamplers.size());
					auto		 alloc		  = samplersHeap->GetHeapHandleBlock(samplersSize);

					Vector<D3D12_CPU_DESCRIPTOR_HANDLE> srcDescriptors;
					Vector<D3D12_CPU_DESCRIPTOR_HANDLE> destDescriptors;
					srcDescriptors.resize(samplersSize, {});
					destDescriptors.resize(samplersSize, {});

					Taskflow tf;
					tf.for_each_index(0, static_cast<int>(samplersSize), 1, [&](int i) {
						auto& loadedSampler		  = m_loadedSamplers[i];
						loadedSampler.shaderIndex = i;

						D3D12_CPU_DESCRIPTOR_HANDLE handle;
						handle.ptr		   = alloc.GetCPUHandle().ptr + i * increment;
						destDescriptors[i] = handle;

						auto& samplerGenData = m_samplers.GetItemR(loadedSampler.idListIndex);
						srcDescriptors[i]	 = samplerGenData.descriptor.GetCPUHandle();
					});
					m_gfxManager->GetSystem()->GetMainExecutor()->RunAndWait(tf);

					m_device->CopyDescriptors(samplersSize, destDescriptors.data(), NULL, samplersSize, srcDescriptors.data(), NULL, D3D12_DESCRIPTOR_HEAP_TYPE_SAMPLER);
				}
			}
		}

		// Dirty all materials
		{
			auto&		 mats		   = m_materials.GetItems();
			const uint32 materialsSize = static_cast<uint32>(mats.size());
			Taskflow	 tf;
			tf.for_each_index(0, static_cast<int>(materialsSize), 1, [&](int i) {
				auto& material = mats[i];

				for (int j = 0; j < FRAMES_IN_FLIGHT; j++)
					material.dirty[j] = true;
			});
			m_gfxManager->GetSystem()->GetMainExecutor()->RunAndWait(tf);
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

	// ********************** RESOURCES ********************************
	// ********************** RESOURCES ********************************
	// ********************** RESOURCES ********************************

	LPCWSTR macros[6] = {{L"LINA_PASS_OPAQUE="}, {L"LINA_PASS_SHADOWS="}, {L"LINA_PIPELINE_STANDARD="}, {L"LINA_PIPELINE_NOVERTEX="}, {L"LINA_PIPELINE_GUI="}, {L"LINA_MATERIAL=cbuffer MaterialBuffer : register(b4)"}};

	int32 Renderer::GenerateMaterial(Material* mat)
	{
		LOCK_GUARD(m_materialMtx);
		const uint32	   existingHandle = mat->GetGPUHandle();
		const uint32	   index		  = existingHandle == -1 ? m_materials.AddItem(GeneratedMaterial()) : existingHandle;
		GeneratedMaterial& genData		  = m_materials.GetItemR(index);

		uint8* data = nullptr;
		size_t sz	= 0;
		mat->GetPropertyBlob(data, sz);

		for (int i = 0; i < FRAMES_IN_FLIGHT; i++)
		{
			if (genData.buffer[i] == nullptr)
			{
				genData.buffer[i] = CreateCPUResource(sz == 0 ? 16 : sz, CPUResourceHint::ConstantBuffer, L"Material Buffer");
				genData.buffer[i]->BufferData(data, sz);
			}
			else
				genData.buffer[i]->BufferData(data, sz);
		}

		delete[] data;
		return static_cast<int32>(index);
	}

	void Renderer::UpdateMaterialProperties(Material* mat)
	{
		GeneratedMaterial& genData = m_materials.GetItemR(mat->GetGPUHandle());
		for (int i = 0; i < FRAMES_IN_FLIGHT; i++)
			genData.dirty[i] = true;
	}

	void Renderer::DestroyMaterial(uint32 handle)
	{
		// Note: no need to mtx lock, this is called from the main thread.
		auto& genData = m_materials.GetItemR(handle);

		for (int i = 0; i < FRAMES_IN_FLIGHT; i++)
			DeleteCPUResource(genData.buffer[i]);

		m_materials.RemoveItem(handle);
	}

	int32 Renderer::GeneratePipeline(Shader* shader)
	{
		LOCK_GUARD(m_shaderMtx);

		const uint32	   index   = m_shaders.AddItem(GeneratedShader());
		auto&			   genData = m_shaders.GetItemR(index);
		const auto&		   stages  = shader->GetStages();
		const PipelineType ppType  = shader->GetPipelineType();

		Vector<D3D12_INPUT_ELEMENT_DESC> inputLayout;

		// Define the vertex input layout.
		{
			if (ppType == PipelineType::Standard)
			{
				inputLayout.push_back({"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0});
				inputLayout.push_back({"NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 12, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0});
				inputLayout.push_back({"COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 24, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0});
				inputLayout.push_back({"TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 40, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0});
			}
			else if (ppType == PipelineType::GUI)
			{
				inputLayout.push_back({"POSITION", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 0, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0});
				inputLayout.push_back({"TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 8, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0});
				inputLayout.push_back({"COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 16, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0});
			}
			else if (ppType == PipelineType::NoVertex)
			{
				// no input.
			}
		}

		// Describe and create the graphics pipeline state object (PSO).
		D3D12_GRAPHICS_PIPELINE_STATE_DESC psoDesc = {};

		if (!inputLayout.empty())
			psoDesc.InputLayout = {&inputLayout[0], static_cast<UINT>(inputLayout.size())};

		D3D12_BLEND_DESC blendDesc						= CD3DX12_BLEND_DESC(D3D12_DEFAULT);
		blendDesc.AlphaToCoverageEnable					= false;
		blendDesc.IndependentBlendEnable				= false;
		blendDesc.RenderTarget[0].BlendEnable			= true;
		blendDesc.RenderTarget[0].SrcBlend				= D3D12_BLEND_SRC_ALPHA;
		blendDesc.RenderTarget[0].DestBlend				= D3D12_BLEND_INV_SRC_ALPHA;
		blendDesc.RenderTarget[0].SrcBlendAlpha			= D3D12_BLEND_ONE;
		blendDesc.RenderTarget[0].DestBlendAlpha		= D3D12_BLEND_INV_SRC_ALPHA;
		blendDesc.RenderTarget[0].BlendOp				= D3D12_BLEND_OP_ADD;
		blendDesc.RenderTarget[0].BlendOpAlpha			= D3D12_BLEND_OP_ADD;
		blendDesc.RenderTarget[0].RenderTargetWriteMask = D3D12_COLOR_WRITE_ENABLE_ALL;
		blendDesc.RenderTarget[0].LogicOpEnable			= false;
		blendDesc.RenderTarget[0].LogicOp				= D3D12_LOGIC_OP_COPY;

		const D3D12_DEPTH_STENCILOP_DESC defaultStencilOp = {D3D12_STENCIL_OP_KEEP, D3D12_STENCIL_OP_KEEP, D3D12_STENCIL_OP_KEEP, D3D12_COMPARISON_FUNC_ALWAYS};
		psoDesc.pRootSignature							  = m_rootSigStandard.Get();
		psoDesc.RasterizerState							  = CD3DX12_RASTERIZER_DESC(D3D12_DEFAULT);
		psoDesc.BlendState								  = blendDesc;
		psoDesc.DepthStencilState.DepthEnable			  = TRUE;
		psoDesc.DepthStencilState.DepthWriteMask		  = D3D12_DEPTH_WRITE_MASK_ALL;
		psoDesc.DepthStencilState.DepthFunc				  = D3D12_COMPARISON_FUNC_LESS_EQUAL;
		psoDesc.DepthStencilState.StencilEnable			  = FALSE;
		psoDesc.DepthStencilState.StencilReadMask		  = D3D12_DEFAULT_STENCIL_READ_MASK;
		psoDesc.DepthStencilState.StencilWriteMask		  = D3D12_DEFAULT_STENCIL_WRITE_MASK;
		psoDesc.DepthStencilState.FrontFace				  = defaultStencilOp;
		psoDesc.DepthStencilState.BackFace				  = defaultStencilOp;
		psoDesc.SampleMask								  = UINT_MAX;
		psoDesc.PrimitiveTopologyType					  = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
		psoDesc.NumRenderTargets						  = 1;
		psoDesc.RTVFormats[0]							  = GetFormat(DEFAULT_RT_FORMAT);
		psoDesc.DSVFormat								  = GetFormat(DEFAULT_DEPTH_FORMAT);
		psoDesc.SampleDesc.Count						  = 1;
		psoDesc.RasterizerState.FillMode				  = D3D12_FILL_MODE_SOLID;

		if (ppType == PipelineType::Standard)
		{
			psoDesc.RasterizerState.CullMode			  = D3D12_CULL_MODE_BACK;
			psoDesc.RasterizerState.FrontCounterClockwise = TRUE;
		}
		else if (ppType == PipelineType::NoVertex)
		{
			psoDesc.RasterizerState.CullMode			  = D3D12_CULL_MODE_NONE;
			psoDesc.RasterizerState.FrontCounterClockwise = TRUE;
		}
		else if (ppType == PipelineType::GUI)
		{
			psoDesc.RasterizerState.CullMode			  = D3D12_CULL_MODE_NONE;
			psoDesc.RasterizerState.FrontCounterClockwise = FALSE;
		}

		for (const auto& [stg, title] : stages)
		{
			const auto&	 bc		  = shader->GetCompiledCode(stg);
			const void*	 byteCode = (void*)bc.data;
			const SIZE_T length	  = static_cast<SIZE_T>(bc.dataSize);

			if (stg == ShaderStage::Vertex)
			{
				psoDesc.VS.pShaderBytecode = byteCode;
				psoDesc.VS.BytecodeLength  = length;
			}
			else if (stg == ShaderStage::Fragment)
			{
				psoDesc.PS.pShaderBytecode = byteCode;
				psoDesc.PS.BytecodeLength  = length;
			}
			else if (stg == ShaderStage::Geometry)
			{
				psoDesc.GS.pShaderBytecode = byteCode;
				psoDesc.GS.BytecodeLength  = length;
			}
			else if (stg == ShaderStage::TesellationControl || stg == ShaderStage::TesellationEval)
			{
				psoDesc.HS.pShaderBytecode = byteCode;
				psoDesc.HS.BytecodeLength  = length;
			}
		}

		try
		{
			ThrowIfFailed(m_device->CreateGraphicsPipelineState(&psoDesc, IID_PPV_ARGS(&genData.pso)));
		}
		catch (HrException e)
		{
			LINA_CRITICAL("[Renderer] -> Exception when creating PSO! {0}", e.what());
		}

		return static_cast<int32>(index);
	}

	void Renderer::DestroyPipeline(uint32 handle)
	{
		auto& genData = m_shaders.GetItemR(handle);
		genData.pso.Reset();
		m_shaders.RemoveItem(handle);
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

				// We will still get reflection info, just with a smaller binary.
				arguments.push_back(L"-Qstrip_debug");
				arguments.push_back(L"-Qstrip_reflect");

#ifndef LINA_PRODUCTION_BUILD
				arguments.push_back(DXC_ARG_DEBUG); //-Zi
#else
				arguments.push_back(DXC_ARG_OPTIMIZATION_LEVEL3);
#endif

				for (int i = 0; i < 6; i++)
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
		}
	}

	int32 Renderer::GenerateImage(Texture* txt, ImageGenerateRequest req)
	{
		LOCK_GUARD(m_textureMtx);

		const int32 existingHandle = txt->GetGPUHandle();

		if (existingHandle != -1 && req.type != TextureResourceType::Texture2DDefault)
		{
			LINA_ERR("[Renderer] -> Re-generating images for textures is only supported for those textures of type Texture2DDefault!");
			return -1;
		}

		const uint32	  index	  = existingHandle == -1 ? m_textures.AddItem(GeneratedTexture()) : existingHandle;
		GeneratedTexture& genData = m_textures.GetItemR(index);
		auto&			  meta	  = txt->GetMetadata();
		const auto&		  ext	  = txt->GetExtent();
		genData.imageType		  = req.type;
		genData.sid				  = txt->GetSID();

		// Render target types will be implemented via CreateRenderTargetXXX functions.
		if (req.type == TextureResourceType::Texture2DDefault)
		{
			if (existingHandle == -1)
				genData.gpuResource = CreateTextureResource(TextureResourceType::Texture2DDefault, txt);

			DX12GfxTextureResource* txtResGPU = static_cast<DX12GfxTextureResource*>(genData.gpuResource);
			auto					format	  = static_cast<Format>(meta.GetUInt8("Format"_hs));

			m_uploadContext->CopyTextureQueueUp(txtResGPU, txt, req);

			if (existingHandle == -1)
			{
				genData.descriptor						= m_textureHeap->GetNewHeapHandle();
				D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
				srvDesc.Shader4ComponentMapping			= D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
				srvDesc.Format							= GetFormat(format);
				srvDesc.ViewDimension					= D3D12_SRV_DIMENSION_TEXTURE2D;
				srvDesc.Texture2D.MipLevels				= txt->GetMipLevels();
				m_device->CreateShaderResourceView(txtResGPU->DX12GetAllocation()->GetResource(), &srvDesc, genData.descriptor.GetCPUHandle());
				m_loadedTextures.push_back({txt->GetSID(), index});
			}
		}
		else if (req.type == TextureResourceType::Texture2DRenderTargetColor)
			m_loadedRTs.push_back({txt->GetSID(), index});

		return static_cast<int32>(index);
	}

	void Renderer::DestroyImage(uint32 handle)
	{
		GeneratedTexture& genData = m_textures.GetItemR(handle);

		if (genData.imageType == TextureResourceType::Texture2DSwapchain)
		{
			m_rtvHeap->FreeHeapHandle(genData.descriptor);
			genData.rawResource.Reset();
		}
		else if (genData.imageType == TextureResourceType::Texture2DRenderTargetDepthStencil)
		{
			m_dsvHeap->FreeHeapHandle(genData.descriptor);
		}
		else if (genData.imageType == TextureResourceType::Texture2DDefault)
		{
			m_textureHeap->FreeHeapHandle(genData.descriptor);
		}
		else if (genData.imageType == TextureResourceType::Texture2DRenderTargetColor)
		{
			m_rtvHeap->FreeHeapHandle(genData.descriptor);
			m_textureHeap->FreeHeapHandle(genData.descriptor);
		}

		if (genData.gpuResource)
			delete genData.gpuResource;

		if (genData.imageType == TextureResourceType::Texture2DDefault)
			m_loadedTextures.erase(linatl::find_if(m_loadedTextures.begin(), m_loadedTextures.end(), [&genData](auto& data) { return data.sid == genData.sid; }));
		else if (genData.imageType == TextureResourceType::Texture2DRenderTargetColor)
			m_loadedRTs.erase(linatl::find_if(m_loadedRTs.begin(), m_loadedRTs.end(), [&genData](auto& data) { return data.sid == genData.sid; }));

		m_textures.RemoveItem(handle);
	}

	int32 Renderer::GenerateSampler(TextureSampler* sampler)
	{
		LOCK_GUARD(m_samplerMtx);
		const uint32	  existingHandle = sampler->GetGPUHandle();
		const uint32	  index			 = existingHandle == -1 ? m_samplers.AddItem(GeneratedSampler()) : existingHandle;
		GeneratedSampler& genData		 = m_samplers.GetItemR(index);
		const auto&		  samplerData	 = sampler->GetSamplerData();
		genData.sid						 = sampler->GetSID();

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
		genData.descriptor	= m_samplerHeap->GetNewHeapHandle();
		m_device->CreateSampler(&desc, genData.descriptor.GetCPUHandle());

		m_loadedSamplers.push_back({sampler->GetSID(), index});
		return static_cast<int32>(index);
	}

	void Renderer::DestroySampler(uint32 handle)
	{
		GeneratedSampler& genData = m_samplers.GetItemR(handle);
		m_samplerHeap->FreeHeapHandle(genData.descriptor);
		m_loadedSamplers.erase(linatl::find_if(m_loadedSamplers.begin(), m_loadedSamplers.end(), [&genData](auto& data) { return data.sid == genData.sid; }));
		m_samplers.RemoveItem(handle);
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
		}

		return handle;
	}

	void Renderer::ReleaseCommanAllocator(uint32 handle)
	{
		m_cmdAllocators.GetItemR(handle).Reset();
		m_cmdAllocators.RemoveItem(handle);
	}

	IGfxCPUResource* Renderer::CreateCPUResource(size_t size, CPUResourceHint hint, const wchar_t* name)
	{
		return new DX12CPUResource(this, hint, size, name);
	}

	IGfxGPUResource* Renderer::CreateGPUResource(size_t size, GPUResourceType type, bool requireJoinBeforeUpdating, const wchar_t* name)
	{
		return new DX12GPUResource(this, type, requireJoinBeforeUpdating, size, name);
	}

	IGfxTextureResource* Renderer::CreateTextureResource(TextureResourceType type, Texture* texture)
	{
		return new DX12GfxTextureResource(this, texture, type);
	}

	void Renderer::DeleteCPUResource(IGfxCPUResource* res)
	{
		delete res;
	}

	void Renderer::DeleteGPUResource(IGfxGPUResource* res)
	{
		delete res;
	}

	uint32 Renderer::GetTextureIndex(const StringID sid)
	{
		auto it = linatl::find_if(m_loadedTextures.begin(), m_loadedTextures.end(), [sid](auto& data) { return data.sid == sid; });

		if (it == m_loadedTextures.end())
			it = linatl::find_if(m_loadedRTs.begin(), m_loadedRTs.end(), [sid](auto& data) { return data.sid == sid; });

		return it->shaderIndex;
	}

	uint32 Renderer::GetSamplerIndex(const StringID textureSid)
	{
		const StringID samplerSid = m_resourceManager->GetResource<Texture>(textureSid)->GetSampler();
		auto		   it		  = linatl::find_if(m_loadedSamplers.begin(), m_loadedSamplers.end(), [samplerSid](auto& data) { return data.sid == samplerSid; });

		return it->shaderIndex;
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
			cmdList->SetGraphicsRootDescriptorTable(6, m_gpuBufferHeap[m_currentFrameIndex]->GetHeapGPUStart());
			cmdList->SetGraphicsRootDescriptorTable(7, m_gpuSamplerHeap[m_currentFrameIndex]->GetHeapGPUStart());
		}
	}

	void Renderer::PrepareRenderTargets(Texture** renderTargets, uint32 renderTargetsSize)
	{
		auto		 texturesHeap	  = m_gpuBufferHeap[m_currentFrameIndex];
		const uint32 heapStart		  = texturesHeap->GetCurrentDescriptorIndex();
		auto		 alloc			  = texturesHeap->GetHeapHandleBlock(renderTargetsSize);
		const uint32 txtHeapIncrement = texturesHeap->GetDescriptorSize();

		Vector<D3D12_CPU_DESCRIPTOR_HANDLE> srcDescriptors;
		Vector<D3D12_CPU_DESCRIPTOR_HANDLE> destDescriptors;
		srcDescriptors.resize(renderTargetsSize, {});
		destDescriptors.resize(renderTargetsSize, {});

		Taskflow tf;
		tf.for_each_index(0, static_cast<int>(renderTargetsSize), 1, [&](int i) {
			auto it			= linatl::find_if(m_loadedRTs.begin(), m_loadedRTs.end(), [&](const LoadedResourceData& data) { return data.sid == renderTargets[i]->GetSID(); });
			it->shaderIndex = heapStart + i;

			D3D12_CPU_DESCRIPTOR_HANDLE handle;
			handle.ptr		   = alloc.GetCPUHandle().ptr + i * txtHeapIncrement;
			destDescriptors[i] = handle;

			auto& txtGenData  = m_textures.GetItemR(it->idListIndex);
			srcDescriptors[i] = txtGenData.descriptorSecondary.GetCPUHandle();
		});

		m_gfxManager->GetSystem()->GetMainExecutor()->RunAndWait(tf);
		m_device->CopyDescriptors(renderTargetsSize, destDescriptors.data(), NULL, renderTargetsSize, srcDescriptors.data(), NULL, D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
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
			ResourceTransition&		t		= transitions[i];
			auto&					genData = m_textures.GetItemR(t.texture->GetGPUHandle());
			DX12GfxTextureResource* res		= static_cast<DX12GfxTextureResource*>(genData.gpuResource);

			if (t.type == ResourceTransitionType::SRV2RT)
				barriers.push_back(CD3DX12_RESOURCE_BARRIER::Transition(res->DX12GetAllocation()->GetResource(), D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE, D3D12_RESOURCE_STATE_RENDER_TARGET));
			else if (t.type == ResourceTransitionType::RT2SRV)
				barriers.push_back(CD3DX12_RESOURCE_BARRIER::Transition(res->DX12GetAllocation()->GetResource(), D3D12_RESOURCE_STATE_RENDER_TARGET, D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE));
			else if (t.type == ResourceTransitionType::RT2Present)
				barriers.push_back(CD3DX12_RESOURCE_BARRIER::Transition(genData.rawResource.Get(), D3D12_RESOURCE_STATE_RENDER_TARGET, D3D12_RESOURCE_STATE_PRESENT));
			else if (t.type == ResourceTransitionType::Present2RT)
				barriers.push_back(CD3DX12_RESOURCE_BARRIER::Transition(genData.rawResource.Get(), D3D12_RESOURCE_STATE_PRESENT, D3D12_RESOURCE_STATE_RENDER_TARGET));
		}

		cmdList->ResourceBarrier(static_cast<UINT>(barriers.size()), barriers.data());
	}

	void Renderer::BeginRenderPass(uint32 cmdListHandle, Texture* colorTexture)
	{
		auto& cmdList = m_cmdLists.GetItemR(cmdListHandle);
		auto& genData = m_textures.GetItemR(colorTexture->GetGPUHandle());

		const float							 cc[]{DEFAULT_CLEAR_CLR.x, DEFAULT_CLEAR_CLR.y, DEFAULT_CLEAR_CLR.z, DEFAULT_CLEAR_CLR.w};
		CD3DX12_CLEAR_VALUE					 clearValue{GetFormat(DEFAULT_RT_FORMAT), cc};
		D3D12_RENDER_PASS_BEGINNING_ACCESS	 renderPassBeginningAccessClear{D3D12_RENDER_PASS_BEGINNING_ACCESS_TYPE_CLEAR, {clearValue}};
		D3D12_RENDER_PASS_ENDING_ACCESS		 renderPassEndingAccessPreserve{D3D12_RENDER_PASS_ENDING_ACCESS_TYPE_PRESERVE, {}};
		D3D12_RENDER_PASS_RENDER_TARGET_DESC renderPassRenderTargetDesc{genData.descriptor.GetCPUHandle(), renderPassBeginningAccessClear, renderPassEndingAccessPreserve};

		cmdList->BeginRenderPass(1, &renderPassRenderTargetDesc, nullptr, D3D12_RENDER_PASS_FLAG_NONE);
	}

	void Renderer::BeginRenderPass(uint32 cmdListHandle, Texture* colorTexture, Texture* depthStencilTexture)
	{
		auto& cmdList			  = m_cmdLists.GetItemR(cmdListHandle);
		auto& genDataCol		  = m_textures.GetItemR(colorTexture->GetGPUHandle());
		auto& genDataDepthStencil = m_textures.GetItemR(depthStencilTexture->GetGPUHandle());

		const float			cc[]{DEFAULT_CLEAR_CLR.x, DEFAULT_CLEAR_CLR.y, DEFAULT_CLEAR_CLR.z, DEFAULT_CLEAR_CLR.w};
		CD3DX12_CLEAR_VALUE clearValue{GetFormat(DEFAULT_RT_FORMAT), cc};
		CD3DX12_CLEAR_VALUE clearDepth{GetFormat(DEFAULT_DEPTH_FORMAT), 1.0f, 0};

		D3D12_RENDER_PASS_BEGINNING_ACCESS	 colorBegin{D3D12_RENDER_PASS_BEGINNING_ACCESS_TYPE_CLEAR, {clearValue}};
		D3D12_RENDER_PASS_ENDING_ACCESS		 colorEnd{D3D12_RENDER_PASS_ENDING_ACCESS_TYPE_PRESERVE, {}};
		D3D12_RENDER_PASS_RENDER_TARGET_DESC colorDesc{genDataCol.descriptor.GetCPUHandle(), colorBegin, colorEnd};

		D3D12_RENDER_PASS_BEGINNING_ACCESS	 depthBegin{D3D12_RENDER_PASS_BEGINNING_ACCESS_TYPE_CLEAR, {clearDepth}};
		D3D12_RENDER_PASS_ENDING_ACCESS		 depthEnd{D3D12_RENDER_PASS_ENDING_ACCESS_TYPE_PRESERVE, {}};
		D3D12_RENDER_PASS_DEPTH_STENCIL_DESC depthDesc{genDataDepthStencil.descriptor.GetCPUHandle(), depthBegin, depthBegin, depthEnd, depthEnd};

		cmdList->BeginRenderPass(1, &colorDesc, &depthDesc, D3D12_RENDER_PASS_FLAG_NONE);
	}

	void Renderer::EndRenderPass(uint32 cmdListHandle)
	{
		auto& cmdList = m_cmdLists.GetItemR(cmdListHandle);
		cmdList->EndRenderPass();
	}

	void Renderer::BindUniformBuffer(uint32 cmdListHandle, uint32 bufferIndex, IGfxCPUResource* buf)
	{
		auto& cmdList = m_cmdLists.GetItemR(cmdListHandle);
		cmdList->SetGraphicsRootConstantBufferView(bufferIndex, buf->GetGPUPointer());
	}

	void Renderer::BindObjectBuffer(uint32 cmdListHandle, IGfxGPUResource* res)
	{
		auto& cmdList = m_cmdLists.GetItemR(cmdListHandle);
		cmdList->SetGraphicsRootShaderResourceView(4, res->GetGPUPointer());
	}

	void Renderer::BindMaterial(uint32 cmdListHandle, Material* mat, Bitmask16 bindFlags)
	{
		auto* shader = mat->GetShader();

		if (shader == nullptr)
		{
			LINA_ERR("[Renderer] -> Can not bind material because the material does not have an assigned shader!");
			return;
		}

		auto& cmdList = m_cmdLists.GetItemR(cmdListHandle);

		if (bindFlags.IsSet(MBF_BindShader))
		{
			auto& shaderData = m_shaders.GetItemR(shader->GetGPUHandle());
			cmdList->SetPipelineState(shaderData.pso.Get());
		}

		if (bindFlags.IsSet(MBF_BindMaterialProperties))
		{
			auto& matData = m_materials.GetItemR(mat->GetGPUHandle());

			if (matData.dirty[m_currentFrameIndex])
			{
				uint8* ptr = nullptr;
				size_t sz  = 0;
				mat->GetPropertyBlob(ptr, sz);
				matData.buffer[m_currentFrameIndex]->BufferData(ptr, sz);
				delete[] ptr;
				matData.dirty[m_currentFrameIndex] = false;
			}

			cmdList->SetGraphicsRootConstantBufferView(5, matData.buffer[m_currentFrameIndex]->GetGPUPointer());
		}
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

	void Renderer::DrawIndexedIndirect(uint32 cmdListHandle, IGfxCPUResource* indirectBuffer, uint32 count, uint64 indirectOffset)
	{
		auto&			 cmdList = m_cmdLists.GetItemR(cmdListHandle);
		DX12CPUResource* buf	 = static_cast<DX12CPUResource*>(indirectBuffer);
		cmdList->ExecuteIndirect(m_commandSigStandard.Get(), count, buf->DX12GetAllocation()->GetResource(), indirectOffset, nullptr, 0);
	}

	void Renderer::SetTopology(uint32 cmdListHandle, Topology topology)
	{
		auto& cmdList = m_cmdLists.GetItemR(cmdListHandle);
		cmdList->IASetPrimitiveTopology(GetTopology(topology));
	}

	void Renderer::BindVertexBuffer(uint32 cmdListHandle, IGfxGPUResource* buffer, size_t vertexSize, uint32 slot)
	{
		auto&					 cmdList = m_cmdLists.GetItemR(cmdListHandle);
		D3D12_VERTEX_BUFFER_VIEW view;
		view.BufferLocation = buffer->GetGPUPointer();
		view.SizeInBytes	= static_cast<UINT>(buffer->GetSize());
		view.StrideInBytes	= static_cast<UINT>(vertexSize);
		cmdList->IASetVertexBuffers(slot, 1, &view);
	}

	void Renderer::BindIndexBuffer(uint32 cmdListHandle, IGfxGPUResource* buffer)
	{
		auto&					cmdList = m_cmdLists.GetItemR(cmdListHandle);
		D3D12_INDEX_BUFFER_VIEW view;
		view.BufferLocation = buffer->GetGPUPointer();
		view.SizeInBytes	= static_cast<UINT>(buffer->GetSize());
		view.Format			= DXGI_FORMAT_R32_UINT;
		cmdList->IASetIndexBuffer(&view);
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
			.path		   = path,
			.sid		   = sid,
			.extent		   = ext,
			.format		   = DEFAULT_RT_FORMAT,
			.targetSampler = DEFAULT_SAMPLER_SID,
			.tiling		   = ImageTiling::Optimal,
		};

		Texture* rt = new Texture(m_resourceManager, textureData);
		rt->GenerateImage({TextureResourceType::Texture2DRenderTargetColor});

		auto& genData					  = m_textures.GetItemR(rt->GetGPUHandle());
		genData.gpuResource				  = CreateTextureResource(TextureResourceType::Texture2DRenderTargetColor, rt);
		genData.descriptor				  = m_rtvHeap->GetNewHeapHandle();
		genData.descriptorSecondary		  = m_textureHeap->GetNewHeapHandle();
		DX12GfxTextureResource* txtResGPU = static_cast<DX12GfxTextureResource*>(genData.gpuResource);

		D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
		srvDesc.Shader4ComponentMapping			= D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
		srvDesc.Format							= GetFormat(DEFAULT_RT_FORMAT);
		srvDesc.ViewDimension					= D3D12_SRV_DIMENSION_TEXTURE2D;
		srvDesc.Texture2D.MipLevels				= 1;
		m_device->CreateShaderResourceView(txtResGPU->DX12GetAllocation()->GetResource(), &srvDesc, genData.descriptorSecondary.GetCPUHandle());

		// Create view
		D3D12_RENDER_TARGET_VIEW_DESC desc = {};
		desc.Format						   = GetFormat(DEFAULT_RT_FORMAT);
		desc.ViewDimension				   = D3D12_RTV_DIMENSION_TEXTURE2D;
		desc.Texture2D.MipSlice			   = 0;

		m_device->CreateRenderTargetView(txtResGPU->DX12GetAllocation()->GetResource(), &desc, genData.descriptor.GetCPUHandle());
		return rt;
	}

	Texture* Renderer::CreateRenderTargetSwapchain(ISwapchain* swp, uint32 bufferIndex, const String& path)
	{
		// Generate image data.
		const StringID sid = TO_SID(path);
		Texture*	   rt  = new Texture(m_resourceManager, true, path, sid);
		rt->GenerateImage({TextureResourceType::Texture2DSwapchain});

		// alloc descriptor
		auto& genData	   = m_textures.GetItemR(rt->GetGPUHandle());
		genData.descriptor = m_rtvHeap->GetNewHeapHandle();

		// Copy swapchain res into descriptor
		DX12Swapchain* dx12Swap = static_cast<DX12Swapchain*>(swp);
		try
		{
			ThrowIfFailed(dx12Swap->GetPtr()->GetBuffer(bufferIndex, IID_PPV_ARGS(&genData.rawResource)));
		}
		catch (HrException e)
		{
			LINA_CRITICAL("[Renderer] -> Exception while creating render target for swapchain! {0}", e.what());
		}

		D3D12_RENDER_TARGET_VIEW_DESC desc = {};
		desc.Format						   = GetFormat(DEFAULT_RT_FORMAT);
		desc.ViewDimension				   = D3D12_RTV_DIMENSION_TEXTURE2D;

		m_device->CreateRenderTargetView(genData.rawResource.Get(), &desc, genData.descriptor.GetCPUHandle());
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
			.path		   = path,
			.sid		   = sid,
			.extent		   = ext,
			.format		   = DEFAULT_DEPTH_FORMAT,
			.targetSampler = DEFAULT_SAMPLER_SID,
			.tiling		   = ImageTiling::Optimal,
		};

		Texture* rt = new Texture(m_resourceManager, textureData);
		rt->GenerateImage({TextureResourceType::Texture2DRenderTargetDepthStencil});

		// Alloc descriptor & resource
		auto& genData		= m_textures.GetItemR(rt->GetGPUHandle());
		genData.descriptor	= m_dsvHeap->GetNewHeapHandle();
		genData.gpuResource = CreateTextureResource(TextureResourceType::Texture2DRenderTargetDepthStencil, rt);

		// Create view
		DX12GfxTextureResource*		  res			   = static_cast<DX12GfxTextureResource*>(genData.gpuResource);
		D3D12_DEPTH_STENCIL_VIEW_DESC depthStencilDesc = {};
		depthStencilDesc.Format						   = GetFormat(DEFAULT_DEPTH_FORMAT);
		depthStencilDesc.ViewDimension				   = D3D12_DSV_DIMENSION_TEXTURE2D;
		depthStencilDesc.Flags						   = D3D12_DSV_FLAG_NONE;
		m_device->CreateDepthStencilView(res->DX12GetAllocation()->GetResource(), &depthStencilDesc, genData.descriptor.GetCPUHandle());

		return rt;
	}

} // namespace Lina
