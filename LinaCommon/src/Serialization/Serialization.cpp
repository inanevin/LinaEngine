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

#include "Serialization/Serialization.hpp"
#include "FileSystem/FileSystem.hpp"
#include "Serialization/Compressor.hpp"
#include "Serialization/SerializationCommon.hpp"

namespace Lina
{
#define COMPRESS_MIN_LIMIT 1000

    bool Serialization::SaveToFile(const char* path, OStream& stream)
    {
        std::ofstream wf(path, std::ios::out | std::ios::binary);

        if (!wf)
        {
            LINA_ERR("[Serialization] -> Could not open file for writing! {0}", path);
            return false;
        }

        if (FileSystem::FileExists(path))
            FileSystem::DeleteFileInPath(path);

        const uint32 streamSize = static_cast<uint32>(stream.GetCurrentSize());

        // Compress if between 750 kb and 150 megabytes
        uint8 shouldCompress = (streamSize < 150000000 && streamSize > 750000) ? 1 : 0;
        stream << shouldCompress;

        // Uncompressed data size
        const uint32 size = streamSize + sizeof(uint8) + sizeof(uint32);
        stream << size;

        if (shouldCompress)
        {
            OStream compressed = Compressor::Compress(stream);
            compressed.WriteToOFStream(wf);
            wf.close();
            compressed.Destroy();
        }
        else
        {
            stream.WriteToOFStream(wf);
            wf.close();
        }

        if (!wf.good())
        {
            LINA_ERR("[Serialization] -> Error occured while writing the file! {0}", path);
            return false;
        }

        return true;
    }

    IStream Serialization::LoadFromFile(const char* path, MemoryAllocatorPool* allocator)
    {
        std::ifstream rf(path, std::ios::out | std::ios::binary);

        if (!rf)
        {
            LINA_ERR("[Serialization] -> Could not open file for reading! {0}", path);
            return IStream();
        }

        auto size = std::filesystem::file_size(path);

        // Create
        IStream readStream;
        readStream.SetAllocator(allocator);
        readStream.Create(size);
        readStream.ReadFromIFStream(rf);
        rf.close();

        if (!rf.good())
            LINA_ERR("[Serialization] -> Error occured while reading the file! {0}", path);

        // Read uncompressed size of archive.
        uint8  shouldDecompress = 0;
        uint32 uncompressedSize = 0;
        readStream.Seek(readStream.GetSize() - sizeof(uint32) - sizeof(uint8));
        readStream.Read(shouldDecompress);
        readStream.Read(uncompressedSize);
        readStream.Seek(0);

        if (shouldDecompress)
        {
            IStream decompressedStream = Compressor::Decompress(readStream, static_cast<size_t>(uncompressedSize));
            readStream.Destroy();
            return decompressedStream;
        }
        else
            return readStream;
    }

} // namespace Lina