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

    String Resource::GetFilename(const String& path, uint32 version)
    {
        const String basePath   = "Resources/Editor/Metacache/";
        const String ext        = ".linameta";
        const String name       = Utility::GetFileWithoutExtension(Utility::GetFileNameOnly(path));
        const String sidStr     = TO_STRING(TO_SID(path));
        const String versionStr = "_v" + TO_STRING(version) + "_";
        const String finalName  = basePath + name + "-" + sidStr + versionStr + ext;

        if (!Utility::FileExists(basePath))
            Utility::CreateFolderInPath(basePath);

        return finalName;
    }

    Serialization::Archive<IStream> Resource::GetMetaArchive(const String& path, uint32 version)
    {
        const String p = GetFilename(path, version);
        if (!Utility::FileExists(p))
            return Serialization::Archive<IStream>();

        return Serialization::LoadArchiveFromFile(p);
    }

    void Resource::SaveMetaArchive(Serialization::Archive<OStream>& arch, const String& path, uint32 version)
    {
        const String p = GetFilename(path, version);
        Serialization::SaveArchiveToFile(p, arch);
    }
    bool Resource::MetaArchiveExists(const String& path, uint32 version)
    {
        const String p = GetFilename(path, version);
        return Utility::FileExists(p);
    }

    void Resource::LoadAssetData()
    {
        const uint32 version = GetVersion();
        if (!MetaArchiveExists(m_path, version))
            SaveAssetData();

        auto archive = GetMetaArchive(m_path, version);
        LoadFromArchive(archive);
        archive.GetStream().Destroy();
    }

    void Resource::SaveAssetData(uint32 reserveSize)
    {
        Serialization::Archive<OStream> archive;
        archive.GetStream().CreateReserveFromPreAllocated(SERIALIZATION_LINEARBLOCK_SID, reserveSize);
        SaveToArchive(archive);
        SaveMetaArchive(archive, m_path, GetVersion());
    }
} // namespace Lina::Resources
