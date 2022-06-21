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

#include "Core/ResourceStorage.hpp"
#include "EventSystem/EventSystem.hpp"
#include "EventSystem/ResourceEvents.hpp"
#include "Utility/ResourceUtility.hpp"
#include "Core/ResourceHandle.hpp"

namespace Lina::Resources
{

    ResourceStorage*         ResourceStorage::s_instance = nullptr;
    Set<ResourceHandleBase*> ResourceHandleBase::s_resourceHandles;

    void ResourceStorage::Initialize(ApplicationInfo& appInfo)
    {
        LINA_TRACE("[Initialization] -> Resource Storage {0}", typeid(*this).name());

        m_packager.Initialize(appInfo.m_appMode);

        Event::EventSystem::Get()->Connect<Event::EResourcePathUpdated, &ResourceStorage::OnResourcePathUpdated>(this);
        Event::EventSystem::Get()->Connect<Event::EResourceReloaded, &ResourceStorage::OnResourceReloaded>(this);
        Event::EventSystem::Get()->Connect<Event::EResourceUnloaded, &ResourceStorage::OnResourceUnloaded>(this);

        m_appInfo = appInfo;

        m_workingDirectory = std::filesystem::current_path().string().c_str();
        m_workingDirectoryReplaced = m_workingDirectory;
        std::replace(m_workingDirectoryReplaced.begin(), m_workingDirectoryReplaced.end(), '\\', '/');

        // Fill the folder structure.
        if (appInfo.m_appMode == ApplicationMode::Editor)
            ScanRootFolder();
    }

    void ResourceStorage::Shutdown()
    {
        LINA_TRACE("[Shutdown] -> Resource Storage {0}", typeid(*this).name());

        for (auto& [resType, cache] : m_resources)
        {
            for (auto& [sid, ptr] : cache)
            {
                GetTypeData(resType).deleteFunc(ptr);
            }
            cache.clear();
        }
        m_resources.clear();

        delete ResourceUtility::s_rootFolder;
    }

    void ResourceStorage::OnResourcePathUpdated(const Event::EResourcePathUpdated& ev)
    {
        // Find the resources with the updated sid, add the new string ID & delete the previous one.
        for (auto& [typeID, cache] : m_resources)
        {
            for (auto& [stringID, ptr] : cache)
            {
                if (stringID == ev.m_previousStringID)
                {
                    IResource* res = static_cast<IResource*>(ptr);
                    res->m_sid     = ev.m_newStringID;
                    res->m_path    = ev.m_newPath;

                    cache[ev.m_newStringID] = ptr;
                    cache.erase(stringID);

                    break;
                }
            }
        }

        for (auto* handle : ResourceHandleBase::s_resourceHandles)
            handle->ResourcePathUpdated(ev);
    }

    void ResourceStorage::OnResourceReloaded(const Event::EResourceReloaded& ev)
    {
        for (auto* handle : ResourceHandleBase::s_resourceHandles)
            handle->ResourceReloaded(ev);
    }

    void ResourceStorage::OnResourceUnloaded(const Event::EResourceUnloaded& ev)
    {
        for (auto* handle : ResourceHandleBase::s_resourceHandles)
            handle->ResourceUnloaded(ev);
    }

    TypeID ResourceStorage::GetTypeIDFromExtension(const String& extension)
    {
        bool   found = false;
        TypeID tid   = -1;

        for (auto& typeCachePair : m_resourceTypes)
        {
            auto& extensions = typeCachePair.second.associatedExtensions;

            for (int i = 0; i < extensions.size(); i++)
            {
                if (extensions[i].compare(extension) == 0)
                {
                    tid   = typeCachePair.first;
                    found = true;
                    break;
                }
            }

            if (found)
                break;
        }

        return tid;
    }

    String ResourceStorage::PackageTypeToString(PackageType type)
    {
        switch (type)
        {
        case PackageType::Static:
            return "static";
        case PackageType::Custom:
            return "custom";
        case PackageType::Audio:
            return "audio";
        case PackageType::Level:
            return "levels";
        case PackageType::Graphics:
            return "graphics";
        case PackageType::Meshes:
            return "meshes";
        case PackageType::Physics:
            return "physics";
        case PackageType::Textures:
            return "textures";
        default:
            return "";
        }
    }


} // namespace Lina::Resources
