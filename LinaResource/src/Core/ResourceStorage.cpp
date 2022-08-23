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
#include "Data/DataCommon.hpp"
#include "EventSystem/EventSystem.hpp"
#include "EventSystem/ResourceEvents.hpp"
#include "Utility/ResourceUtility.hpp"
#include "Core/ResourceHandle.hpp"
#include "Loaders/EditorResourceLoader.hpp"
#include "Loaders/StandaloneResourceLoader.hpp"

namespace Lina::Resources
{

    void ResourceCache::Add(StringID sid, void* ptr)
    {
        m_resources[sid] = ptr;
    }

    void ResourceCache::Remove(StringID sid)
    {
        const auto& it = m_resources.find(sid);
        if (it != m_resources.end())
            m_resources.erase(it);
    }

    void ResourceCache::Unload(StringID sid, bool removeFromMap)
    {
        if (!Exists(sid))
        {
            LINA_WARN("Resource you are trying to unload does not exists! {0}", ResourceStorage::Get()->GetPathFromSID(sid));
            return;
        }

        LINA_TRACE("[Resource Storage] -> Unloading resource: {0}", ResourceStorage::Get()->GetPathFromSID(sid));
        Event::EventSystem::Get()->Trigger<Event::EResourceUnloaded>(Event::EResourceUnloaded{sid, m_typeData.tid});

        auto* ptr = GetResource(sid);
        m_typeData.deleteFunc(ptr);

        if (removeFromMap)
            Remove(sid);
    }

    void ResourceCache::UnloadAll()
    {
        for (auto& [sid, ptr] : m_resources)
            Unload(sid, false);

        m_resources.clear();
    }

    bool ResourceCache::Exists(StringID sid)
    {
        return m_resources.find(sid) != m_resources.end();
    }

    void* ResourceCache::GetResource(StringID sid)
    {
        return m_resources[sid];
    }

    void ResourceCache::AddResourceHandle(ResourceHandleBase* handle)
    {
        m_handles.insert(handle);
    }

    void ResourceCache::RemoveResourceHandle(ResourceHandleBase* handle)
    {
        m_handles.erase(handle);
    }

    ResourceStorage* ResourceStorage::s_instance = nullptr;

    void ResourceStorage::Initialize()
    {
        LINA_TRACE("[Initialization] -> Resource Storage {0}", typeid(*this).name());

        Event::EventSystem::Get()->Connect<Event::EResourceLoaded, &ResourceStorage::OnResourceLoaded>(this);
        Event::EventSystem::Get()->Connect<Event::EResourcePathUpdated, &ResourceStorage::OnResourcePathUpdated>(this);
        Event::EventSystem::Get()->Connect<Event::EResourceReloaded, &ResourceStorage::OnResourceReloaded>(this);
        Event::EventSystem::Get()->Connect<Event::EResourceUnloaded, &ResourceStorage::OnResourceUnloaded>(this);

        if (g_appInfo.GetAppMode() == ApplicationMode::Editor)
            m_loader = new EditorResourceLoader();
        else
            m_loader = new StandaloneResourceLoader();

        ResourceUtility::InitializeWorkingDirectory();

        // Fill the folder structure.
        if (g_appInfo.GetAppMode() == ApplicationMode::Editor)
            ResourceUtility::ScanRootFolder();
    }

    void ResourceStorage::Shutdown()
    {
        LINA_TRACE("[Shutdown] -> Resource Storage {0}", typeid(*this).name());

        Event::EventSystem::Get()->Disconnect<Event::EResourceLoaded>(this);
        Event::EventSystem::Get()->Disconnect<Event::EResourcePathUpdated>(this);
        Event::EventSystem::Get()->Disconnect<Event::EResourceReloaded>(this);
        Event::EventSystem::Get()->Disconnect<Event::EResourceUnloaded>(this);

        for (auto& [tid, cache] : m_caches)
            cache.UnloadAll();

        m_caches.clear();

        delete ResourceUtility::s_rootFolder;
        delete m_loader;
    }

    void ResourceStorage::Load(TypeID tid, const String& path, bool loadAsync)
    {
        GetLoader()->LoadResource(tid, path, loadAsync);
    }

    void ResourceStorage::UnloadAll()
    {
        LINA_TRACE("[Resource Storage] -> Unloading all.");

        for (auto& cachePair : m_caches)
            cachePair.second.UnloadAll();
    }

    void ResourceStorage::OnResourceLoaded(const Event::EResourceLoaded& ev)
    {
        // In case there were invalidated handlers, notify them.
        auto& cache = m_caches[ev.tid];
        for (auto* handle : cache.m_handles)
        {
            if (handle->sid == ev.sid)
                handle->OnResourceLoaded();
        }
    }

    void ResourceStorage::OnResourcePathUpdated(const Event::EResourcePathUpdated& ev)
    {
        auto&       cache = m_caches[ev.tid];
        const auto& it    = cache.m_resources.find(ev.previousStringID);
        if (it != cache.m_resources.end())
        {
            auto*      ptr = it->second;
            IResource* res = static_cast<IResource*>(ptr);
            res->m_sid     = ev.newStringID;
            res->m_path    = ev.newPath;
            cache.Remove(ev.previousStringID);
            cache.Add(ev.newStringID, ptr);
        }

        // Check if any resource handles were using that resource.
        for (auto* handle : cache.m_handles)
        {
            if (handle->sid == ev.previousStringID)
                handle->OnResourceSIDChanged(ev.newStringID);
        }
    }

    void ResourceStorage::OnResourceReloaded(const Event::EResourceReloaded& ev)
    {
    }

    void ResourceStorage::OnResourceUnloaded(const Event::EResourceUnloaded& ev)
    {
        // Check if any resource handles were using that resource.
        auto& cache = m_caches[ev.tid];
        for (auto* handle : cache.m_handles)
        {
            if (handle->sid == ev.sid)
                handle->OnResourceUnloaded();
        }
    }

    TypeID ResourceStorage::GetTypeIDFromExtension(const String& extension)
    {

        for (const auto& [tid, cache] : m_caches)
        {
            const auto& it = linatl::find_if(cache.m_typeData.associatedExtensions.begin(), cache.m_typeData.associatedExtensions.end(),
                                             [extension](const String& ext) {
                                                 return ext.compare(extension) == 0;
                                             });
            if (it != cache.m_typeData.associatedExtensions.end())
                return tid;
        }

        return -1;
    }

    String ResourceStorage::GetPathFromSID(StringID sid)
    {
#ifdef LINA_ENABLE_LOGGING

        for (auto& [tid, cache] : m_caches)
        {
            for (auto& [s, ptr] : cache.m_resources)
            {
                if (s == sid && ptr)
                {
                    IResource* res = static_cast<IResource*>(ptr);
                    return res->GetPath();
                }
            }
        }

        return "-path-not-found-";

#else
        return "";
#endif
    }

} // namespace Lina::Resources
