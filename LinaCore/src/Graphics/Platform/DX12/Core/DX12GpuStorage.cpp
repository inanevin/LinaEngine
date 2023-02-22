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

#include "Graphics/Platform/DX12/Core/DX12GpuStorage.hpp"
#include "Graphics/Resource/Texture.hpp"
#include "FileSystem/FileSystem.hpp"
#include "Graphics/Resource/Shader.hpp"
#include "Graphics/Resource/Material.hpp"
#include "Graphics/Platform/DX12/Core/DX12GfxManager.hpp"
#include "Graphics/Platform/DX12/Core/DX12Backend.hpp"

using Microsoft::WRL::ComPtr;

namespace Lina
{
	DxcDefine macros[6] = {{L"LINA_PASS_OPAQUE", L""}, {L"LINA_PASS_SHADOWS", L""}, {L"LINA_PIPELINE_STANDARD", L""}, {L"LINA_PIPELINE_NOVERTEX", L""}, {L"LINA_PIPELINE_GUI", L""}, {L"LINA_MATERIAL", L"cbuffer MaterialBuffer : register(b4)"}};

	void DX12GpuStorage::Initilalize()
	{
		HRESULT hr = DxcCreateInstance(CLSID_DxcLibrary, IID_PPV_ARGS(&m_idxcLib));
		if (FAILED(hr))
		{
			LINA_CRITICAL("[D3D12 Gpu Storage] -> Failed to create DXC library!");
		}

		hr = DxcCreateInstance(CLSID_DxcCompiler, IID_PPV_ARGS(&m_idxcCompiler));
		if (FAILED(hr))
		{
			LINA_CRITICAL("[D3D12 Gpu Storage] -> Failed to create DXC compiler");
		}
	}

	void DX12GpuStorage::Shutdown()
	{
	}

	uint32 DX12GpuStorage::GenerateMaterial(Material* mat, uint32 existingHandle)
	{
		LOCK_GUARD(m_shaderMtx);

		const uint32	   index   = existingHandle == -1 ? m_materials.AddItem(GeneratedMaterial()) : existingHandle;
		GeneratedMaterial& genData = m_materials.GetItemR(index);

		return index;
	}

	void DX12GpuStorage::UpdateMaterialProperties(Material* mat, uint32 imageIndex)
	{
	}

	void DX12GpuStorage::UpdateMaterialTextures(Material* mat, uint32 imageIndex, const Vector<uint32>& dirtyTextures)
	{
	}

	void DX12GpuStorage::DestroyMaterial(uint32 handle)
	{
		// Note: no need to mtx lock, this is called from the main thread.
		const uint32 index	 = handle;
		auto&		 genData = m_materials.GetItemR(index);
		m_materials.RemoveItem(index);
	}

	uint32 DX12GpuStorage::GeneratePipeline(Shader* shader)
	{
		LOCK_GUARD(m_shaderMtx);

		const uint32	   index   = m_shaders.AddItem(GeneratedShader());
		auto&			   genData = m_shaders.GetItemR(index);
		const auto&		   stages  = shader->GetStages();
		const PipelineType ppType  = shader->GetPipelineType();

		// Define the vertex input layout.

		Vector<D3D12_INPUT_ELEMENT_DESC> inputLayout;

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

		// Describe and create the graphics pipeline state object (PSO).
		D3D12_GRAPHICS_PIPELINE_STATE_DESC psoDesc = {};

		if (!inputLayout.empty())
			psoDesc.InputLayout = {&inputLayout[0], static_cast<UINT>(inputLayout.size())};

		psoDesc.pRootSignature					= m_gfxManager->GetRootSignature();
		psoDesc.RasterizerState					= CD3DX12_RASTERIZER_DESC(D3D12_DEFAULT);
		psoDesc.BlendState						= CD3DX12_BLEND_DESC(D3D12_DEFAULT);
		psoDesc.DepthStencilState.DepthEnable	= FALSE;
		psoDesc.DepthStencilState.StencilEnable = FALSE;
		psoDesc.SampleMask						= UINT_MAX;
		psoDesc.PrimitiveTopologyType			= D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
		psoDesc.NumRenderTargets				= 1;
		psoDesc.RTVFormats[0]					= DEFAULT_SWAPCHAIN_FORMAT;
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

		ThrowIfFailed(m_gfxManager->GetDX12Backend()->GetDevice()->CreateGraphicsPipelineState(&psoDesc, IID_PPV_ARGS(&genData.pso)));

		return index;
	}

	void DX12GpuStorage::DestroyPipeline(uint32 handle)
	{
		const uint32 index	 = handle;
		auto&		 genData = m_shaders.GetItemR(index);

		m_shaders.RemoveItem(index);
	}

	void DX12GpuStorage::CompileShader(const char* path, const HashMap<ShaderStage, String>& stages, HashMap<ShaderStage, ShaderByteCode>& outCompiledCode)
	{

		wchar_t* pathw = FileSystem::CharToWChar(path);
		HRESULT	 hr;

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

			ComPtr<IDxcOperationResult> result;
			ThrowIfFailed(m_idxcCompiler->Compile(sourceBlob.Get(), pathw, entry, targetProfile, NULL, 0, &macros[0], 6, &m_includeInterface, &result));

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

	uint32 DX12GpuStorage::GenerateImage(Texture* txt, uint32 aspectFlags, uint32 imageUsageFlags)
	{
		LOCK_GUARD(m_textureMtx);

		const uint32	  index	  = m_textures.AddItem(GeneratedTexture());
		GeneratedTexture& genData = m_textures.GetItemR(index);
		auto&			  meta	  = txt->GetMetadata();
		const auto&		  ext	  = txt->GetExtent();

		return index;
	}

	uint32 DX12GpuStorage::GenerateImageAndUpload(Texture* txt)
	{
		LOCK_GUARD(m_textureMtx);

		const uint32	  index	  = m_textures.AddItem(GeneratedTexture());
		GeneratedTexture& genData = m_textures.GetItemR(index);
		auto&			  meta	  = txt->GetMetadata();
		const auto&		  ext	  = txt->GetExtent();

		return index;
	}

	void DX12GpuStorage::DestroyImage(uint32 handle)
	{
		const uint32	  index = handle;
		GeneratedTexture& gen	= m_textures.GetItemR(index);

		m_textures.RemoveItem(index);
	}

	void DX12GpuStorage::BindPipeline(ID3D12GraphicsCommandList* cmdList, Material* mat)
	{
		GeneratedShader& genData = m_shaders.GetItemR(mat->GetShader()->GetGPUHandle());
		cmdList->SetPipelineState(genData.pso.Get());
	}

} // namespace Lina
