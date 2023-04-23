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

#include "Graphics/Platform/DX12/Core/DX12GfxContext.hpp"
#include "Graphics/Platform/DX12/Core/DX12Renderer.hpp"
#include "Graphics/Platform/DX12/Core/DX12ResourceTexture.hpp"
#include "Graphics/Platform/DX12/Core/DX12ResourceCPU.hpp"
#include "Graphics/Platform/DX12/Core/DX12ResourceGPU.hpp"
#include "Graphics/Platform/DX12/Core/DX12Pipeline.hpp"
#include "Graphics/Platform/DX12/Core/DX12GPUHeap.hpp"
#include "Graphics/Platform/DX12/SDK/D3D12MemAlloc.h"
#include "Graphics/Core/GfxManager.hpp"
#include "Graphics/Resource/Texture.hpp"
#include "Graphics/Resource/Shader.hpp"
#include "Graphics/Resource/Material.hpp"
#include "Math/Rect.hpp"
#include "System/ISystem.hpp"

using Microsoft::WRL::ComPtr;

namespace Lina
{
	DX12GfxContext::DX12GfxContext(Renderer* renderer, CommandType type) : IGfxContext(renderer, type), m_cmdAllocators(20, Microsoft::WRL::ComPtr<ID3D12CommandAllocator>()), m_cmdLists(50, Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList4>())
	{
		m_gfxManager = m_renderer->GetGfxManager();
		m_device	 = m_renderer->DX12GetDevice();

		D3D12_COMMAND_QUEUE_DESC queueDesc = {};
		queueDesc.Flags					   = D3D12_COMMAND_QUEUE_FLAG_NONE;
		queueDesc.Type					   = GetCommandType(m_contextType);
		ThrowIfFailed(m_device->CreateCommandQueue(&queueDesc, IID_PPV_ARGS(&m_queue)));

		NAME_DX12_OBJECT(m_queue, L"Context Queue");
	}

	DX12GfxContext::~DX12GfxContext()
	{
		m_queue.Reset();
	}

	uint32 DX12GfxContext::CreateCommandAllocator()
	{
		const uint32 handle = m_cmdAllocators.AddItem(ComPtr<ID3D12CommandAllocator>());
		auto&		 alloc	= m_cmdAllocators.GetItemR(handle);

		try
		{
			ThrowIfFailed(m_device->CreateCommandAllocator(GetCommandType(m_contextType), IID_PPV_ARGS(alloc.GetAddressOf())));
		}
		catch (HrException e)
		{
			LINA_CRITICAL("[Renderer] -> Exception when creating a command allocator! {0}", e.what());
			m_renderer->DX12Exception(e);
		}

		return handle;
	}

	uint32 DX12GfxContext::CreateCommandList(uint32 allocatorHandle)
	{
		const uint32 handle	   = m_cmdLists.AddItem(ComPtr<ID3D12GraphicsCommandList4>());
		auto&		 list	   = m_cmdLists.GetItemR(handle);
		auto&		 allocator = m_cmdAllocators.GetItemR(allocatorHandle);

		try
		{
			ThrowIfFailed(m_device->CreateCommandList(0, GetCommandType(m_contextType), allocator.Get(), nullptr, IID_PPV_ARGS(list.GetAddressOf())));
			ThrowIfFailed(list->Close());
		}
		catch (HrException e)
		{
			LINA_CRITICAL("[Renderer] -> Exception when creating a command list! {0}", e.what());
			m_renderer->DX12Exception(e);
		}

		return handle;
	}

	void DX12GfxContext::ReleaseCommandAllocator(uint32 handle)
	{
		m_cmdAllocators.GetItemR(handle).Reset();
		m_cmdAllocators.RemoveItem(handle);
	}

	void DX12GfxContext::ReleaseCommandList(uint32 handle)
	{
		m_cmdLists.GetItemR(handle).Reset();
		m_cmdLists.RemoveItem(handle);
	}

	void DX12GfxContext::ResetCommandListAndAllocator(uint32 cmdAllocatorHandle, uint32 cmdListHandle)
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
			m_renderer->DX12Exception(e);
		}
	}

	void DX12GfxContext::ResetCommandList(uint32 cmdAllocatorHandle, uint32 cmdListHandle)
	{
		auto& cmdList	   = m_cmdLists.GetItemR(cmdListHandle);
		auto& cmdAllocator = m_cmdAllocators.GetItemR(cmdAllocatorHandle);

		try
		{
			ThrowIfFailed(cmdList->Reset(cmdAllocator.Get(), nullptr));
		}
		catch (HrException e)
		{
			LINA_CRITICAL("[Renderer] -> Exception when resetting a command list! {0}", e.what());
			m_renderer->DX12Exception(e);
		}
	}

	void DX12GfxContext::ResetCommandAllocator(uint32 cmdAllocatorHandle)
	{
		auto& cmdAllocator = m_cmdAllocators.GetItemR(cmdAllocatorHandle);

		try
		{
			ThrowIfFailed(cmdAllocator->Reset());
		}
		catch (HrException e)
		{
			LINA_CRITICAL("[Renderer] -> Exception when resetting a command list! {0}", e.what());
			m_renderer->DX12Exception(e);
		}
	}

	void DX12GfxContext::PrepareCommandList(uint32 cmdListHandle, const Viewport& viewport, const Recti& scissors)
	{
		auto* bufferHeap  = m_renderer->DX12GetBufferHeap(m_renderer->GetCurrentFrameIndex());
		auto* samplerHeap = m_renderer->DX12GetSamplerHeap(m_renderer->GetCurrentFrameIndex());

		auto&				  cmdList = m_cmdLists.GetItemR(cmdListHandle);
		ID3D12DescriptorHeap* heaps[] = {bufferHeap->GetHeap(), samplerHeap->GetHeap()};
		cmdList->SetGraphicsRootSignature(m_renderer->DX12GetRootSignatureStandard());
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
			sc.right  = static_cast<LONG>(scissors.pos.x + scissors.size.x);
			sc.bottom = static_cast<LONG>(scissors.pos.y + scissors.size.y);

			cmdList->RSSetViewports(1, &vp);
			cmdList->RSSetScissorRects(1, &sc);
		}

		// Textures & samplers
		{
			cmdList->SetGraphicsRootDescriptorTable(GBB_TxtData, bufferHeap->GetHeapGPUStart());
			cmdList->SetGraphicsRootDescriptorTable(GBB_SamplerData, samplerHeap->GetHeapGPUStart());
			cmdList->SetGraphicsRootDescriptorTable(GBB_MatData, {bufferHeap->GetOffsetedHandle(m_renderer->DX12GetTexturesHeapAllocCount()).GetGPUHandle()});
		}
	}

	void DX12GfxContext::SetScissors(uint32 cmdListHandle, const Recti& scissors)
	{
		auto&	   cmdList = m_cmdLists.GetItemR(cmdListHandle);
		D3D12_RECT sc;
		sc.left	  = static_cast<LONG>(scissors.pos.x);
		sc.top	  = static_cast<LONG>(scissors.pos.y);
		sc.right  = static_cast<LONG>(scissors.pos.x + scissors.size.x);
		sc.bottom = static_cast<LONG>(scissors.pos.y + scissors.size.y);
		cmdList->RSSetScissorRects(1, &sc);
	}

	void DX12GfxContext::FinalizeCommandList(uint32 cmdListHandle)
	{
		auto& cmdList = m_cmdLists.GetItemR(cmdListHandle);

		try
		{
			ThrowIfFailed(cmdList->Close());
		}
		catch (HrException e)
		{
			LINA_CRITICAL("[Renderer] -> Exception when closing a command list! {0}", e.what());
			m_renderer->DX12Exception(e);
		}
	}

	void DX12GfxContext::ExecuteCommandLists(const Vector<uint32>& lists)
	{
		const UINT sz = static_cast<UINT>(lists.size());

		Vector<ID3D12CommandList*> _lists;

		for (UINT i = 0; i < sz; i++)
		{
			auto& lst = m_cmdLists.GetItemR(lists[i]);
			_lists.push_back(lst.Get());
		}

		ID3D12CommandList* const* data = _lists.data();
		m_queue->ExecuteCommandLists(sz, data);
	}

	void DX12GfxContext::ResourceBarrier(uint32 cmdListHandle, ResourceTransition* transitions, uint32 count)
	{
		auto&							 cmdList = m_cmdLists.GetItemR(cmdListHandle);
		Vector<CD3DX12_RESOURCE_BARRIER> barriers;

		for (uint32 i = 0; i < count; i++)
		{
			ResourceTransition&	 t	 = transitions[i];
			DX12ResourceTexture* res = static_cast<DX12ResourceTexture*>(t.texture->GetGfxResource());

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

	void DX12GfxContext::BeginRenderPass(uint32 cmdListHandle, Texture* colorTexture)
	{
		auto& cmdList = m_cmdLists.GetItemR(cmdListHandle);

		const float							 cc[]{DEFAULT_CLEAR_CLR.x, DEFAULT_CLEAR_CLR.y, DEFAULT_CLEAR_CLR.z, DEFAULT_CLEAR_CLR.w};
		CD3DX12_CLEAR_VALUE					 clearValue{GetFormat(DEFAULT_RT_FORMAT), cc};
		D3D12_RENDER_PASS_BEGINNING_ACCESS	 renderPassBeginningAccessClear{D3D12_RENDER_PASS_BEGINNING_ACCESS_TYPE_CLEAR, {clearValue}};
		D3D12_RENDER_PASS_ENDING_ACCESS		 renderPassEndingAccessPreserve{D3D12_RENDER_PASS_ENDING_ACCESS_TYPE_PRESERVE, {}};
		D3D12_RENDER_PASS_RENDER_TARGET_DESC renderPassRenderTargetDesc{colorTexture->GetDescriptor().GetCPUHandle(), renderPassBeginningAccessClear, renderPassEndingAccessPreserve};

		cmdList->BeginRenderPass(1, &renderPassRenderTargetDesc, nullptr, D3D12_RENDER_PASS_FLAG_NONE);
	}

	void DX12GfxContext::BeginRenderPass(uint32 cmdListHandle, Texture* colorTexture, Texture* depthStencilTexture)
	{
		auto& cmdList = m_cmdLists.GetItemR(cmdListHandle);

		const float			cc[]{DEFAULT_CLEAR_CLR.x, DEFAULT_CLEAR_CLR.y, DEFAULT_CLEAR_CLR.z, DEFAULT_CLEAR_CLR.w};
		CD3DX12_CLEAR_VALUE clearValue{GetFormat(DEFAULT_RT_FORMAT), cc};
		CD3DX12_CLEAR_VALUE clearDepth{GetFormat(DEFAULT_DEPTH_FORMAT), 1.0f, 0};

		D3D12_RENDER_PASS_BEGINNING_ACCESS	 colorBegin{D3D12_RENDER_PASS_BEGINNING_ACCESS_TYPE_CLEAR, {clearValue}};
		D3D12_RENDER_PASS_ENDING_ACCESS		 colorEnd{D3D12_RENDER_PASS_ENDING_ACCESS_TYPE_PRESERVE, {}};
		D3D12_RENDER_PASS_RENDER_TARGET_DESC colorDesc{colorTexture->GetDescriptor().GetCPUHandle(), colorBegin, colorEnd};

		D3D12_RENDER_PASS_BEGINNING_ACCESS	 depthBegin{D3D12_RENDER_PASS_BEGINNING_ACCESS_TYPE_CLEAR, {clearDepth}};
		D3D12_RENDER_PASS_ENDING_ACCESS		 depthEnd{D3D12_RENDER_PASS_ENDING_ACCESS_TYPE_PRESERVE, {}};
		D3D12_RENDER_PASS_DEPTH_STENCIL_DESC depthDesc{depthStencilTexture->GetDescriptor().GetCPUHandle(), depthBegin, depthBegin, depthEnd, depthEnd};

		cmdList->BeginRenderPass(1, &colorDesc, &depthDesc, D3D12_RENDER_PASS_FLAG_NONE);
	}

	void DX12GfxContext::EndRenderPass(uint32 cmdListHandle)
	{
		auto& cmdList = m_cmdLists.GetItemR(cmdListHandle);
		cmdList->EndRenderPass();
	}

	void DX12GfxContext::BindPipeline(uint32 cmdListHandle, Shader* shader)
	{
		auto&		  cmdList = m_cmdLists.GetItemR(cmdListHandle);
		DX12Pipeline* pl	  = static_cast<DX12Pipeline*>(shader->GetPipeline());
		cmdList->SetPipelineState(pl->GetPipelineState());
	}

	void DX12GfxContext::BindMaterials(Material** materials, uint32 materialsSize)
	{
		LOCK_GUARD(m_bindMtx);

		auto		 heap					= m_renderer->DX12GetBufferHeap(m_renderer->GetCurrentFrameIndex());
		const uint32 currentDescriptorIndex = heap->GetCurrentDescriptorIndex() - m_renderer->DX12GetTexturesHeapAllocCount();
		const uint32 heapIncrement			= heap->GetDescriptorSize();
		auto		 alloc					= heap->GetHeapHandleBlock(materialsSize);

		Vector<D3D12_CPU_DESCRIPTOR_HANDLE> srcDescriptors;
		Vector<D3D12_CPU_DESCRIPTOR_HANDLE> destDescriptors;
		srcDescriptors.resize(materialsSize, {});
		destDescriptors.resize(materialsSize, {});

		Taskflow tf;
		tf.for_each_index(0, static_cast<int>(materialsSize), 1, [&](int i) {
			Material* mat = materials[i];
			mat->SetBindlessIndex(currentDescriptorIndex + i);

			D3D12_CPU_DESCRIPTOR_HANDLE handle;
			handle.ptr		   = alloc.GetCPUHandle() + i * heapIncrement;
			destDescriptors[i] = handle;

			// Update if necessary
			{
				if (true || mat->IsDirty(m_renderer->GetCurrentFrameIndex()))
				{
					uint8* ptr = nullptr;
					size_t sz  = 0;
					mat->GetPropertyBlob(ptr, sz);
					mat->GetGfxResource(m_renderer->GetCurrentFrameIndex())->BufferData(ptr, sz);
					delete[] ptr;
					mat->SetIsDirty(m_renderer->GetCurrentFrameIndex(), false);
				}
			}
			srcDescriptors[i] = {mat->GetDescriptor(m_renderer->GetCurrentFrameIndex()).GetCPUHandle()};
		});

		m_gfxManager->GetSystem()->GetMainExecutor()->RunAndWait(tf);
		m_device->CopyDescriptors(materialsSize, destDescriptors.data(), NULL, materialsSize, srcDescriptors.data(), NULL, D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
	}

	void DX12GfxContext::SetMaterialID(uint32 cmdListHandle, uint32 id)
	{
		auto& cmdList = m_cmdLists.GetItemR(cmdListHandle);
		cmdList->SetGraphicsRoot32BitConstant(GBB_IndirectData, id, 1);
	}

	void DX12GfxContext::BindDynamicTextures(Texture** textures, uint32 texturesSize)
	{
		LOCK_GUARD(m_bindMtx);
		auto		 heap					= m_renderer->DX12GetBufferHeap(m_renderer->GetCurrentFrameIndex());
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

			texture->SetBindlessIndex(currentDescriptorIndex + i);

			D3D12_CPU_DESCRIPTOR_HANDLE handle;
			handle.ptr		   = alloc.GetCPUHandle() + i * heapIncrement;
			destDescriptors[i] = handle;

			srcDescriptors[i] = {texture->GetResourceType() == TextureResourceType::Texture2DDefaultDynamic ? texture->GetDescriptor().GetCPUHandle() : texture->GetDescriptorSecondary().GetCPUHandle()};

			if (texture->GetResourceType() != TextureResourceType::Texture2DRenderTargetColor && texture->GetResourceType() != TextureResourceType::Texture2DDefaultDynamic)
			{
				LINA_ERR("[Renderer] -> You should only bind dynamic textures or color render targets via BindDynamicTextures()!");
			}
		});

		m_gfxManager->GetSystem()->GetMainExecutor()->RunAndWait(tf);
		m_device->CopyDescriptors(texturesSize, destDescriptors.data(), NULL, texturesSize, srcDescriptors.data(), NULL, D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
	}

	void DX12GfxContext::BindUniformBuffer(uint32 cmdListHandle, uint32 bufferIndex, IGfxResourceCPU* buf)
	{
		auto& cmdList = m_cmdLists.GetItemR(cmdListHandle);
		cmdList->SetGraphicsRootConstantBufferView(bufferIndex, buf->GetGPUPointer());
	}

	void DX12GfxContext::BindObjectBuffer(uint32 cmdListHandle, IGfxResourceGPU* res)
	{
		auto& cmdList = m_cmdLists.GetItemR(cmdListHandle);
		cmdList->SetGraphicsRootShaderResourceView(GBB_ObjData, res->GetGPUPointer());
	}

	void DX12GfxContext::BindVertexBuffer(uint32 cmdListHandle, IGfxResourceGPU* buffer, size_t vertexSize, uint32 slot, size_t maxSize)
	{
		auto&					 cmdList = m_cmdLists.GetItemR(cmdListHandle);
		D3D12_VERTEX_BUFFER_VIEW view;
		view.BufferLocation = buffer->GetGPUPointer();
		view.SizeInBytes	= maxSize == 0 ? static_cast<UINT>(buffer->GetSize()) : static_cast<UINT>(maxSize);
		view.StrideInBytes	= static_cast<UINT>(vertexSize);
		cmdList->IASetVertexBuffers(slot, 1, &view);
	}

	void DX12GfxContext::BindIndexBuffer(uint32 cmdListHandle, IGfxResourceGPU* buffer, size_t maxSize)
	{
		auto&					cmdList = m_cmdLists.GetItemR(cmdListHandle);
		D3D12_INDEX_BUFFER_VIEW view;
		view.BufferLocation = buffer->GetGPUPointer();
		view.SizeInBytes	= maxSize == 0 ? static_cast<UINT>(buffer->GetSize()) : static_cast<UINT>(maxSize);
		view.Format			= DXGI_FORMAT_R32_UINT;
		cmdList->IASetIndexBuffer(&view);
	}

	void DX12GfxContext::DrawInstanced(uint32 cmdListHandle, uint32 vertexCount, uint32 instanceCount, uint32 startVertex, uint32 startInstance)
	{
		auto& cmdList = m_cmdLists.GetItemR(cmdListHandle);
		cmdList->DrawInstanced(vertexCount, instanceCount, startVertex, startInstance);
	}

	void DX12GfxContext::DrawIndexedInstanced(uint32 cmdListHandle, uint32 indexCountPerInstance, uint32 instanceCount, uint32 startIndexLocation, uint32 baseVertexLocation, uint32 startInstanceLocation)
	{
		auto& cmdList = m_cmdLists.GetItemR(cmdListHandle);
		cmdList->DrawIndexedInstanced(indexCountPerInstance, instanceCount, startIndexLocation, baseVertexLocation, startInstanceLocation);
	}

	void DX12GfxContext::DrawIndexedIndirect(uint32 cmdListHandle, IGfxResourceCPU* indirectBuffer, uint32 count, uint64 indirectOffset)
	{
		auto&			 cmdList = m_cmdLists.GetItemR(cmdListHandle);
		DX12ResourceCPU* buf	 = static_cast<DX12ResourceCPU*>(indirectBuffer);
		cmdList->ExecuteIndirect(m_renderer->DX12GetCommandSignatureStandard(), count, buf->DX12GetAllocation()->GetResource(), indirectOffset, nullptr, 0);
	}

	void DX12GfxContext::SetTopology(uint32 cmdListHandle, Topology topology)
	{
		auto& cmdList = m_cmdLists.GetItemR(cmdListHandle);
		cmdList->IASetPrimitiveTopology(GetTopology(topology));
	}

	void DX12GfxContext::WaitForFences(uint32 fenceHandle, uint64 frameFenceValue)
	{
		auto&		 fences	   = m_renderer->DX12GetFences();
		auto&		 fence	   = fences.GetItemR(fenceHandle);
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
	void DX12GfxContext::Signal(uint32 fenceHandle, uint64 frameFenceValue)
	{
		auto& fences = m_renderer->DX12GetFences();
		auto& fence	 = fences.GetItemR(fenceHandle);
		m_queue->Signal(fence.Get(), frameFenceValue);
	}

	void DX12GfxContext::Wait(uint32 fenceHandle, uint64 frameFenceValue)
	{
		auto& fences = m_renderer->DX12GetFences();
		auto& fence	 = fences.GetItemR(fenceHandle);
		m_queue->Wait(fence.Get(), frameFenceValue);
	}

	void DX12GfxContext::Copy(IGfxResourceCPU* staging, IGfxResourceGPU* destination, uint32 cmdListHandle)
	{
		auto&			 cmdList		= m_cmdLists.GetItemR(cmdListHandle);
		DX12ResourceCPU* dx12ResStaging = static_cast<DX12ResourceCPU*>(staging);
		DX12ResourceGPU* dx12ResTarget	= static_cast<DX12ResourceGPU*>(destination);
		cmdList->CopyResource(dx12ResTarget->DX12GetAllocation()->GetResource(), dx12ResStaging->DX12GetAllocation()->GetResource());
	}
} // namespace Lina
