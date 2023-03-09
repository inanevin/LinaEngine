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

#ifndef DX12Renderer_HPP
#define DX12Renderer_HPP

#include "Core/Common.hpp"
#include "Graphics/Core/CommonGraphics.hpp"
#include "Graphics/Data/RenderData.hpp"
#include "Graphics/Data/Vertex.hpp"
#include "Data/HashMap.hpp"
#include "Graphics/Platform/DX12/Core/DX12Common.hpp"
#include "Graphics/Platform/DX12/Core/DX12DescriptorHeap.hpp"
#include "Graphics/Platform/DX12/Core/DX12GpuUploader.hpp"
#include "Graphics/Platform/DX12/Utility/ID3DIncludeInterface.hpp"

namespace D3D12MA
{
	class Allocator;
}

namespace Lina
{
	class Material;
	class Shader;
	class Texture;
	class Recti;
	class GfxManager;
	class ISwapchain;
	class IGfxBufferResource;
	class IGfxTextureResource;
	class Color;

	struct GeneratedTexture
	{
		// Only for swapchain render target
		Microsoft::WRL::ComPtr<ID3D12Resource> rawResource;

		// For all others.
		IGfxTextureResource* gfxResource = nullptr;

		DescriptorHandle descriptor;
		ImageType		 imageType = ImageType::Color;
	};

	struct GeneratedShader
	{
		Microsoft::WRL::ComPtr<ID3D12PipelineState> pso;
	};

	struct GeneratedMaterial
	{
	};

	class Renderer
	{
	public:
		Renderer()
			: m_textures(100, GeneratedTexture()), m_materials(100, GeneratedMaterial()), m_shaders(100, GeneratedShader()), m_cmdAllocators(20, Microsoft::WRL::ComPtr<ID3D12CommandAllocator>()),
			  m_cmdLists(50, Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList4>()), m_fences(10, Microsoft::WRL::ComPtr<ID3D12Fence>()), m_rtvHeap(this), m_bufferHeap(this), m_dsvHeap(this), m_samplerHeap(this), m_uploader(this){};

		struct StatePerFrame
		{
			uint64 storedFenceGraphics = 0;
		};

		// ******************* SYSTEM ******************* //
		// ******************* SYSTEM ******************* //
		// ******************* SYSTEM ******************* //
		void PreInitialize(const SystemInitializationInfo& initInfo, GfxManager* gfxMan);
		void Initialize(const SystemInitializationInfo& initInfo);
		void Shutdown();

		// ******************* RESOURCES *******************
		// ******************* RESOURCES *******************
		// ******************* RESOURCES *******************
		uint32 GenerateMaterial(Material* mat, uint32 existingHandle);
		void   UpdateMaterialProperties(Material* mat, uint32 imageIndex);
		void   UpdateMaterialTextures(Material* mat, uint32 imageIndex, const Vector<uint32>& dirtyTextures);
		void   DestroyMaterial(uint32 handle);
		uint32 GeneratePipeline(Shader* shader);
		void   DestroyPipeline(uint32 handle);
		void   CompileShader(const char* path, const HashMap<ShaderStage, String>& stages, HashMap<ShaderStage, ShaderByteCode>& outCompiledCode);
		uint32 GenerateImage(Texture* txt, ImageType type);
		uint32 GenerateImageAndUpload(Texture* txt);
		void   DestroyImage(uint32 handle);

		// ******************* API *******************
		// ******************* API *******************
		// ******************* API *******************

		// Frame Control
		void BeginFrame(uint32 frameIndex);
		void EndFrame(uint32 frameIndex);
		void WaitForGPUGraphics();

		// Swapchain
		ISwapchain* CreateSwapchain(const Vector2i& size, void* windowHandle);
		void		Present(ISwapchain* swp);
		uint32		GetNextBackBuffer(ISwapchain* swp);

		// Resources
		IGfxBufferResource*	 CreateBufferResource(BufferResourceType type, void* initialData, size_t size);
		IGfxTextureResource* CreateTextureResource(TextureResourceType type, const Vector2i& initialSize);
		void				 DeleteBufferResource(IGfxBufferResource* res);

		// Commands
		uint32 CreateCommandAllocator(CommandType type);
		uint32 CreateCommandList(CommandType type, uint32 allocatorHandle);
		void   ReleaseCommanAllocator(uint32 handle);
		void   ReleaseCommandList(uint32 handle);
		void   ResetCommandList(uint32 cmdAllocatorHandle, uint32 cmdListHandle);
		void   PrepareCommandList(uint32 cmdListHandle, const Viewport& viewport, const Recti& scissors);
		void   FinalizeCommandList(uint32 cmdListHandle);
		void   ExecuteCommandListsGraphics(const Vector<uint32>& lists);
		void   ExecuteCommandListsTransfer(const Vector<uint32>& lists);
		void   TransitionPresent2RT(uint32 cmdListHandle, Texture* txt);
		void   TransitionRT2Present(uint32 cmdListHandle, Texture* txt);
		void   BeginRenderPass(uint32 cmdListHandle, Texture* colorTexture, const Color& clearColor);
		void   BeginRenderPass(uint32 cmdListHandle, Texture* colorTexture, Texture* depthStencil, const Color& clearColor);
		void   EndRenderPass(uint32 cmdListHandle);
		void   BindUniformBuffer(uint32 cmdListHandle, uint32 bufferIndex, IGfxBufferResource* buf);
		void   BindObjectBuffer(uint32 cmdListHandle, IGfxBufferResource* buf);
		void   BindMaterial(uint32 cmdListHandle, Material* mat);
		void   DrawInstanced(uint32 cmdListHandle, uint32 vertexCount, uint32 instanceCount, uint32 startVertex, uint32 startInstance);
		void   DrawIndexedInstanced(uint32 cmdListHandle, uint32 indexCountPerInstance, uint32 instanceCount, uint32 startIndexLocation, uint32 baseVertexLocation, uint32 startInstanceLocation);
		void   DrawIndexedIndirect(uint32 cmdListHandle, IGfxBufferResource* indirectBuffer, uint32 count, uint64 indirectOffset);
		void   SetTopology(uint32 cmdListHandle, Topology topology);
		void   PushTransferCommand(GfxCommand& cmd);
		void   BindVertexBuffer(uint32 cmdListHandle, IGfxBufferResource* buffer, uint32 slot = 0);
		void   BindIndexBuffer(uint32 cmdListHandle, IGfxBufferResource* buffer);
		void   CopyCPU2GPU(IGfxBufferResource* cpuBuffer, IGfxBufferResource* gpuBuffer, void* data, size_t sz, ResourceState finalState);
		void   WaitForCopyQueue();

		// Fences
		uint32 CreateFence();
		void   ReleaseFence(uint32 handle);
		void   WaitForFences(uint32 fence, uint64 userData0, uint64 userData1);

		// Textures
		Texture* CreateRenderTarget(const String& path);
		Texture* CreateRenderTarget(ISwapchain* swp, uint32 bufferIndex, const String& path);
		Texture* CreateDepthStencil(const String& pathName, const Vector2i& size);

		// ******************* DX12 INTERFACE *******************
		// ******************* DX12 INTERFACE *******************
		// ******************* DX12 INTERFACE *******************
		inline ID3D12Device* DX12GetDevice()
		{
			return m_device.Get();
		}

		inline IDXGIFactory4* DX12GetFactory()
		{
			return m_factory.Get();
		}

		inline ID3D12CommandQueue* DX12GetGraphicsQueue()
		{
			return m_graphicsQueue.Get();
		}

		inline ID3D12CommandQueue* DX12GetTransferQueue()
		{
			return m_copyQueue.Get();
		}

		inline D3D12MA::Allocator* DX12GetAllocator()
		{
			return m_dx12Allocator;
		}

		// ******************* DX12 INTERFACE *******************

	private:
		void GetHardwareAdapter(IDXGIFactory1* pFactory, IDXGIAdapter1** ppAdapter, PreferredGPUType gpuType);

	private:
		// General
		GfxManager*		m_gfxManager = nullptr;
		StatePerFrame	m_frames[FRAMES_IN_FLIGHT];
		uint64			m_fenceValueGraphics = 0;
		uint32			m_frameFenceGraphics = 0;
		HANDLE			m_fenceEventGraphics = NULL;
		DX12GpuUploader m_uploader;

		// Backend
		D3D12MA::Allocator*							   m_dx12Allocator = nullptr;
		Microsoft::WRL::ComPtr<IDXGIAdapter1>		   m_adapter	   = nullptr;
		Microsoft::WRL::ComPtr<ID3D12Device>		   m_device;
		Microsoft::WRL::ComPtr<ID3D12CommandQueue>	   m_graphicsQueue;
		Microsoft::WRL::ComPtr<ID3D12CommandQueue>	   m_copyQueue;
		Microsoft::WRL::ComPtr<IDXGIFactory4>		   m_factory;
		DX12DescriptorHeap							   m_rtvHeap;
		DX12DescriptorHeap							   m_bufferHeap;
		DX12DescriptorHeap							   m_dsvHeap;
		DX12DescriptorHeap							   m_samplerHeap;
		Microsoft::WRL::ComPtr<ID3D12RootSignature>	   m_rootSigStandard;
		Microsoft::WRL::ComPtr<ID3D12CommandSignature> m_commandSigStandard;

		// Resources
		IDList<GeneratedTexture>			m_textures;
		IDList<GeneratedShader>				m_shaders;
		IDList<GeneratedMaterial>			m_materials;
		Mutex								m_textureMtx;
		Mutex								m_shaderMtx;
		Mutex								m_materialMtx;
		Microsoft::WRL::ComPtr<IDxcLibrary> m_idxcLib;
		ID3DIncludeInterface				m_includeInterface;

		// API
		IDList<Microsoft::WRL::ComPtr<ID3D12CommandAllocator>>	   m_cmdAllocators;
		IDList<Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList4>> m_cmdLists;
		IDList<Microsoft::WRL::ComPtr<ID3D12Fence>>				   m_fences;
	};

} // namespace Lina

#endif