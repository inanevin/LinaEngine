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

#ifndef DX12GpuUploader_HPP
#define DX12GpuUploader_HPP

#include "Data/Functional.hpp"
#include "Data/Queue.hpp"
#include "Graphics/Platform/DX12/Core/DX12Common.hpp"
#include "Graphics/Core/CommonGraphics.hpp"
#include "Data/Mutex.hpp"

namespace Lina
{
	class Renderer;

	class DX12GpuUploader
	{
	public:
		DX12GpuUploader(Renderer* rend) : m_renderer(rend){};
		~DX12GpuUploader() = default;

		void Initialize();
		void Shutdown();
		void Flush();
		void PushCommand(const GfxCommand& cmd);
		void WaitForFence();

	private:
		Renderer*							m_renderer = nullptr;
		Mutex								m_mtx;
		Microsoft::WRL::ComPtr<ID3D12Fence> m_fence;
		HANDLE								m_fenceEvent = NULL;
		uint64								m_fenceValue = 0;
		uint32								m_allocator	 = 0;
		uint32								m_cmdList	 = 0;
		Vector<GfxCommand>					m_waitingUploads;
	};
} // namespace Lina

#endif