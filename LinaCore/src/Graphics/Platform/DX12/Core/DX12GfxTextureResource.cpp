/*
This file is a part of: Lina Engine
https://github.com/inanevin/LinaEngine

Author: Inan Evin
http://www.inanevin.com

Copyright (c) [2018-] [Inan Evin]

Permission is hereby granted: free of charge: to any person obtaining a copy
of this software and associated documentation files (the "Software"): to deal
in the Software without restriction: including without limitation the rights
to use: copy: modify: merge: publish: distribute: sublicense: and/or sell
copies of the Software: and to permit persons to whom the Software is
furnished to do so: subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS": WITHOUT WARRANTY OF ANY KIND: EXPRESS OR
IMPLIED: INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY:
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM: DAMAGES OR OTHER
LIABILITY: WHETHER IN AN ACTION OF CONTRACT: TORT OR OTHERWISE: ARISING FROM:
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
*/

#include "Graphics/Platform/DX12/Core/DX12GfxTextureResource.hpp"
#include "Graphics/Platform/DX12/Core/DX12StagingHeap.hpp"
#include "Graphics/Platform/DX12/Core/DX12Renderer.hpp"
#include "Graphics/Platform/DX12/SDK/D3D12MemAlloc.h"
#include "Graphics/Resource/Texture.hpp"

namespace Lina
{
	DX12GfxTextureResource::DX12GfxTextureResource(Renderer* rend, Texture* txt, TextureResourceType type) : m_renderer(rend), IGfxTextureResource(type, Vector2i(txt->GetExtent().width, txt->GetExtent().height))
	{
		m_texture = txt;
		CreateTexture();
	}

	DX12GfxTextureResource::~DX12GfxTextureResource()
	{
		Cleanup();
	}

	uint64 DX12GfxTextureResource::GetGPUPointer()
	{
		return m_allocation->GetResource()->GetGPUVirtualAddress();
	}

	void DX12GfxTextureResource::CreateTexture()
	{
		const auto&	   ext	= m_texture->GetExtent();
		auto&		   meta = m_texture->GetMetadata();
		const Vector2i size = Vector2i(ext.width, ext.height);

		D3D12_RESOURCE_DESC resourceDesc = {};
		resourceDesc.Dimension			 = D3D12_RESOURCE_DIMENSION_TEXTURE2D;
		resourceDesc.Alignment			 = 0;
		resourceDesc.Width				 = size.x;
		resourceDesc.Height				 = size.y;
		resourceDesc.DepthOrArraySize	 = 1;
		resourceDesc.MipLevels			 = 1;
		resourceDesc.SampleDesc.Count	 = 1;
		resourceDesc.SampleDesc.Quality	 = 0;
		resourceDesc.Layout				 = D3D12_TEXTURE_LAYOUT_UNKNOWN;
		resourceDesc.Flags				 = D3D12_RESOURCE_FLAG_NONE;
		resourceDesc.Format				 = GetFormat(static_cast<Format>(meta.GetUInt8("Format"_hs)));

		D3D12MA::ALLOCATION_DESC allocationDesc = {};
		allocationDesc.HeapType					= D3D12_HEAP_TYPE_DEFAULT;

		D3D12_RESOURCE_STATES state		 = D3D12_RESOURCE_STATE_COMMON;
		D3D12_CLEAR_VALUE*	  clear		 = NULL;
		auto				  depthClear = CD3DX12_CLEAR_VALUE(GetFormat(DEFAULT_DEPTH_FORMAT), 1.0f, 0);

		if (m_type == TextureResourceType::Texture2DDepthStencil)
		{
			resourceDesc.Flags = D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL | D3D12_RESOURCE_FLAG_DENY_SHADER_RESOURCE;
			state			   = D3D12_RESOURCE_STATE_DEPTH_WRITE;
			clear			   = &depthClear;
		}
		else if (m_type == TextureResourceType::Texture2DDefaultGPU)
		{
			state = D3D12_RESOURCE_STATE_COPY_DEST;
			// SKIP MIPS FOR NOW
		}

		ThrowIfFailed(m_renderer->DX12GetAllocator()->CreateResource(&allocationDesc, &resourceDesc, state, clear, &m_allocation, IID_NULL, NULL));
	}

	void DX12GfxTextureResource::Cleanup()
	{
		m_allocation->Release();
		m_allocation = nullptr;
	}
} // namespace Lina
