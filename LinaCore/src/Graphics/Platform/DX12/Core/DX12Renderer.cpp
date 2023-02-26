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
#include "Graphics/Core/GfxManager.hpp"
#include "Graphics/Resource/Shader.hpp"
#include "Graphics/Resource/Texture.hpp"
#include "Graphics/Resource/Material.hpp"
#include "FileSystem/FileSystem.hpp"
#include "System/ISystem.hpp"
#include "Resources/Core/ResourceManager.hpp"
#include "Math/Rect.hpp"
#include "Math/Color.hpp"
#include "Log/Log.hpp"

using Microsoft::WRL::ComPtr;

namespace Lina
{
	Renderer::State Renderer::s_state;
	DWORD			msgCallback = 0;

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
		s_state.gfxManager = gfxMan;

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
			ThrowIfFailed(CreateDXGIFactory2(dxgiFactoryFlags, IID_PPV_ARGS(&s_state.factory)));
		}

		// Choose gpu & create device
		{
			if (initInfo.preferredGPUType == PreferredGPUType::CPU)
			{
				ComPtr<IDXGIAdapter> warpAdapter;
				ThrowIfFailed(s_state.factory->EnumWarpAdapter(IID_PPV_ARGS(&warpAdapter)));

				ThrowIfFailed(D3D12CreateDevice(warpAdapter.Get(), D3D_FEATURE_LEVEL_11_0, IID_PPV_ARGS(&s_state.device)));
			}
			else
			{
				GetHardwareAdapter(s_state.factory.Get(), &s_state.adapter, initInfo.preferredGPUType);

				ThrowIfFailed(D3D12CreateDevice(s_state.adapter.Get(), D3D_FEATURE_LEVEL_11_0, IID_PPV_ARGS(&s_state.device)));
			}
		}

#ifndef LINA_PRODUCTION_BUILD
		// Dbg callback
		{
			ID3D12InfoQueue1* infoQueue = nullptr;
			if (SUCCEEDED(s_state.device->QueryInterface<ID3D12InfoQueue1>(&infoQueue)))
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
			ThrowIfFailed(s_state.device->CreateCommandQueue(&queueDesc, IID_PPV_ARGS(&s_state.graphicsQueue)));

			queueDesc.Type = D3D12_COMMAND_LIST_TYPE_COPY;
			ThrowIfFailed(s_state.device->CreateCommandQueue(&queueDesc, IID_PPV_ARGS(&s_state.copyQueue)));
		}

		// Heaps
		{
			s_state.bufferHeap.Create(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV, D3D12_DESCRIPTOR_HEAP_FLAG_NONE, 100);
			s_state.dsvHeap.Create(D3D12_DESCRIPTOR_HEAP_TYPE_DSV, D3D12_DESCRIPTOR_HEAP_FLAG_NONE, 10);
			s_state.rtvHeap.Create(D3D12_DESCRIPTOR_HEAP_TYPE_RTV, D3D12_DESCRIPTOR_HEAP_FLAG_NONE, 10);
			s_state.samplerHeap.Create(D3D12_DESCRIPTOR_HEAP_TYPE_SAMPLER, D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE, 100);
		}

		// Allocator
		{
			D3D12MA::ALLOCATOR_DESC allocatorDesc;
			allocatorDesc.pDevice			   = s_state.device.Get();
			allocatorDesc.PreferredBlockSize   = 0;
			allocatorDesc.Flags				   = D3D12MA::ALLOCATOR_FLAG_NONE;
			allocatorDesc.pAdapter			   = s_state.adapter.Get();
			allocatorDesc.pAllocationCallbacks = NULL;
			ThrowIfFailed(D3D12MA::CreateAllocator(&allocatorDesc, &s_state.dx12Allocator));
		}
	}

	void Renderer::Initialize(const SystemInitializationInfo& initInfo)
	{
		// DXC
		{
			HRESULT hr = DxcCreateInstance(CLSID_DxcLibrary, IID_PPV_ARGS(&s_state.idxcLib));
			if (FAILED(hr))
			{
				LINA_CRITICAL("[D3D12 Gpu Storage] -> Failed to create DXC library!");
			}
		}

		// Standard Pipeline Root Signature
		{
			/*******************************************************/
			/** PARAM0: CBV				-> Global Data			-> REG:b0
			/**
			/** PARAM1: CBV TABLE
			/**							-> Scene Data			-> REG:b1
			/**							-> View Data			-> REG:b2
			/**							-> Light Data			-> REG:b3
			/**
			/** PARAM2: CBV				-> Material Data        -> REG:b4
			/** PARAM3: Sampler Table	-> Material Samplers	-> REG:[s0-s9]
			/** PARAM4: SRV Table		-> Material Textures	-> REG:[t0-t9]
			/** PARAM5: SRV				-> Object Buffer View	-> REG:t10
			/*******************************************************/

			CD3DX12_DESCRIPTOR_RANGE1 descRange[3] = {};

			// Scene Data - View Data - Light Data
			descRange[0].Init(D3D12_DESCRIPTOR_RANGE_TYPE_CBV, 3, 1);

			// Textures & Samplers
			descRange[1].Init(D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 10, 0, 0, D3D12_DESCRIPTOR_RANGE_FLAG_DATA_VOLATILE);
			descRange[2].Init(D3D12_DESCRIPTOR_RANGE_TYPE_SAMPLER, 10, 0);

			CD3DX12_ROOT_PARAMETER1 rp[6];

			// Global constant buffer
			rp[0].InitAsConstantBufferView(0, 0, D3D12_ROOT_DESCRIPTOR_FLAG_NONE, D3D12_SHADER_VISIBILITY_VERTEX);

			// Scene & View & Light data
			rp[1].InitAsDescriptorTable(1, &descRange[0], D3D12_SHADER_VISIBILITY_VERTEX);

			// Material data
			rp[2].InitAsConstantBufferView(4, 0, D3D12_ROOT_DESCRIPTOR_FLAG_DATA_VOLATILE, D3D12_SHADER_VISIBILITY_ALL);

			// Texture & samplers
			rp[3].InitAsDescriptorTable(1, &descRange[1], D3D12_SHADER_VISIBILITY_ALL);
			rp[4].InitAsDescriptorTable(1, &descRange[2], D3D12_SHADER_VISIBILITY_ALL);

			// Object buffer.
			rp[5].InitAsShaderResourceView(10, 0, D3D12_ROOT_DESCRIPTOR_FLAG_DATA_VOLATILE, D3D12_SHADER_VISIBILITY_VERTEX);

			CD3DX12_STATIC_SAMPLER_DESC samplerDesc[1] = {};
			samplerDesc[0].Init(10, D3D12_FILTER_ANISOTROPIC);

			CD3DX12_VERSIONED_ROOT_SIGNATURE_DESC rootSig(6, rp, 1, samplerDesc, D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT);
			ComPtr<ID3DBlob>					  signatureBlob = nullptr;
			ComPtr<ID3DBlob>					  errorBlob		= nullptr;

			HRESULT hr = D3D12SerializeVersionedRootSignature(&rootSig, &signatureBlob, &errorBlob);

			if (FAILED(hr))
			{
				LINA_CRITICAL("[Shader Compiler] -> Failed serializing root signature! {0}", (char*)errorBlob->GetBufferPointer());
			}

			hr = s_state.device->CreateRootSignature(0, signatureBlob->GetBufferPointer(), signatureBlob->GetBufferSize(), IID_PPV_ARGS(&s_state.rootSigStandard));

			if (FAILED(hr))
			{
				LINA_CRITICAL("[Shader Compiler] -> Failed creating root signature!");
			}
		}
	}

	void Renderer::Shutdown()
	{
		ID3D12InfoQueue1* infoQueue = nullptr;
		if (SUCCEEDED(s_state.device->QueryInterface<ID3D12InfoQueue1>(&infoQueue)))
		{
			infoQueue->UnregisterMessageCallback(msgCallback);
		}

		s_state.dx12Allocator->Release();
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

	uint32 Renderer::GenerateMaterial(Material* mat, uint32 existingHandle)
	{
		LOCK_GUARD(s_state.shaderMtx);

		const uint32	   index   = existingHandle == -1 ? s_state.materials.AddItem(GeneratedMaterial()) : existingHandle;
		GeneratedMaterial& genData = s_state.materials.GetItemR(index);

		return index;
	}

	void Renderer::UpdateMaterialProperties(Material* mat, uint32 imageIndex)
	{
	}

	void Renderer::UpdateMaterialTextures(Material* mat, uint32 imageIndex, const Vector<uint32>& dirtyTextures)
	{
	}

	void Renderer::DestroyMaterial(uint32 handle)
	{
		// Note: no need to mtx lock, this is called from the main thread.
		const uint32 index	 = handle;
		auto&		 genData = s_state.materials.GetItemR(index);
		s_state.materials.RemoveItem(index);
	}

	uint32 Renderer::GeneratePipeline(Shader* shader)
	{
		LOCK_GUARD(s_state.shaderMtx);

		const uint32	   index   = s_state.shaders.AddItem(GeneratedShader());
		auto&			   genData = s_state.shaders.GetItemR(index);
		const auto&		   stages  = shader->GetStages();
		const PipelineType ppType  = shader->GetPipelineType();

		Vector<D3D12_INPUT_ELEMENT_DESC> inputLayout;

		// Define the vertex input layout.
		{
			if (ppType == PipelineType::Standard)
			{
				inputLayout.push_back({"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0});
				inputLayout.push_back({"NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 12, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0});
				inputLayout.push_back({"COLOR", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 24, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0});
				inputLayout.push_back({"TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 36, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0});
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

		psoDesc.pRootSignature					= s_state.rootSigStandard.Get();
		psoDesc.RasterizerState					= CD3DX12_RASTERIZER_DESC(D3D12_DEFAULT);
		psoDesc.BlendState						= CD3DX12_BLEND_DESC(D3D12_DEFAULT);
		psoDesc.DepthStencilState.DepthEnable	= FALSE;
		psoDesc.DepthStencilState.StencilEnable = FALSE;
		psoDesc.SampleMask						= UINT_MAX;
		psoDesc.PrimitiveTopologyType			= D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
		psoDesc.NumRenderTargets				= 1;
		psoDesc.RTVFormats[0]					= GetFormat(DEFAULT_SWAPCHAIN_FORMAT);
		psoDesc.SampleDesc.Count				= 1;
		psoDesc.RasterizerState.FillMode		= D3D12_FILL_MODE_SOLID;

		if (ppType == PipelineType::Standard)
		{
			psoDesc.RasterizerState.CullMode			  = D3D12_CULL_MODE_BACK;
			psoDesc.RasterizerState.FrontCounterClockwise = FALSE;
		}
		else if (ppType == PipelineType::NoVertex)
		{
			psoDesc.RasterizerState.CullMode			  = D3D12_CULL_MODE_FRONT;
			psoDesc.RasterizerState.FrontCounterClockwise = TRUE;
		}
		else if (ppType == PipelineType::GUI)
		{
			psoDesc.RasterizerState.CullMode			  = D3D12_CULL_MODE_NONE;
			psoDesc.RasterizerState.FrontCounterClockwise = TRUE;
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

		ThrowIfFailed(s_state.device->CreateGraphicsPipelineState(&psoDesc, IID_PPV_ARGS(&genData.pso)));

		return index;
	}

	void Renderer::DestroyPipeline(uint32 handle)
	{
		const uint32 index	 = handle;
		auto&		 genData = s_state.shaders.GetItemR(index);

		s_state.shaders.RemoveItem(index);
	}

	void Renderer::CompileShader(const char* path, const HashMap<ShaderStage, String>& stages, HashMap<ShaderStage, ShaderByteCode>& outCompiledCode)
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
			ThrowIfFailed(s_state.idxcLib->CreateBlobFromFile(pathw, &codePage, &sourceBlob));

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
			ThrowIfFailed(idxcCompiler->Compile(&sourceBuffer, arguments.data(), static_cast<uint32>(arguments.size()), &s_state.includeInterface, IID_PPV_ARGS(result.GetAddressOf())));

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

	uint32 Renderer::GenerateImage(Texture* txt, ImageType type)
	{
		LOCK_GUARD(s_state.textureMtx);

		const uint32	  index	  = s_state.textures.AddItem(GeneratedTexture());
		GeneratedTexture& genData = s_state.textures.GetItemR(index);
		auto&			  meta	  = txt->GetMetadata();
		const auto&		  ext	  = txt->GetExtent();
		genData.imageType		  = type;

		// Rest will be done by CreateRenderTarget
		if (type == ImageType::Swapchain)
		{
			genData.descriptor = s_state.rtvHeap.Allocate();
		}
		else
		{
		}

		return index;
	}

	uint32 Renderer::GenerateImageAndUpload(Texture* txt)
	{
		LOCK_GUARD(s_state.textureMtx);

		const uint32	  index	  = s_state.textures.AddItem(GeneratedTexture());
		GeneratedTexture& genData = s_state.textures.GetItemR(index);
		auto&			  meta	  = txt->GetMetadata();
		const auto&		  ext	  = txt->GetExtent();

		return index;
	}

	void Renderer::DestroyImage(uint32 handle)
	{
		const uint32	  index	  = handle;
		GeneratedTexture& genData = s_state.textures.GetItemR(index);

		if (genData.imageType == ImageType::Swapchain)
		{
			s_state.rtvHeap.Free(genData.descriptor);
			genData.resource.Reset();
		}

		s_state.textures.RemoveItem(index);
	}

	ISwapchain* Renderer::CreateSwapchain(const Vector2i& size, void* windowHandle)
	{
		return new DX12Swapchain(size, windowHandle);
	}

	uint32 Renderer::CreateCommandAllocator(CommandType type)
	{
		const uint32 handle = s_state.cmdAllocators.AddItem(ComPtr<ID3D12CommandAllocator>());
		auto&		 alloc	= s_state.cmdAllocators.GetItemR(handle);
		ThrowIfFailed(s_state.device->CreateCommandAllocator(GetCommandType(type), IID_PPV_ARGS(alloc.GetAddressOf())));
		return handle;
	}

	uint32 Renderer::CreateCommandList(CommandType type, uint32 allocatorHandle)
	{
		const uint32 handle	   = s_state.cmdLists.AddItem(ComPtr<ID3D12GraphicsCommandList4>());
		auto&		 list	   = s_state.cmdLists.GetItemR(handle);
		auto&		 allocator = s_state.cmdAllocators.GetItemR(allocatorHandle);
		ThrowIfFailed(s_state.device->CreateCommandList(0, GetCommandType(type), allocator.Get(), nullptr, IID_PPV_ARGS(list.GetAddressOf())));
		ThrowIfFailed(list->Close());
		return handle;
	}

	uint32 Renderer::CreateFence()
	{
		const uint32 handle = s_state.fences.AddItem(ComPtr<ID3D12Fence>());
		auto&		 fence	= s_state.fences.GetItemR(handle);
		ThrowIfFailed(s_state.device->CreateFence(0, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(&fence)));
		return handle;
	}

	void Renderer::ReleaseCommanAllocator(uint32 handle)
	{
		s_state.cmdAllocators.GetItemR(handle).Reset();
		s_state.cmdAllocators.RemoveItem(handle);
	}

	void Renderer::ReleaseCommandList(uint32 handle)
	{
		s_state.cmdLists.GetItemR(handle).Reset();
		s_state.cmdLists.RemoveItem(handle);
	}

	void Renderer::SetCurrentCommandList(uint32 cmdAllocatorHandle, uint32 cmdListHandle)
	{
		auto& cmdAllocator		  = s_state.cmdAllocators.GetItemR(cmdAllocatorHandle);
		auto& cmdList			  = s_state.cmdLists.GetItemR(cmdListHandle);
		s_state._currentAllocator = cmdAllocator.Get();
		s_state._currentCmdList	  = cmdList.Get();
	}

	void Renderer::ResetCommandList()
	{
		ThrowIfFailed(s_state._currentAllocator->Reset());
		ThrowIfFailed(s_state._currentCmdList->Reset(s_state._currentAllocator, nullptr));
	}

	void Renderer::PrepareCommandList(const Viewport& viewport, const Recti& scissors)
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

		s_state._currentCmdList->SetGraphicsRootSignature(s_state.rootSigStandard.Get());
		s_state._currentCmdList->RSSetViewports(1, &vp);
		s_state._currentCmdList->RSSetScissorRects(1, &sc);
	}

	void Renderer::FinalizeCommandList()
	{
		ThrowIfFailed(s_state._currentCmdList->Close());
		s_state._currentCmdList = nullptr;
	}

	void Renderer::ExecuteCommandListsGraphics(const Vector<uint32>& lists)
	{
		const UINT sz = static_cast<UINT>(lists.size());

		Vector<ID3D12CommandList*> _lists;

		for (UINT i = 0; i < sz; i++)
		{
			auto& lst = s_state.cmdLists.GetItemR(lists[i]);
			_lists.push_back(lst.Get());
		}

		ID3D12CommandList* const* data = _lists.data();
		s_state.graphicsQueue->ExecuteCommandLists(sz, data);
	}

	void Renderer::Present(ISwapchain* swp)
	{
		DX12Swapchain* dx12Swap = static_cast<DX12Swapchain*>(swp);

		try
		{
			dx12Swap->GetPtr()->Present(1, 0);
		}
		catch (HrException& e)
		{
			if (e.Error() == DXGI_ERROR_DEVICE_REMOVED || e.Error() == DXGI_ERROR_DEVICE_RESET)
			{
			}
			else
			{
				throw;
			}
		}
	}

	uint32 Renderer::GetNextBackBuffer(ISwapchain* swp)
	{
		DX12Swapchain* dx12Swap = static_cast<DX12Swapchain*>(swp);
		return static_cast<uint32>(dx12Swap->GetPtr()->GetCurrentBackBufferIndex());
	}

	void Renderer::ReleaseFence(uint32 handle)
	{
		s_state.fences.GetItemR(handle).Reset();
		s_state.fences.RemoveItem(handle);
	}

	void Renderer::WaitForFences(uint32 fenceHandle, uint64 userData0, uint64 userData1)
	{
		// UserData0: Current Fence Value in caller
		// UserData1: Stored fence value in caller's frame

		auto&		 fence	   = s_state.fences.GetItemR(fenceHandle);
		const UINT64 lastFence = fence->GetCompletedValue();

		if (userData1 > lastFence)
		{
			HANDLE eventHandle = CreateEvent(nullptr, FALSE, FALSE, nullptr);
			if (eventHandle == nullptr)
			{
				ThrowIfFailed(HRESULT_FROM_WIN32(GetLastError()));
			}
			else
			{
				ThrowIfFailed(fence->SetEventOnCompletion(userData1, eventHandle));
				WaitForSingleObject(eventHandle, INFINITE);
				CloseHandle(eventHandle);
			}
		}
	}

	void Renderer::SignalFenceIncrementGraphics(uint32 fenceHandle, uint64 value)
	{
		auto& fence = s_state.fences.GetItemR(fenceHandle);
		s_state.graphicsQueue->Signal(fence.Get(), value);
	}

	Texture* Renderer::CreateRenderTarget(const String& path)
	{
		const StringID sid = TO_SID(path);
		Texture*	   rt  = new Texture(s_state.gfxManager->GetSystem()->CastSubsystem<ResourceManager>(SubsystemType::ResourceManager), true, path, sid);

		return rt;
	}

	Texture* Renderer::CreateRenderTarget(ISwapchain* swp, uint32 bufferIndex, const String& path)
	{
		const StringID sid = TO_SID(path);
		Texture*	   rt  = new Texture(s_state.gfxManager->GetSystem()->CastSubsystem<ResourceManager>(SubsystemType::ResourceManager), true, path, sid);
		rt->GenerateImage(ImageType::Swapchain);
		auto& genData = s_state.textures.GetItemR(rt->GetGPUHandle());

		DX12Swapchain* dx12Swap = static_cast<DX12Swapchain*>(swp);
		ThrowIfFailed(dx12Swap->GetPtr()->GetBuffer(bufferIndex, IID_PPV_ARGS(&genData.resource)));
		s_state.device->CreateRenderTargetView(genData.resource.Get(), nullptr, genData.descriptor.cpuHandle);

		return rt;
	}

	void Renderer::TransitionPresent2RT(Texture* txt)
	{
		auto& genData	  = s_state.textures.GetItemR(txt->GetGPUHandle());
		auto  presentToRT = CD3DX12_RESOURCE_BARRIER::Transition(genData.resource.Get(), D3D12_RESOURCE_STATE_PRESENT, D3D12_RESOURCE_STATE_RENDER_TARGET);
		s_state._currentCmdList->ResourceBarrier(1, &presentToRT);
	}

	void Renderer::TransitionRT2Present(Texture* txt)
	{
		auto& genData	  = s_state.textures.GetItemR(txt->GetGPUHandle());
		auto  rtToPresent = CD3DX12_RESOURCE_BARRIER::Transition(genData.resource.Get(), D3D12_RESOURCE_STATE_RENDER_TARGET, D3D12_RESOURCE_STATE_PRESENT);
		s_state._currentCmdList->ResourceBarrier(1, &rtToPresent);
	}

	void Renderer::BeginRenderPass(Texture* colorTexture, const Color& clearColor)
	{
		auto& genData = s_state.textures.GetItemR(colorTexture->GetGPUHandle());

		const float			cc[]{clearColor.x, clearColor.y, clearColor.z, clearColor.w};
		CD3DX12_CLEAR_VALUE clearValue{GetFormat(DEFAULT_COLOR_FORMAT), cc};

		D3D12_RENDER_PASS_BEGINNING_ACCESS	 renderPassBeginningAccessClear{D3D12_RENDER_PASS_BEGINNING_ACCESS_TYPE_CLEAR, {clearValue}};
		D3D12_RENDER_PASS_ENDING_ACCESS		 renderPassEndingAccessPreserve{D3D12_RENDER_PASS_ENDING_ACCESS_TYPE_PRESERVE, {}};
		D3D12_RENDER_PASS_RENDER_TARGET_DESC renderPassRenderTargetDesc{genData.descriptor.cpuHandle, renderPassBeginningAccessClear, renderPassEndingAccessPreserve};

		// D3D12_RENDER_PASS_BEGINNING_ACCESS	 renderPassBeginningAccessNoAccess{D3D12_RENDER_PASS_BEGINNING_ACCESS_TYPE_NO_ACCESS, {}};
		// D3D12_RENDER_PASS_ENDING_ACCESS		 renderPassEndingAccessNoAccess{D3D12_RENDER_PASS_ENDING_ACCESS_TYPE_NO_ACCESS, {}};
		// D3D12_RENDER_PASS_DEPTH_STENCIL_DESC renderPassDepthStencilDesc{dsvCPUDescriptorHandle, renderPassBeginningAccessNoAccess, renderPassBeginningAccessNoAccess, renderPassEndingAccessNoAccess, renderPassEndingAccessNoAccess};

		s_state._currentCmdList->BeginRenderPass(1, &renderPassRenderTargetDesc, nullptr, D3D12_RENDER_PASS_FLAG_NONE);
	}

	void Renderer::EndRenderPass()
	{
		s_state._currentCmdList->EndRenderPass();
	}

	// void Renderer::BindPipeline(ID3D12GraphicsCommandList* cmdList, Material* mat)
	// {
	// 	GeneratedShader& genData = s_state.shaders.GetItemR(mat->GetShader()->GetGPUHandle());
	// 	cmdList->SetPipelineState(genData.pso.Get());
	// }

} // namespace Lina
