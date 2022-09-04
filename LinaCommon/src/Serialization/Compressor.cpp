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

#include "Serialization/Compressor.hpp"
#include "Data/Streams.hpp"

#define LZ4_STATIC_LINKING_ONLY_DISABLE_MEMORY_ALLOCATION
#include <lz4/lz4.h>

namespace Lina::Serialization
{
    size_t EstimateDecompressSize(size_t compressedSize)
    {
        // return (compressedSize << 8) - compressedSize - 2526;
        return 255 * compressedSize + 24;
    }

    OStream Compressor::Compress(const OStream& stream)
    {
        const int size          = static_cast<int>(stream.GetCurrentSize());
        const int compressBound = LZ4_compressBound(size);

        // Create stream capable of holding max compressed bytes.
        OStream compressedStream;
        compressedStream.CreateReserve(compressBound);
        char* dest = (char*)compressedStream.GetDataRaw();
        
        char* data         = (char*)stream.GetDataRaw();
        int   bytesWritten = LZ4_compress_default(data, dest, size, compressBound);
        compressedStream.Shrink(static_cast<size_t>(bytesWritten));

        return compressedStream;
    }
    IStream Compressor::Decompress(IStream& stream)
    {

        const size_t size            = stream.GetSize();
        const size_t decompressedEst = EstimateDecompressSize(size);
        IStream      decompressedStream;
        decompressedStream.Create(decompressedEst);
        void*     src              = stream.GetDataRaw();
        void*     ptr              = decompressedStream.GetDataRaw();
        const int decompressedSize = LZ4_decompress_safe((char*)src, (char*)ptr, static_cast<int>(size), static_cast<int>(decompressedEst));
        decompressedStream.Shrink(static_cast<size_t>(decompressedSize));
        return decompressedStream;
    }
} // namespace Lina::Serialization
