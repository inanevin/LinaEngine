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

#include "Common/SizeDefinitions.hpp"
#include "Common/Serialization/Endianness.hpp"
#include "Common/Memory/Memory.hpp"
#include "Common/StringID.hpp"
#include "Common/Data/CommonData.hpp"
#include "Common/Data/HashMap.hpp"
#include "Common/Data/String.hpp"
#include "Common/Data/Vector.hpp"
#include <type_traits>
#include <iosfwd>

namespace Lina
{

	class IStream
	{
	public:
		void Create(size_t size);
		void Create(uint8* data, size_t size);
		void Create(const Span<uint8>& span)
		{
			Create(span.data(), span.size());
		}
		void Destroy();
		void ReadFromIFStream(std::ifstream& stream);
		void ReadToRawEndianSafe(void* ptr, size_t size);
		void ReadToRaw(void* ptr, size_t size);
		void ReadToRaw(const Span<uint8>& span)
		{
			ReadToRaw(span.data(), span.size());
		}

		template <typename T> void Read(T& t)
		{
			MEMCPY(reinterpret_cast<uint8*>(&t), &m_data[m_index], sizeof(T));
			m_index += sizeof(T);
		}

		inline void SkipBy(size_t size)
		{
			m_index += size;
		}

		inline void Seek(size_t ind)
		{
			m_index = ind;
		}

		inline bool IsCompleted()
		{
			return m_index >= m_size;
		}

		inline size_t GetSize() const
		{
			return m_size;
		}

		inline bool Empty() const
		{
			return m_size == 0;
		}

		inline uint8* GetDataRaw() const
		{
			return m_data;
		}

		inline uint8* GetDataCurrent()
		{
			return &m_data[m_index];
		}

		inline void Shrink(size_t size)
		{
			m_size = size;
		}

	private:
		uint8* m_data  = nullptr;
		size_t m_index = 0;
		size_t m_size  = 0;
	};

	// Helper traits to detect vector and hashmap
	template <typename T> struct is_vector : std::false_type
	{
	};

	template <typename U> struct is_vector<std::vector<U>> : std::true_type
	{
	};

	template <typename U> struct is_vector<const std::vector<U>> : std::true_type
	{
	};

	template <typename T> inline constexpr bool is_vector_v = is_vector<T>::value;

	template <typename T> struct is_hashmap : std::false_type
	{
	};

	template <typename U, typename V> struct is_hashmap<HashMap<U, V>> : std::true_type
	{
	};

	template <typename U, typename V> struct is_hashmap<const HashMap<U, V>> : std::true_type
	{
	};

	template <typename T> inline constexpr bool is_hashmap_v = is_hashmap<T>::value;

	template <typename Stream, typename Key, typename Value> void SerializeHashMap(Stream& stream, const HashMap<Key, Value>& map)
	{
		stream << static_cast<uint32_t>(map.size());
		for (const auto& [key, value] : map)
		{
			stream << key << value;
		}
	}

	template <typename Stream, typename Key, typename Value> void DeserializeHashMap(Stream& stream, HashMap<Key, Value>& map)
	{
		uint32_t size = 0;
		stream >> size;
		Key	  key;
		Value value;
		for (uint32_t i = 0; i < size; ++i)
		{
			stream >> key >> value;
			map[key] = value;
		}
	}

	template <typename Stream, typename U> void DeserializeVector(Stream& stream, Vector<U>& vec)
	{
		uint32 sz = 0;
		stream >> sz;
		vec.resize(sz);
		if constexpr (std::is_pointer<U>::value)
		{
			for (uint32 i = 0; i < sz; i++)
			{
				using UnderlyingType = typename std::remove_pointer<U>::type;
				vec[i]				 = new UnderlyingType();
				vec[i]->LoadFromStream(stream);
			}
		}
		else
		{
			for (uint32 i = 0; i < sz; i++)
				stream >> vec[i];
		}
	}

	template <typename Stream, typename U> void SerializeVector(Stream& stream, Vector<U>& vec)
	{
		const uint32 sz = static_cast<uint32>(vec.size());
		stream << sz;

		if constexpr (std::is_pointer<U>::value)
		{
			for (auto item : vec)
				item->SaveToStream(stream);
		}
		else
		{
			for (auto& item : vec)
				stream << item;
		}
	}

	template <typename Stream, typename U> void SerializeVector(Stream& stream, const Vector<U>& vec)
	{
		const uint32 sz = static_cast<uint32>(vec.size());
		stream << sz;

		if constexpr (std::is_pointer<U>::value)
		{
			for (auto item : vec)
				item->SaveToStream(stream);
		}
		else
		{
			for (const auto& item : vec)
				stream << item;
		}
	}

	template <typename T> IStream& operator>>(IStream& stream, T& val)
	{
		if constexpr (std::is_arithmetic_v<T>)
		{
			stream.Read(val);
			if (Endianness::ShouldSwap())
			{
				Endianness::SwapEndian(val);
			}
		}
		else if constexpr (std::is_same_v<T, String> || std::is_same_v<T, std::string>)
		{
			uint32 sz = 0;
			stream >> sz;
			void* d = MALLOC(sz);
			stream.ReadToRawEndianSafe(d, static_cast<size_t>(sz));
			String s((char*)d, sz);
			val = s;
			FREE(d);
		}
		else if constexpr (std::is_enum_v<T>)
		{
			uint8 u8 = 0;
			stream >> u8;
			val = static_cast<T>(u8);
		}
		else if constexpr (is_vector_v<T>)
		{
			DeserializeVector(stream, val);
		}
		else if constexpr (is_hashmap_v<T>)
		{
			using KeyType	= typename T::key_type;
			using ValueType = typename T::mapped_type;
			DeserializeHashMap(stream, val);
		}
		else if constexpr (std::is_class_v<T>)
		{
			// Handle custom classes or structs
			val.LoadFromStream(stream);
		}
		else
		{
			assert(false);
		}

		return stream;
	}

	class OStream;

	class OStream
	{
	public:
		void CreateReserve(size_t size);
		void Destroy();
		void CheckGrow(size_t sz);
		void WriteToOFStream(std::ofstream& stream);
		void WriteRawEndianSafe(const uint8* ptr, size_t size);
		void WriteRaw(const uint8* ptr, size_t size);
		void WriteTo(Span<uint8>& span);

		void WriteRaw(const Span<uint8>& span)
		{
			WriteRaw(span.data(), span.size());
		}

		template <typename T> void Write(T& t)
		{
			if (m_data == nullptr)
				CreateReserve(sizeof(T));

			uint8* ptr	= (uint8*)&t;
			size_t size = sizeof(T);

			CheckGrow(size);
			MEMCPY(&m_data[m_currentSize], ptr, size);
			m_currentSize += size;
		}

		inline size_t GetCurrentSize() const
		{
			return m_currentSize;
		}

		inline uint8* GetDataRaw() const
		{
			return m_data;
		}

		inline void Shrink(size_t size)
		{
			m_currentSize = size;
		}

	private:
		uint8* m_data		 = nullptr;
		size_t m_currentSize = 0;
		size_t m_totalSize	 = 0;
	};

	template <typename T> OStream& operator<<(OStream& stream, T& val)
	{
		if constexpr (std::is_arithmetic_v<T>)
		{
			auto copy = const_cast<typename std::remove_const<T>::type&>(val);
			if (Endianness::ShouldSwap())
				Endianness::SwapEndian(copy);
			stream.Write<T>(copy);
		}
		else if constexpr (std::is_same_v<T, String> || std::is_same_v<T, const String>)
		{
			const uint32 sz = static_cast<uint32>(val.size());
			stream << sz;
			stream.WriteRawEndianSafe((uint8*)val.data(), val.size());
		}
		else if constexpr (std::is_enum_v<T>)
		{
			const uint8 u8 = static_cast<uint8>(val);
			stream << u8;
		}
		else if constexpr (is_vector_v<T>)
		{
			SerializeVector(stream, val);
		}
		else if constexpr (is_hashmap_v<T>)
		{
			using KeyType	= typename T::key_type;
			using ValueType = typename T::mapped_type;
			SerializeHashMap(stream, val);
		}
		else if constexpr (std::is_class_v<T>)
		{
			// Handle custom classes or structs
			val.SaveToStream(stream);
		}
		else
		{
			assert(false);
		}

		return stream;
	}

	template <typename T> OStream& operator<<(OStream& stream, T&& val)
	{
		if constexpr (std::is_arithmetic_v<T>)
		{
			auto copy = const_cast<typename std::remove_const<T>::type&>(val);
			if (Endianness::ShouldSwap())
				Endianness::SwapEndian(copy);
			stream.Write<T>(copy);
		}
		else if constexpr (std::is_same_v<T, String>)
		{
			const uint32 sz = static_cast<uint32>(val.size());
			stream << sz;
			stream.WriteRawEndianSafe((uint8*)val.data(), val.size());
		}
		else if constexpr (std::is_enum_v<T>)
		{
			const uint8 u8 = static_cast<uint8>(val);
			stream << u8;
		}
		else if constexpr (is_vector_v<T>)
		{
			// Handle vector
			const uint32 sz = static_cast<uint32>(val.size());
			stream << sz;
			for (const auto& item : val)
				stream << val;
		}
		else if constexpr (is_hashmap_v<T>)
		{
			// Handle hashmap
			const uint32 sz = static_cast<uint32>(val.size());
			stream << sz;

			for (const auto& [key, value] : val)
			{
				stream << key;
				stream << val;
			}
		}
		else if constexpr (std::is_class_v<T>)
		{
			// Handle custom classes or structs
			val.SaveToStream(stream);
		}
		else
		{
			assert(false);
		}

		return stream;
	}

	class RawStream
	{
	public:
		RawStream() : m_data({}){};
		RawStream(const RawStream& other) = delete;

		~RawStream()
		{
		}

		void Create(OStream& stream);
		void Create(const Span<uint8>& sp);
		void Create(uint8* data, size_t size);
		void Destroy();

		inline Span<uint8> GetSpan()
		{
			return m_data;
		}

		inline uint8* GetRaw() const
		{
			return m_data.data();
		}

		inline size_t GetSize() const
		{
			return m_data.size();
		}

		bool IsEmpty() const
		{
			return m_data.empty();
		}

		void SaveToStream(OStream& stream) const;
		void LoadFromStream(IStream& stream);

	private:
		Span<uint8> m_data;
	};

} // namespace Lina
