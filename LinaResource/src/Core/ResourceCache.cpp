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

#include "Core/ResourceCache.hpp"

namespace Lina::Resources
{
    void MetadataCache::Destroy()
    {
        for (auto& [sid, var] : m_variables)
            FREE(var.ptr);

        m_variables.clear();
    }

    void ResourceCacheBase::AddResourceHandle(ResourceHandleBase* handle)
    {
        m_handles.insert(handle);
    }
    void ResourceCacheBase::RemoveResourceHandle(ResourceHandleBase* handle)
    {
        m_handles.erase(handle);
    }

    void ResourceCacheBase::SaveMetadataToArchive(Serialization::Archive<OStream>& archive)
    {
        const uint32 size = static_cast<uint32>(m_metaCache.size());
        archive(size);

        for (auto& [sid, metaCache] : m_metaCache)
        {
            const auto&  variables     = metaCache.m_variables;
            const uint32 variablesSize = static_cast<uint32>(variables.size());
            archive(sid);
            archive(variablesSize);

            for (auto& [variableSid, var] : variables)
            {
                const uint32 varSize = static_cast<uint32>(var.size);
                archive(variableSid);
                archive(varSize);
                archive.GetStream().WriteEndianSafe((uint8*)var.ptr, var.size);
            }
        }
    }

    void ResourceCacheBase::LoadMetadataFromArchive(Serialization::Archive<IStream>& archive)
    {
        uint32 size = 0;
        archive(size);

        for (uint32 i = 0; i < size; i++)
        {
            uint32 variablesSize = 0;
            uint32 sid           = 0;
            archive(sid);
            archive(variablesSize);

            m_metaCache[sid] = MetadataCache();

            for (uint32 k = 0; k < variablesSize; k++)
            {
                StringID variableSid = 0;
                uint32   varSize     = 0;
                archive(variableSid);
                archive(varSize);
                void* data = MALLOC(varSize);
                archive.GetStream().ReadEndianSafe(data, varSize);
                MetaVariable metaVar                      = MetaVariable{.size = varSize, .ptr = data};
                m_metaCache[sid].m_variables[variableSid] = metaVar;
            }
        }
    }

    void ResourceCacheBase::Shutdown()
    {
        for (auto& [sid, cache] : m_metaCache)
            cache.Destroy();
    }
} // namespace Lina::Resources
