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

#ifndef D3D12GfxContext_HPP
#define D3D12GfxContext_HPP

#include "Graphics/Interfaces/IGfxContext.hpp"
#include "Graphics/Platform/DX12/Core/DX12Common.hpp"
#include "Data/IDList.hpp"
#include "Data/Mutex.hpp"

namespace Lina
{
	class Renderer;

	class DX12GfxContext : public IGfxContext
	{
	public:
		DX12GfxContext(Renderer* renderer, CommandType type);
		virtual ~DX12GfxContext();

		virtual uint32 CreateCommandAllocator() override;
		virtual uint32 CreateCommandList(uint32 allocator) override;
		virtual void   ReleaseCommandAllocator(uint32 allocator) override;
		virtual void   ReleaseCommandList(uint32 handle) override;
		virtual void   ResetCommandListAndAllocator(uint32 cmdAllocatorHandle, uint32 cmdListHandle) override;
		virtual void   ResetCommandList(uint32 cmdAllocatorHandle, uint32 cmdListHandle) override;
		virtual void   ResetCommandAllocator(uint32 cmdAllocatorHandle) override;
		virtual void   PrepareCommandList(uint32 cmdListHandle, const Viewport& viewport, const Recti& scissors) override;
		virtual void   SetScissors(uint32 cmdListHandle, const Recti& scissors) override;
		virtual void   ExecuteCommandLists(const Vector<uint32>& lists) override;
		virtual void   ResourceBarrier(uint32 cmdListHandle, ResourceTransition* transitions, uint32 count) override;
		virtual void   BeginRenderPass(uint32 cmdListHandle, Texture* colorTexture) override;
		virtual void   BeginRenderPass(uint32 cmdListHandle, Texture* colorTexture, Texture* depthStencil) override;
		virtual void   EndRenderPass(uint32 cmdListHandle) override;
		virtual void   BindPipeline(uint32 cmdListHandle, Shader* shader) override;
		virtual void   BindMaterials(Material** materials, uint32 materialsSize) override;
		virtual void   SetMaterialID(uint32 cmdListHandle, uint32 id) override;
		virtual void   BindDynamicTextures(Texture** textures, uint32 texturesSize) override;
		virtual void   BindUniformBuffer(uint32 cmdListHandle, uint32 bufferIndex, IGfxResourceCPU* buf) override;
		virtual void   BindObjectBuffer(uint32 cmdListHandle, IGfxResourceGPU* res) override;
		virtual void   BindVertexBuffer(uint32 cmdListHandle, IGfxResourceGPU* buffer, size_t vertexSize, uint32 slot, size_t maxSize) override;
		virtual void   BindIndexBuffer(uint32 cmdListHandle, IGfxResourceGPU* buffer, size_t maxSize) override;
		virtual void   DrawInstanced(uint32 cmdListHandle, uint32 vertexCount, uint32 instanceCount, uint32 startVertex, uint32 startInstance) override;
		virtual void   DrawIndexedInstanced(uint32 cmdListHandle, uint32 indexCountPerInstance, uint32 instanceCount, uint32 startIndexLocation, uint32 baseVertexLocation, uint32 startInstanceLocation) override;
		virtual void   DrawIndexedIndirect(uint32 cmdListHandle, IGfxResourceCPU* indirectBuffer, uint32 count, uint64 indirectOffset) override;
		virtual void   SetTopology(uint32 cmdListHandle, Topology topology) override;
		virtual void   WaitForFences(uint32 fenceHandle, uint64 frameFenceValue) override;
		virtual void   Signal(uint32 fenceHandle, uint64 frameFenceValue) override;
		virtual void   Wait(uint32 fenceHandle, uint64 frameFenceValue) override;

		virtual void Copy(IGfxResourceCPU* staging, IGfxResourceGPU* destination, uint32 cmdList) override;

		inline ID3D12CommandQueue* GetQueue() const
		{
			return m_queue.Get();
		}

	private:
		Microsoft::WRL::ComPtr<ID3D12CommandQueue>				   m_queue;
		GfxManager*												   m_gfxManager = nullptr;
		Mutex													   m_bindMtx;
		ID3D12Device*											   m_device = nullptr;
		IDList<Microsoft::WRL::ComPtr<ID3D12CommandAllocator>>	   m_cmdAllocators;
		IDList<Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList4>> m_cmdLists;
		uint64													   m_fenceValue = 0;
	};
} // namespace Lina

#endif