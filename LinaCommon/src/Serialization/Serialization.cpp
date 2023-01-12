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
#include <iostream>
#include <fstream>
#include <filesystem>
namespace Lina::Serialization
{
    void SaveArchiveToFile(const String& path, Archive<OStream>& archive)
    {
        std::ofstream wf(path.c_str(), std::ios::out | std::ios::binary);

        if (!wf)
        {
            LINA_ERR("[Serialization] -> Could not open file for writing! {0}", path);
            return;
        }

        if (Utility::FileExists(path))
            Utility::DeleteFileInPath(path);

        const uint32 archiveSize = static_cast<uint32>(archive.GetStream().GetCurrentSize());

        // Compress if between 750 kb and 150 megabytes
        uint8 shouldCompress = (archiveSize < 150000000 && archiveSize > 750000) ? 1 : 0;
        archive(shouldCompress);

        // Uncompressed data size
        const uint32 size = archiveSize + sizeof(uint8) + sizeof(uint32);
        archive(size);

        if (shouldCompress)
        {
            OStream compressed = Compressor::Compress(archive.GetStream());
            archive.GetStream().Destroy();
            compressed.WriteToStream(wf);
            wf.close();
            compressed.Destroy();
        }
        else
        {
            archive.GetStream().WriteToStream(wf);
            wf.close();
            archive.GetStream().Destroy();
        }

        if (!wf.good())
        {
            LINA_ERR("[Serialization] -> Error occured while writing the file! {0}", path);
            return;
        }
    }

    Archive<IStream> LoadArchiveFromFile(const String& path)
    {
        std::ifstream rf(path.c_str(), std::ios::out | std::ios::binary);

        if (!rf)
        {
            LINA_ERR("[Serialization] -> Could not open file for reading! {0}", path);
            return Archive<IStream>();
        }

        auto size = std::filesystem::file_size(path.c_str());

        // Create
        IStream readStream;
        readStream.CreateFromPreAllocated(SERIALIZATION_LINEARBLOCK_SID, size);
        readStream.ReadFromStream(rf);
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

        Archive<IStream> arch;

        if (shouldDecompress)
        {
            IStream decompressedStream = Compressor::Decompress(readStream, static_cast<size_t>(uncompressedSize));
            arch.SetStream(decompressedStream);
            readStream.Destroy();
        }
        else
            arch.SetStream(readStream);

        return arch;
    }

} // namespace Lina::Serialization