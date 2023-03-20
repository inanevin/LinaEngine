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

#ifndef DX12GPUResource_HPP
#define DX12GPUResource_HPP

#include "Graphics/Core/IGfxGPUResource.hpp"
#include "Graphics/Core/CommonGraphics.hpp"

namespace D3D12MA
{
	class Allocation;
}

namespace Lina
{
	class DX12CPUResource;

	class DX12GPUResource : public IGfxGPUResource
	{
	public:
		DX12GPUResource(Renderer* renderer, GPUResourceType type, bool requireJoinBeforeUpdating, size_t sz, const wchar_t* name = L"DX12 CPU Resource");
		virtual ~DX12GPUResource();

		virtual uint64 GetGPUPointer() override;
		virtual void   BufferData(const void* data, size_t sz, size_t padding, CopyDataType copyType) override;
		virtual void   Copy(CopyDataType copyType) override;

		inline size_t GetSize()
		{
			return m_size;
		}

		inline D3D12MA::Allocation* DX12GetAllocation()
		{
			return m_allocation;
		}

	private:
		void CreateResource();
		void Cleanup();

	private:
		const wchar_t*		 m_name			   = L"";
		DX12CPUResource*	 m_stagingResource = nullptr;
		D3D12MA::Allocation* m_allocation	   = nullptr;
	};
} // namespace Lina

#endif
