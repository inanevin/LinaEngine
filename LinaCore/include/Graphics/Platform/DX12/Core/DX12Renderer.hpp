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
	class IGfxResource;
	class Color;

	struct GeneratedTexture
	{
		Microsoft::WRL::ComPtr<ID3D12Resource> resource;
		DescriptorHandle					   descriptor;
		ImageType							   imageType = ImageType::Color;
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
		struct State
		{
			State()
				: textures(100, GeneratedTexture()), materials(100, GeneratedMaterial()), shaders(100, GeneratedShader()), cmdAllocators(20, Microsoft::WRL::ComPtr<ID3D12CommandAllocator>()), cmdLists(50, Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList4>()),
				  fences(10, Microsoft::WRL::ComPtr<ID3D12Fence>()){};

			struct StatePerFrame
			{
				uint64 storedFenceGraphics = 0;
			};

			// General
			GfxManager*		gfxManager = nullptr;
			StatePerFrame	frames[FRAMES_IN_FLIGHT];
			uint64			fenceValueGraphics = 0;
			uint32			frameFenceGraphics = 0;
			HANDLE			fenceEventGraphics = NULL;
			DX12GpuUploader uploader;

			// Backend
			D3D12MA::Allocator*							   dx12Allocator = nullptr;
			Microsoft::WRL::ComPtr<IDXGIAdapter1>		   adapter		 = nullptr;
			Microsoft::WRL::ComPtr<ID3D12Device>		   device;
			Microsoft::WRL::ComPtr<ID3D12CommandQueue>	   graphicsQueue;
			Microsoft::WRL::ComPtr<ID3D12CommandQueue>	   copyQueue;
			Microsoft::WRL::ComPtr<IDXGIFactory4>		   factory;
			DX12DescriptorHeap							   rtvHeap;
			DX12DescriptorHeap							   bufferHeap;
			DX12DescriptorHeap							   dsvHeap;
			DX12DescriptorHeap							   samplerHeap;
			Microsoft::WRL::ComPtr<ID3D12RootSignature>	   rootSigStandard;
			Microsoft::WRL::ComPtr<ID3D12CommandSignature> commandSigStandard;

			// Resources
			IDList<GeneratedTexture>			textures;
			IDList<GeneratedShader>				shaders;
			IDList<GeneratedMaterial>			materials;
			Mutex								textureMtx;
			Mutex								shaderMtx;
			Mutex								materialMtx;
			Microsoft::WRL::ComPtr<IDxcLibrary> idxcLib;
			ID3DIncludeInterface				includeInterface;

			// API
			IDList<Microsoft::WRL::ComPtr<ID3D12CommandAllocator>>	   cmdAllocators;
			IDList<Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList4>> cmdLists;
			IDList<Microsoft::WRL::ComPtr<ID3D12Fence>>				   fences;
		};

		// ******************* SYSTEM ******************* //
		// ******************* SYSTEM ******************* //
		// ******************* SYSTEM ******************* //
		static void PreInitialize(const SystemInitializationInfo& initInfo, GfxManager* gfxMan);
		static void Initialize(const SystemInitializationInfo& initInfo);
		static void Shutdown();

		// ******************* RESOURCES *******************
		// ******************* RESOURCES *******************
		// ******************* RESOURCES *******************
		static uint32 GenerateMaterial(Material* mat, uint32 existingHandle);
		static void	  UpdateMaterialProperties(Material* mat, uint32 imageIndex);
		static void	  UpdateMaterialTextures(Material* mat, uint32 imageIndex, const Vector<uint32>& dirtyTextures);
		static void	  DestroyMaterial(uint32 handle);
		static uint32 GeneratePipeline(Shader* shader);
		static void	  DestroyPipeline(uint32 handle);
		static void	  CompileShader(const char* path, const HashMap<ShaderStage, String>& stages, HashMap<ShaderStage, ShaderByteCode>& outCompiledCode);
		static uint32 GenerateImage(Texture* txt, ImageType type);
		static uint32 GenerateImageAndUpload(Texture* txt);
		static void	  DestroyImage(uint32 handle);

		// ******************* API *******************
		// ******************* API *******************
		// ******************* API *******************

		// Frame Control
		static void BeginFrame(uint32 frameIndex);
		static void EndFrame(uint32 frameIndex);
		static void WaitForGPUGraphics();

		// Swapchain
		static ISwapchain* CreateSwapchain(const Vector2i& size, void* windowHandle);
		static void		   Present(ISwapchain* swp);
		static uint32	   GetNextBackBuffer(ISwapchain* swp);

		// Resources
		static IGfxResource* CreateBufferResource(ResourceMemoryState memState, ResourceState resState, void* initialData, size_t size);
		static IGfxResource* CreateIndirectBufferResource();
		static void			 DeleteBufferResource(IGfxResource* res);

		// Commands
		static uint32 CreateCommandAllocator(CommandType type);
		static uint32 CreateCommandList(CommandType type, uint32 allocatorHandle);
		static void	  ReleaseCommanAllocator(uint32 handle);
		static void	  ReleaseCommandList(uint32 handle);
		static void	  ResetCommandList(uint32 cmdAllocatorHandle, uint32 cmdListHandle);
		static void	  PrepareCommandList(uint32 cmdListHandle, const Viewport& viewport, const Recti& scissors);
		static void	  FinalizeCommandList(uint32 cmdListHandle);
		static void	  ExecuteCommandListsGraphics(const Vector<uint32>& lists);
		static void	  ExecuteCommandListsTransfer(const Vector<uint32>& lists);
		static void	  TransitionPresent2RT(uint32 cmdListHandle, Texture* txt);
		static void	  TransitionRT2Present(uint32 cmdListHandle, Texture* txt);
		static void	  BeginRenderPass(uint32 cmdListHandle, Texture* colorTexture, const Color& clearColor);
		static void	  EndRenderPass(uint32 cmdListHandle);
		static void	  BindUniformBuffer(uint32 cmdListHandle, uint32 bufferIndex, IGfxResource* buf);
		static void	  BindObjectBuffer(uint32 cmdListHandle, IGfxResource* buf);
		static void	  BindMaterial(uint32 cmdListHandle, Material* mat);
		static void	  DrawInstanced(uint32 cmdListHandle, uint32 vertexCount, uint32 instanceCount, uint32 startVertex, uint32 startInstance);
		static void	  DrawIndexedInstanced(uint32 cmdListHandle, uint32 indexCountPerInstance, uint32 instanceCount, uint32 startIndexLocation, uint32 baseVertexLocation, uint32 startInstanceLocation);
		static void	  DrawIndexedIndirect(uint32 cmdListHandle, IGfxResource* indirectBuffer, uint32 count, uint64 indirectOffset);
		static void	  SetTopology(uint32 cmdListHandle, Topology topology);
		static void	  PushTransferCommand(GfxCommand& cmd);
		static void	  BindVertexBuffer(uint32 cmdListHandle, IGfxResource* buffer, uint32 slot = 0);
		static void	  BindIndexBuffer(uint32 cmdListHandle, IGfxResource* buffer);
		static void	  CopyCPU2GPU(IGfxResource* cpuBuffer, IGfxResource* gpuBuffer, void* data, size_t sz, ResourceState finalState);
		static void	  WaitForCopyQueue();

		// Fences
		static uint32 CreateFence();
		static void	  ReleaseFence(uint32 handle);
		static void	  WaitForFences(uint32 fence, uint64 userData0, uint64 userData1);

		// Textures
		static Texture* CreateRenderTarget(const String& path);
		static Texture* CreateRenderTarget(ISwapchain* swp, uint32 bufferIndex, const String& path);

		// ******************* DX12 INTERFACE *******************
		// ******************* DX12 INTERFACE *******************
		// ******************* DX12 INTERFACE *******************
		static inline ID3D12Device* DX12GetDevice()
		{
			return s_state.device.Get();
		}

		static inline IDXGIFactory4* DX12GetFactory()
		{
			return s_state.factory.Get();
		}

		static inline ID3D12CommandQueue* DX12GetGraphicsQueue()
		{
			return s_state.graphicsQueue.Get();
		}

		static inline ID3D12CommandQueue* DX12GetTransferQueue()
		{
			return s_state.copyQueue.Get();
		}

		static inline D3D12MA::Allocator* DX12GetAllocator()
		{
			return s_state.dx12Allocator;
		}

		// ******************* DX12 INTERFACE *******************

	private:
		static void GetHardwareAdapter(IDXGIFactory1* pFactory, IDXGIAdapter1** ppAdapter, PreferredGPUType gpuType);

	private:
		static State s_state;
	};

} // namespace Lina

#endif