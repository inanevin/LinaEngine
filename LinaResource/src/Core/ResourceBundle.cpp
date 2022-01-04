/*
This file is a part of: Lina Engine
https://github.com/inanevin/LinaEngine

Author: Inan Evin
http://www.inanevin.com

Copyright (c) [2018-2020] [Inan Evin]

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

#include "Core/ResourceBundle.hpp"
#include "Resources/ResourceStorage.hpp"
#include "Core/ResourceManager.hpp"
#include "EventSystem/EventSystem.hpp"
#include "EventSystem/ResourceEvents.hpp"
#include "Log/Log.hpp"
#include "Math/Math.hpp"
#include "Utility/UtilityFunctions.hpp"

namespace Lina::Resources
{
    MemoryEntry::~MemoryEntry()
    {
        m_data.clear();
    }

    void ResourceBundle::PushResourceFromMemory(const std::string& path, std::vector<unsigned char>& data)
    {
        auto*             storage   = Resources::ResourceStorage::Get();
        const std::string extension = Utility::GetFileExtension(Utility::GetFileNameOnly(path));
        auto&             typeData  = storage->GetTypeData(storage->GetTypeIDFromExtension(extension));
        m_memoryResources.push(MemoryEntry(typeData.m_loadPriority, path, data));
        data.clear();
    }

    void ResourceBundle::LoadAllMemoryResources()
    {
        auto* storage = Resources::ResourceStorage::Get();
        while (!m_memoryResources.empty())
        {

            MemoryEntry       entry     = m_memoryResources.top();
            const std::string extension = Utility::GetFileExtension(Utility::GetFileNameOnly(entry.m_path));
            TypeID            tid       = storage->GetTypeIDFromExtension(extension);
            StringIDType      sid       = StringID(entry.m_path.c_str()).value();

            if (m_lastResourcePriority != entry.m_priority)
            {
                m_lastResourcePriority = entry.m_priority;
                Event::EventSystem::Get()->Trigger<Event::EAllResourcesOfTypeLoaded>(Event::EAllResourcesOfTypeLoaded{m_lastResourceTypeID});
            }
            m_lastResourceTypeID = tid;

            if (!storage->Exists(tid, sid))
            {
                auto&      typeData       = storage->GetTypeData(tid);
                IResource* res            = typeData.m_createFunc();
                void*      loadedResource = res->LoadFromMemory(entry.m_path, &entry.m_data[0], entry.m_data.size());
                storage->Add(loadedResource, tid, sid);
                Event::EventSystem::Get()->Trigger<Event::EResourceLoadCompleted>(Event::EResourceLoadCompleted{tid, sid});

                ResourceManager::s_currentProgressData.m_currentResourceName = entry.m_path;
                ResourceManager::s_currentProgressData.m_currentProcessedFiles++;
                ResourceManager::s_currentProgressData.m_currentProgress = ((float)ResourceManager::s_currentProgressData.m_currentProcessedFiles / (float)ResourceManager::s_currentProgressData.m_currentTotalFiles) * 100.0f;
                ResourceManager::s_currentProgressData.m_currentProgress = Math::Clamp(ResourceManager::s_currentProgressData.m_currentProgress, 0.0f, 100.0f);
                ResourceManager::TriggerResourceUpdatedEvent();
            }

            m_memoryResources.pop();
        }
    }

    void ResourceBundle::ScanFileResources(Utility::Folder* folder)
    {
        // Recursively scan children.
        for (auto* folder : folder->m_folders)
            ScanFileResources(folder);

        // We find the resource type id by extension,
        // get it's priority from the storage and add it into a priority queue.
        auto* storage = ResourceStorage::Get();
        for (auto* file : folder->m_files)
        {
            TypeID tid = storage->GetTypeIDFromExtension(file->m_extension);

            if (tid != -1)
            {
                auto& typeData = storage->GetTypeData(tid);
                m_fileResources.push(FileEntry(typeData.m_loadPriority, file));
            }
        }
    }

    void ResourceBundle::LoadAllFileResources()
    {
        // We load every single entry in the file resource priority queue.
        auto* storage = ResourceStorage::Get();
        while (!m_fileResources.empty())
        {
            auto         fileEntry = m_fileResources.top();
            TypeID       tid       = storage->GetTypeIDFromExtension(fileEntry.m_file->m_extension);
            StringIDType sid       = StringID(fileEntry.m_file->m_fullPath.c_str()).value();

            if (m_lastResourcePriority != fileEntry.m_priority)
            {
                m_lastResourcePriority = fileEntry.m_priority;
                Event::EventSystem::Get()->Trigger<Event::EAllResourcesOfTypeLoaded>(Event::EAllResourcesOfTypeLoaded{m_lastResourceTypeID});
            }
            m_lastResourceTypeID = tid;

            LoadSingleFile(tid, fileEntry.m_file->m_fullPath);

            m_fileResources.pop();
        }
    }

    void ResourceBundle::LoadSingleFile(TypeID tid, const std::string& path)
    {
        auto*        storage = ResourceStorage::Get();
        StringIDType sid     = StringID(path.c_str()).value();

        if (!storage->Exists(tid, sid))
        {
            auto&      typeData       = storage->GetTypeData(tid);
            IResource* res            = typeData.m_createFunc();
            void*      loadedResource = res->LoadFromFile(path);

            storage->Add(loadedResource, tid, sid);
            Event::EventSystem::Get()->Trigger<Event::EResourceLoadCompleted>(Event::EResourceLoadCompleted{tid, sid});

            ResourceManager::s_currentProgressData.m_currentResourceName = path;
            ResourceManager::s_currentProgressData.m_currentProcessedFiles++;
            ResourceManager::s_currentProgressData.m_currentProgress = ((float)ResourceManager::s_currentProgressData.m_currentProcessedFiles / (float)ResourceManager::s_currentProgressData.m_currentTotalFiles) * 100.0f;
            ResourceManager::s_currentProgressData.m_currentProgress = Math::Clamp(ResourceManager::s_currentProgressData.m_currentProgress, 0.0f, 100.0f);
            ResourceManager::TriggerResourceUpdatedEvent();
        }
    }

} // namespace Lina::Resources
