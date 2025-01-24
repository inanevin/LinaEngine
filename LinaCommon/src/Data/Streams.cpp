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

#include "Common/Data/Streams.hpp"
#include "Common/Data/Vector.hpp"
#include "Common/Data/CommonData.hpp"
#include <algorithm>
#include <fstream>

namespace Lina
{

	void IStream::Create(size_t size)
	{
		Destroy();
		m_data	= new uint8[size];
		m_index = 0;
		m_size	= size;
	}

	void IStream::Create(uint8* data, size_t size)
	{
		Destroy();
		const size_t unaligned = size;
		m_data				   = new uint8[size];
		MEMCPY(m_data, data, unaligned);
		m_index = 0;
		m_size	= size;
	}

	void IStream::Destroy()
	{
		if (m_data == nullptr)
			return;

		delete[] m_data;

		m_index = 0;
		m_size	= 0;
		m_data	= nullptr;
	}

	void IStream::ReadFromIFStream(std::ifstream& stream)
	{
		stream.read((char*)m_data, m_size);
	}

	void IStream::ReadToRawEndianSafe(void* ptr, size_t size)
	{
		if (Endianness::ShouldSwap())
		{
			uint8*		  data = &m_data[m_index];
			Vector<uint8> v;
			v.insert(v.end(), data, data + size);

			Vector<uint8> v2;
			v2.resize(v.size());
			linatl::reverse_copy(v.begin(), v.end(), v2.begin());

			MEMCPY(ptr, v2.data(), size);

			v.clear();
			v2.clear();
		}
		else
			MEMCPY(ptr, &m_data[m_index], size);

		m_index += size;
	}

	void IStream::ReadToRaw(void* ptr, size_t size)
	{
		MEMCPY(ptr, &m_data[m_index], size);
		m_index += size;
	}

	void OStream::CreateReserve(size_t size)
	{
		m_data		  = new uint8[size];
		m_totalSize	  = size;
		m_currentSize = 0;
	}

	void OStream::Destroy()
	{
		delete[] m_data;

		m_currentSize = 0;
		m_totalSize	  = 0;
		m_data		  = nullptr;
	}

	void OStream::WriteRawEndianSafe(const uint8* ptr, size_t size)
	{
		if (m_data == nullptr)
			CreateReserve(size);

		CheckGrow(size);

		if (Endianness::ShouldSwap())
		{
			Vector<uint8> v;
			v.insert(v.end(), ptr, (ptr) + size);

			Vector<uint8> v2;
			v2.resize(v.size());
			linatl::reverse_copy(v.begin(), v.end(), v2.begin());
			MEMCPY(&m_data[m_currentSize], v2.data(), size);

			v.clear();
			v2.clear();
		}
		else
			MEMCPY(&m_data[m_currentSize], ptr, size);

		m_currentSize += size;
	}

	void OStream::WriteRaw(const uint8* ptr, size_t size)
	{
		if (m_data == nullptr)
			CreateReserve(size);

		CheckGrow(size);
		MEMCPY(&m_data[m_currentSize], ptr, size);
		m_currentSize += size;
	}

	void OStream::WriteTo(Span<uint8>& span)
	{
		span = {new uint8[m_currentSize], m_currentSize};
		MEMCPY(span.data(), GetDataRaw(), m_currentSize);
	}

	void OStream::CheckGrow(size_t sz)
	{
		if (m_currentSize + sz > m_totalSize)
		{
			m_totalSize	   = static_cast<size_t>((static_cast<float>(m_currentSize + sz) * 2.0f));
			uint8* newData = new uint8[m_totalSize];
			MEMCPY(newData, m_data, m_currentSize);
			delete[] m_data;
			m_data = newData;
		}
	}
	void OStream::WriteToOFStream(std::ofstream& stream)
	{
		stream.write((char*)m_data, m_currentSize);
	}

	void RawStream::Create(const Span<uint8>& sp)
	{
		Destroy();
		m_data = {new uint8[sp.size()], sp.size()};
		MEMCPY(m_data.data(), sp.data(), sp.size());
	}

	void RawStream::Create(uint8* data, size_t size)
	{
		Destroy();
		m_data = {new uint8[size], size};
		MEMCPY(m_data.data(), data, size);
	}

	void RawStream::Create(OStream& stream)
	{
		Destroy();
		m_data = {new uint8[stream.GetCurrentSize()], stream.GetCurrentSize()};
		MEMCPY(m_data.data(), stream.GetDataRaw(), stream.GetCurrentSize());
	}

	void RawStream::Destroy()
	{
		if (IsEmpty())
			return;
		delete[] m_data.data();
		m_data = {};
	}

	void RawStream::SaveToStream(OStream& stream) const
	{
		const uint32 sz = static_cast<uint32>(m_data.size());
		stream << sz;
		if (sz != 0)
			stream.WriteRaw(m_data);
	}

	void RawStream::LoadFromStream(IStream& stream)
	{
		uint32 size = 0;
		stream >> size;
		if (size != 0)
		{
			const size_t sz = static_cast<size_t>(size);
			Destroy();
			m_data = {new uint8[sz], sz};
			stream.ReadToRaw(m_data);
		}
	}

} // namespace Lina
