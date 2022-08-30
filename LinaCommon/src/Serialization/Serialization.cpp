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

        wf.write((char*)archive.m_stream.m_data, archive.m_stream.m_currentSize);
        wf.close();

        if (!wf.good())
        {
            LINA_ERR("[Serialization] -> Error occured while writing the file! {0}", path);
            return;
        }

        archive.m_stream.Destroy();
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

        Archive<IStream> arch;
        arch.m_stream.Create(size);
        rf.read(arch.m_stream.m_data, size);
        rf.close();

        if (!rf.good())
            LINA_ERR("[Serialization] -> Error occured while reading the file! {0}", path);

        return arch;
    }

} // namespace Lina::Serialization