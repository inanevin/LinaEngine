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
#include "Graphics/Data/DescriptorHandle.hpp"
#include "Data/HashMap.hpp"
#include "Data/IDList.hpp"
#include "Data/HashSet.hpp"
#include "Graphics/Platform/DX12/Core/DX12Common.hpp"
#include "Graphics/Platform/DX12/Core/DX12UploadContext.hpp"
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
	class IGfxResourceCPU;
	class IGfxResourceTexture;
	class Color;
	class DX12GPUHeap;
	class DX12StagingHeap;
	class IUploadContext;
	class ResourceManager;
	class TextureSampler;
	class IWindow;

	class Renderer
	{
	public:
		Renderer(const SystemInitializationInfo& initInfo, GfxManager* gfxMan);

		struct StatePerFrame
		{
			uint64 storedFenceGraphics = 0;
		};

		// ******************* SYSTEM ******************* //
		// ******************* SYSTEM ******************* //
		// ******************* SYSTEM ******************* //
		void Shutdown();
		void WaitForPresentation(ISwapchain* swapchain);
		void BeginFrame(uint32 frameIndex);
		void EndFrame(uint32 frameIndex);
		void Join();
		bool Present(ISwapchain* swp);
		void ResetResources();
		void OnWindowResized(IWindow* window, StringID sid, const Recti& rect);
		void OnVsyncChanged(VsyncMode mode);

		inline bool IsOK()
		{
			return !m_failed;
		}

		inline uint32 GetCurrentFrameIndex()
		{
			return m_currentFrameIndex;
		}

		inline GfxManager* GetGfxManager()
		{
			return m_gfxManager;
		}

		inline IUploadContext* GetUploadContext()
		{
			return m_uploadContext;
		}

		// ******************* RESOURCES *******************
		// ******************* RESOURCES *******************
		// ******************* RESOURCES *******************
		void GenerateMaterial(Material* mat);
		void DestroyMaterial(Material* mat);
		void GeneratePipeline(Shader* shader);
		void DestroyPipeline(Shader* shader);
		void CompileShader(const char* path, const HashMap<ShaderStage, String>& stages, HashMap<ShaderStage, ShaderByteCode>& outCompiledCode);
		void GenerateImage(Texture* txt, ImageGenerateRequest req);
		void DestroyImage(Texture* txt);
		void GenerateSampler(TextureSampler* sampler);
		void DestroySampler(TextureSampler* sampler);

		// ******************* API *******************
		// ******************* API *******************
		// ******************* API *******************

		// Swapchain
		ISwapchain* CreateSwapchain(const Vector2i& size, IWindow* window, StringID sid);
		uint32		GetNextBackBuffer(ISwapchain* swp);

		// Resources
		IGfxResourceCPU*	 CreateCPUResource(size_t size, CPUResourceHint hint, const wchar_t* name = L"CPU Visible Resource");
		IGfxResourceGPU*	 CreateGPUResource(size_t size, GPUResourceType type, bool requireJoinBeforeUpdating, const wchar_t* name = L"GPU Resource");
		IGfxResourceTexture* CreateTextureResource(TextureResourceType type, Texture* texture);
		void				 DeleteCPUResource(IGfxResourceCPU* res);
		void				 DeleteGPUResource(IGfxResourceGPU* res);

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
		void   ResourceBarrier(uint32 cmdListHandle, ResourceTransition* transitions, uint32 count);
		void   BeginRenderPass(uint32 cmdListHandle, Texture* colorTexture);
		void   BeginRenderPass(uint32 cmdListHandle, Texture* colorTexture, Texture* depthStencil);
		void   EndRenderPass(uint32 cmdListHandle);
		void   BindPipeline(uint32 cmdListHandle, Shader* shader);
		void   BindMaterials(Material** materials, uint32 materialsSize);
		void   SetMaterialID(uint32 cmdListHandle, uint32 id);
		void   BindDynamicTextures(Texture** textures, uint32 texturesSize);
		void   BindUniformBuffer(uint32 cmdListHandle, uint32 bufferIndex, IGfxResourceCPU* buf);
		void   BindObjectBuffer(uint32 cmdListHandle, IGfxResourceGPU* res);
		void   BindVertexBuffer(uint32 cmdListHandle, IGfxResourceGPU* buffer, size_t vertexSize = sizeof(Vertex), uint32 slot = 0);
		void   BindIndexBuffer(uint32 cmdListHandle, IGfxResourceGPU* buffer);
		void   DrawInstanced(uint32 cmdListHandle, uint32 vertexCount, uint32 instanceCount, uint32 startVertex, uint32 startInstance);
		void   DrawIndexedInstanced(uint32 cmdListHandle, uint32 indexCountPerInstance, uint32 instanceCount, uint32 startIndexLocation, uint32 baseVertexLocation, uint32 startInstanceLocation);
		void   DrawIndexedIndirect(uint32 cmdListHandle, IGfxResourceCPU* indirectBuffer, uint32 count, uint64 indirectOffset);
		void   SetTopology(uint32 cmdListHandle, Topology topology);

		// Fences
		uint32 CreateFence();
		void   ReleaseFence(uint32 handle);
		void   WaitForFences(uint32 fence, uint64 frameFenceValue);

		// Textures
		Texture* CreateRenderTargetColor(const String& path, const Vector2i& size);
		Texture* CreateRenderTargetSwapchain(ISwapchain* swp, uint32 bufferIndex, const String& path);
		Texture* CreateRenderTargetDepthStencil(const String& pathName, const Vector2i& size);

		// ******************* DX12 INTERFACE *******************
		// ******************* DX12 INTERFACE *******************
		// ******************* DX12 INTERFACE *******************

		void DX12Exception(HrException e);

		inline ID3D12RootSignature* DX12GetRootSignatureStandard()
		{
			return m_rootSigStandard.Get();
		}

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

		inline DX12StagingHeap* DX12GetCPUBufferHeap()
		{
			return m_bufferHeap;
		}

		inline bool DX12IsTearingAllowed()
		{
			return m_allowTearing;
		}

		// ******************* DX12 INTERFACE *******************

	private:
		void GetHardwareAdapter(IDXGIFactory1* pFactory, IDXGIAdapter1** ppAdapter, PreferredGPUType gpuType);

	private:
		// General
		SystemInitializationInfo m_initInfo;
		GfxManager*				 m_gfxManager = nullptr;
		StatePerFrame			 m_frames[FRAMES_IN_FLIGHT];
		uint64					 m_fenceValueGraphics	  = 0;
		uint32					 m_frameFenceGraphics	  = 0;
		HANDLE					 m_fenceEventGraphics	  = NULL;
		uint32					 m_currentFrameIndex	  = 0;
		ResourceManager*		 m_resourceManager		  = nullptr;
		IUploadContext*			 m_uploadContext		  = nullptr;
		bool					 m_allowTearing			  = false;
		VsyncMode				 m_vsync				  = VsyncMode::None;
		uint32					 m_texturesHeapAllocCount = 0;
		uint32					 m_samplersHeapAllocCount = 0;
		bool					 m_failed				  = false;

		// Backend
		D3D12MA::Allocator*							   m_dx12Allocator = nullptr;
		Microsoft::WRL::ComPtr<IDXGIAdapter1>		   m_adapter	   = nullptr;
		Microsoft::WRL::ComPtr<ID3D12Device>		   m_device;
		Microsoft::WRL::ComPtr<ID3D12CommandQueue>	   m_graphicsQueue;
		Microsoft::WRL::ComPtr<ID3D12CommandQueue>	   m_copyQueue;
		Microsoft::WRL::ComPtr<IDXGIFactory4>		   m_factory;
		DX12GPUHeap*								   m_gpuBufferHeap[FRAMES_IN_FLIGHT]  = {nullptr};
		DX12GPUHeap*								   m_gpuSamplerHeap[FRAMES_IN_FLIGHT] = {nullptr};
		DX12StagingHeap*							   m_rtvHeap						  = nullptr;
		DX12StagingHeap*							   m_bufferHeap						  = nullptr;
		DX12StagingHeap*							   m_textureHeap					  = nullptr;
		DX12StagingHeap*							   m_dsvHeap						  = nullptr;
		DX12StagingHeap*							   m_samplerHeap					  = nullptr;
		Microsoft::WRL::ComPtr<ID3D12RootSignature>	   m_rootSigStandard;
		Microsoft::WRL::ComPtr<ID3D12CommandSignature> m_commandSigStandard;

		// Resources
		Microsoft::WRL::ComPtr<IDxcLibrary> m_idxcLib;
		ID3DIncludeInterface				m_includeInterface;

		// API
		IDList<Microsoft::WRL::ComPtr<ID3D12CommandAllocator>>	   m_cmdAllocators;
		IDList<Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList4>> m_cmdLists;
		IDList<Microsoft::WRL::ComPtr<ID3D12Fence>>				   m_fences;
	};

} // namespace Lina

#endif