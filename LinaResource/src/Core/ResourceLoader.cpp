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

#include "Core/ResourceLoader.hpp"
#include "Core/ResourceManager.hpp"
#include "Core/ResourceCache.hpp"
#include "Core/Resource.hpp"
#include "Data/Streams.hpp"
#include "Core/CommonEngine.hpp"
#include "JobSystem/JobSystem.hpp"
#include <iostream>
#include <fstream>
#include <filesystem>

namespace Lina::Resources
{
    void ResourceLoader::LoadLevelResources(const Vector<Pair<TypeID, String>>& resources)
    {
    }
    void ResourceLoader::LoadEngineResources()
    {
        Vector<Pair<TypeID, String>> engineResources;
        auto&                        engineRes = g_defaultResources.GetEngineResources();

        for (auto& pair : engineRes)
        {
            for (auto& res : pair.second)
                engineResources.push_back(linatl::make_pair(pair.first, res));
        }

        LoadResources(engineResources, false);
    }

    void ResourceLoader::LoadResources(const Vector<Pair<TypeID, String>>& resources, bool async)
    {
        // Categorize per packageType
        HashMap<PackageType, Vector<StringID>> resourcesPerTID;
        for (auto& pair : resources)
        {
            auto& typeData = ResourceManager::Get()->GetCache(pair.first)->GetTypeData();
            resourcesPerTID[typeData.packageType].push_back(HashedString(pair.second.c_str()).value());
        }

        for (auto& [pkgType, vec] : resourcesPerTID)
        {
            const String& packagePath    = "Packages" + GetPackageTypeName(pkgType);
            const uint32  packageVersion = GetPackageVersion(pkgType);

            auto          size = std::filesystem::file_size(packagePath.c_str());
            std::ifstream rf(packagePath.c_str(), std::ios::out | std::ios::binary);

            if (!rf)
            {
                LINA_ERR("[Resource Loader]-> Could not open package! {0}", packagePath);
                return;
            }

            IStream istream;
            istream.Create(size);
            rf.read(istream.m_data, size);
            rf.close();

            uint32 version = 0;
            istream >> version;

            if (version != packageVersion)
            {
                LINA_ERR("[Resource Loader] -> Package versions does not match! Current: {0} - File: {1}", packageVersion, version);
                LINA_ASSERT(false, "");
                return;
            }

            auto exists = [&](StringID sid) {
                for (auto s : vec)
                {
                    if (s == sid)
                        return true;
                }
                return false;
            };

            Vector<Pair<TidSid, IStream>> resourcesToLoad;

            while (!istream.IsCompleted())
            {
                TypeID   typeID = 0;
                StringID sid    = 0;
                uint32   size   = 0;
                istream >> typeID;
                istream >> sid;
                istream >> size;

                // found the target resource.
                if (exists(sid))
                {
                    IStream newStream;
                    newStream.Create(&istream.m_data[istream.m_index], size);
                    TidSid tidSid = {typeID, sid};

                    if (!async)
                        CreateResource(tidSid, newStream, false);
                    else
                        resourcesToLoad.push_back(linatl::make_pair(tidSid, newStream));
                }

                istream.SkipBy(size);
            }

            if (async)
            {
                Taskflow tf;
                tf.for_each(resourcesToLoad.begin(), resourcesToLoad.end(), [this](auto& pair) {
                    CreateResource(pair.first, pair.second, false);
                });
                JobSystem::Get()->GetResourceExecutor().Run(tf).wait();
            }

            istream.Destroy();
        }
    }

    void ResourceLoader::LoadResource(TypeID tid, const String& path, bool async)
    {
        LoadResources({linatl::make_pair(tid, path)}, async);
    }

    void ResourceLoader::CreateResource(TidSid tidSid, const IStream& stream, bool async)
    {
        // Capt by val.
        auto load = [tidSid, stream] {
            ResourceTypeData typeData = Resources::ResourceManager::Get()->GetCache(tidSid.tid)->GetTypeData();
            Resource*        res      = Resources::ResourceManager::Get()->GetCache(tidSid.tid)->Create(tidSid.sid);
            res->LoadFromMemory(stream);
        };

        if (async)
            JobSystem::Get()->GetResourceExecutor().SilentAsync(load);
        else
            load();
    }
} // namespace Lina::Resources
