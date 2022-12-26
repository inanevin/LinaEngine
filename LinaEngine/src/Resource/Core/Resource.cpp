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

#include "Resource/Core/Resource.hpp"
#include "Serialization/Serialization.hpp"

namespace Lina::Resources
{

    String Resource::GetFilename(const String& path)
    {
        const String basePath  = "Resources/Editor/Metacache/";
        const String ext       = ".linameta";
        const String name      = Utility::GetFileWithoutExtension(Utility::GetFileNameOnly(path));
        const String sidStr    = TO_STRING(TO_SID(path));
        const String finalName = basePath + name + "-" + sidStr + ext;

        if (!Utility::FileExists(basePath))
            Utility::CreateFolderInPath(basePath);

        return finalName;
    }

    Serialization::Archive<IStream> Resource::GetMetaArchive(const String& path)
    {
        const String p = GetFilename(path);
        if (!Utility::FileExists(p))
            return Serialization::Archive<IStream>();

        return Serialization::LoadArchiveFromFile(p);
    }

    void Resource::SaveMetaArchive(Serialization::Archive<OStream>& arch, const String& path)
    {
        const String p = GetFilename(path);
        Serialization::SaveArchiveToFile(p, arch);
    }
    bool Resource::MetaArchiveExists(const String& path)
    {
        const String p = GetFilename(path);
        return Utility::FileExists(p);
    }

    void Resource::LoadAssetData()
    {
        if (!MetaArchiveExists(m_path))
            SaveAssetData();

        auto archive = GetMetaArchive(m_path);
        LoadFromArchive(archive);
        archive.GetStream().Destroy();
    }

    void Resource::SaveAssetData()
    {
        Serialization::Archive<OStream> archive;
        archive.GetStream().CreateReserve(200);
        SaveToArchive(archive);
        SaveMetaArchive(archive, m_path);
    }
} // namespace Lina::Resources
