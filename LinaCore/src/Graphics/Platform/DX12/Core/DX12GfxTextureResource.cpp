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
#include "Graphics/Platform/DX12/Core/DX12Renderer.hpp"
#include "Graphics/Platform/DX12/SDK/D3D12MemAlloc.h"

namespace Lina
{
	DX12GfxTextureResource::DX12GfxTextureResource(Renderer* rend, TextureResourceType type, const Vector2i& initialSize) : m_renderer(rend), IGfxTextureResource(type, initialSize)
	{
	}

	DX12GfxTextureResource::~DX12GfxTextureResource()
	{
		Cleanup();
	}

	uint64 DX12GfxTextureResource::GetGPUPointer()
	{
		return m_allocation->GetResource()->GetGPUVirtualAddress();
	}

	void DX12GfxTextureResource::CreateDepthStencil(const Vector2i& size)
	{
		D3D12_RESOURCE_DESC resourceDesc = {};
		resourceDesc.Dimension			 = D3D12_RESOURCE_DIMENSION_TEXTURE2D;
		resourceDesc.Alignment			 = 0;
		resourceDesc.Width				 = size.x;
		resourceDesc.Height				 = size.y;
		resourceDesc.DepthOrArraySize	 = 1;
		resourceDesc.MipLevels			 = 0;
		resourceDesc.SampleDesc.Count	 = 1;
		resourceDesc.SampleDesc.Quality	 = 0;
		resourceDesc.Format				 = GetFormat(DEFAULT_DEPTH_FORMAT);
		resourceDesc.Layout				 = D3D12_TEXTURE_LAYOUT_UNKNOWN;
		resourceDesc.Flags				 = D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL | D3D12_RESOURCE_FLAG_DENY_SHADER_RESOURCE;

		D3D12MA::ALLOCATION_DESC allocationDesc = {};
		allocationDesc.HeapType					= D3D12_HEAP_TYPE_DEFAULT;
		ThrowIfFailed(m_renderer->DX12GetAllocator()->CreateResource(&allocationDesc, &resourceDesc, D3D12_RESOURCE_STATE_DEPTH_WRITE, NULL, &m_allocation, IID_NULL, NULL));
	}

	void DX12GfxTextureResource::Cleanup()
	{
		m_allocation->Release();
		m_allocation = nullptr;
	}
} // namespace Lina
