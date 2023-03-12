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

#ifndef DX12GfxResource_HPP
#define DX12GfxResource_HPP

#include "Graphics/Core/IGfxBufferResource.hpp"
#include "Graphics/Platform/DX12/Core/DX12Common.hpp"

namespace D3D12MA
{
	class Allocation;
}

namespace Lina
{
	class Renderer;

	class DX12GfxBufferResource : public IGfxBufferResource
	{
	public:
		DX12GfxBufferResource(Renderer* rend, BufferResourceType type, void* initialData, size_t sz);
		virtual ~DX12GfxBufferResource();

		virtual void   Recreate(void* data, size_t sz) override;
		virtual void   Update(void* data, size_t sz) override;
		virtual uint64 GetGPUPointer() override;

		inline D3D12MA::Allocation* DX12GetAllocation()
		{
			return m_allocation;
		}

	private:
		void CreateGPUBuffer(void* data, size_t sz);
		void Cleanup();

	private:
		Renderer*			 m_renderer	  = nullptr;
		D3D12MA::Allocation* m_allocation = nullptr;
	};
} // namespace Lina

#endif
