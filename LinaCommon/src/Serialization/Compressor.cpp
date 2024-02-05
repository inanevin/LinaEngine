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

#include "Common/Serialization/Compressor.hpp"
#include "Common/Data/Streams.hpp"
#include "Common/Serialization/CommonSerialization.hpp"

#define LZ4_STATIC_LINKING_ONLY_DISABLE_MEMORY_ALLOCATION
#include "lz4/lz4.h"

namespace Lina
{
	size_t EstimateDecompressSize(size_t compressedSize)
	{
		// return (compressedSize << 8) - compressedSize - 2526;
		return 255 * compressedSize + 24;
	}

	OStream Compressor::Compress(OStream& stream)
	{
		const uint32 streamSize		  = static_cast<uint32>(stream.GetCurrentSize());
		const uint8	 shouldCompress	  = (streamSize < 150000000 && streamSize > 750000) ? 1 : 0;
		const uint32 uncompressedSize = streamSize + sizeof(uint8) + sizeof(uint32);

		stream << shouldCompress;
		stream << uncompressedSize;

		if (!shouldCompress)
		{
			OStream compressed;
			compressed.CreateReserve(stream.GetCurrentSize());
			compressed.WriteRaw(stream.GetDataRaw(), stream.GetCurrentSize());
			return compressed;
		}

		const int size			= static_cast<int>(stream.GetCurrentSize());
		const int compressBound = LZ4_compressBound(size);

		// Create stream capable of holding max compressed bytes.
		OStream compressedStream = OStream();
		compressedStream.CreateReserve(compressBound);
		char* dest		   = (char*)compressedStream.GetDataRaw();
		char* data		   = (char*)stream.GetDataRaw();
		int	  bytesWritten = LZ4_compress_default(data, dest, size, compressBound);

		if (bytesWritten == 0)
		{
			LINA_ERR("[Compressor] -> LZ4 compression failed!");
		}

		compressedStream.Shrink(static_cast<size_t>(bytesWritten));
		return compressedStream;
	}
	IStream Compressor::Decompress(IStream& stream)
	{
		// Read uncompressed size of archive.
		uint8  shouldDecompress = 0;
		uint32 uncompressedSize = 0;
		stream.Seek(stream.GetSize() - sizeof(uint32) - sizeof(uint8));
		stream.Read(shouldDecompress);
		stream.Read(uncompressedSize);
		stream.Seek(0);

		if (!shouldDecompress)
		{
			IStream copy;
			copy.Create(stream.GetDataRaw(), stream.GetSize() - sizeof(uint32) - sizeof(uint8));
			return copy;
		}

		const size_t size				= stream.GetSize();
		IStream		 decompressedStream = IStream();
		decompressedStream.Create(uncompressedSize);
		void*	  src			   = stream.GetDataRaw();
		void*	  ptr			   = decompressedStream.GetDataRaw();
		const int decompressedSize = LZ4_decompress_safe((char*)src, (char*)ptr, static_cast<int>(size), static_cast<int>(uncompressedSize));
		decompressedStream.Shrink(static_cast<size_t>(decompressedSize) - sizeof(uint32) - sizeof(uint8));
		return decompressedStream;
	}
} // namespace Lina
