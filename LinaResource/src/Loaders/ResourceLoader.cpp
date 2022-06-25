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

#include "Loaders/ResourceLoader.hpp"
#include "Core/ResourceStorage.hpp"
#include "Utility/UtilityFunctions.hpp"
#include "EventSystem/EventSystem.hpp"

namespace Lina::Resources
{
    MemoryEntry::~MemoryEntry()
    {
        m_data.clear();
    }

    void ResourceLoader::PushResourceFromMemory(const String& path, Vector<unsigned char>& data)
    {
        auto*        storage   = Resources::ResourceStorage::Get();
        const String extension = Utility::GetFileExtension(Utility::GetFileNameOnly(path));
        auto&        typeData  = storage->GetTypeData(storage->GetTypeIDFromExtension(extension));
        m_memoryResources.push(MemoryEntry(typeData.loadPriority, path, data));
        data.clear();
    }

    void ResourceLoader::LoadAllMemoryResources()
    {
        auto* storage = Resources::ResourceStorage::Get();
        while (!m_memoryResources.empty())
        {

            MemoryEntry  entry     = m_memoryResources.top();
            const String extension = Utility::GetFileExtension(Utility::GetFileNameOnly(entry.m_path));
            TypeID       tid       = storage->GetTypeIDFromExtension(extension);
            StringIDType sid       = StringID(entry.m_path.c_str()).value();

            if (m_lastResourcePriority != entry.m_priority)
                m_lastResourcePriority = entry.m_priority;

            m_lastResourceTypeID = tid;

            if (!storage->Exists(tid, sid))
            {
                auto&      typeData       = storage->GetTypeData(tid);
                IResource* res            = typeData.createFunc();
                void*      loadedResource = res->LoadFromMemory(entry.m_path, &entry.m_data[0], entry.m_data.size());
                storage->Add(loadedResource, tid, sid);
                Event::EventSystem::Get()->Trigger<Event::EResourceLoaded>(Event::EResourceLoaded{tid, sid});
            }

            m_memoryResources.pop();
        }
    }

    void ResourceLoader::ScanResourcesInFolder(Utility::Folder* folder)
    {

        // Recursively scan children.
        for (auto* folder : folder->folders)
            ScanResourcesInFolder(folder);

        // We find the resource type id by extension,
        // get it's priority from the storage and add it into a priority queue.
        auto* storage = ResourceStorage::Get();
        for (auto* file : folder->files)
        {
            TypeID tid = storage->GetTypeIDFromExtension(file->extension);

            if (tid != -1)
            {
                auto& typeData = storage->GetTypeData(tid);
                m_fileResources.push(FileEntry(typeData.loadPriority, file));
            }
        }
    }

    void ResourceLoader::LoadAllScannedResources()
    {
        // We load every single entry in the file resource priority queue.
        auto* storage = ResourceStorage::Get();
        while (!m_fileResources.empty())
        {
            auto         fileEntry = m_fileResources.top();
            TypeID       tid       = storage->GetTypeIDFromExtension(fileEntry.m_file->extension);
            StringIDType sid       = StringID(fileEntry.m_file->fullPath.c_str()).value();

            if (m_lastResourcePriority != fileEntry.m_priority)
                m_lastResourcePriority = fileEntry.m_priority;

            m_lastResourceTypeID = tid;
            LoadSingleResourceFromFile(tid, fileEntry.m_file->fullPath);

            m_fileResources.pop();
        }
    }

    void ResourceLoader::UnloadUnusedResources(const HashMap<TypeID, HashSet<String>>& currentLevelResources)
    {
        ResourceStorage* storage = ResourceStorage::Get();

        // Find all resources that are not used by the next level, unload them.
        auto& resourceTypes = storage->GetResourceTypes();

        // For every resource type
        for (auto pair : resourceTypes)
        {
            if (pair.second.packageType == PackageType::Static || pair.second.packageType == PackageType::Level)
                continue;

            if (pair.second.isAssetData)
                continue;

            const Cache& cache = storage->GetCache(pair.first);

            // If the new resources to load doesn't contain the type id at all, unload all resoruces belonging to this type id.
            if (!currentLevelResources.contains(pair.first))
            {
                for (auto existingResource : cache)
                    storage->Unload(pair.first, existingResource.first);

                continue;
            }

            const auto& set = currentLevelResources.at(pair.first);

            // Search all resources belonging to the type.
            for (auto existingResource : cache)
            {
                bool found = false;
                // If resource doesn't exist in the resourceMap to load, unload it.
                for (auto path : set)
                {
                    if (StringID(path.c_str()).value() == existingResource.first)
                    {
                        found = true;
                        break;
                    }
                }

                if (!found)
                    storage->Unload(pair.first, existingResource.first);
            }
        }
    }

    bool ResourceLoader::LoadSingleResourceFromFile(TypeID tid, const String& path)
    {
        auto*        storage = ResourceStorage::Get();
        StringIDType sid     = StringID(path.c_str()).value();

        if (!storage->Exists(tid, sid))
        {
            auto&      typeData       = storage->GetTypeData(tid);
            IResource* res            = typeData.createFunc();
            void*      loadedResource = res->LoadFromFile(path);

            if (loadedResource == nullptr)
                return false;

            storage->Add(loadedResource, tid, sid);
            Event::EventSystem::Get()->Trigger<Event::EResourceLoaded>(Event::EResourceLoaded{tid, sid});
        }

        return true;
    }

} // namespace Lina::Resources