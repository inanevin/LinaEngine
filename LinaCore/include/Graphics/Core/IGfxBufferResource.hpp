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

#ifndef IGfxBufferResource_HPP
#define IGfxBufferResource_HPP

#include "Graphics/Core/CommonGraphics.hpp"

namespace Lina
{
	class IGfxBufferResource
	{
	public:
		IGfxBufferResource(){};
		IGfxBufferResource(BufferResourceType type, size_t sz) : m_type(type), m_size(sz){};
		IGfxBufferResource(BufferResourceType2 type, BufferResourceHint hint, size_t sz) : m_type2(type), m_hint(hint), m_size(sz){};
		virtual ~IGfxBufferResource() = default;

		virtual void   Recreate(const void* data, size_t sz)					 = 0;
		virtual void   Update(const void* data, size_t sz)						 = 0;
		virtual void   UpdatePadded(const void* data, size_t sz, size_t padding) = 0;
		virtual uint64 GetGPUPointer()											 = 0;

		inline size_t GetSize()
		{
			return m_size;
		}

	protected:
		BufferResourceType	m_type		 = BufferResourceType::UniformBuffer;
		BufferResourceType2 m_type2		 = BufferResourceType2::CPUResource;
		BufferResourceHint	m_hint		 = BufferResourceHint::None;
		uint8*				m_mappedData = nullptr;
		size_t				m_size		 = 0;
	};
} // namespace Lina

#endif
