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

#ifndef IGfxResourceCPU_HPP
#define IGfxResourceCPU_HPP

#include "Graphics/Core/CommonGraphics.hpp"

namespace Lina
{
	class Renderer;

	class IGfxResourceCPU
	{
	public:
		IGfxResourceCPU(Renderer* renderer, CPUResourceHint hint, size_t sz) : m_renderer(renderer), m_hint(hint), m_size(sz){};
		virtual ~IGfxResourceCPU() = default;

		virtual void   BufferData(const void* data, size_t sz)						 = 0;
		virtual void   BufferDataPadded(const void* data, size_t sz, size_t padding) = 0;
		virtual uint64 GetGPUPointer()												 = 0;

		inline size_t GetSize()
		{
			return m_size;
		}

	protected:
		Renderer*		m_renderer	 = nullptr;
		CPUResourceHint m_hint		 = CPUResourceHint::None;
		uint8*			m_mappedData = nullptr;
		size_t			m_size		 = 0;
	};
} // namespace Lina

#endif
