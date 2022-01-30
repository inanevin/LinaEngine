/*
This file is a part of: Lina Engine
https://github.com/inanevin/Lina

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

#include "Core/ResourceManager.hpp"
#include "Resources/ResourceStorage.hpp"
#include "EventSystem/EventSystem.hpp"
#include "EventSystem/ResourceEvents.hpp"
#include "Log/Log.hpp"
#include "Utility/UtilityFunctions.hpp"

namespace Lina::Resources
{

#define RESOURCEPACKAGE_EXTENSION ".linabundle"

    ResourceManager*     ResourceManager::s_resourceManager = nullptr;
    ResourceProgressData ResourceManager::s_currentProgressData;

    void ResourceManager::LoadEditorResources()
    {
        LINA_WARN("[Resource Manager] -> Loading Editor Resources");

        // Find resources.
        m_rootFolder = new Utility::Folder();
        m_rootFolder->m_fullPath = "Resources/";
        m_rootFolder->m_name    = "Resources";
        Utility::ScanFolder(m_rootFolder, true, &s_currentProgressData.m_currentTotalFiles);

        // Set progress & fill.
        s_currentProgressData.m_state                 = ResourceProgressState::Pending;
        s_currentProgressData.m_progressTitle         = "Loading resources...";
        s_currentProgressData.m_state                 = ResourceProgressState::InProgress;
        s_currentProgressData.m_currentProcessedFiles = 0;

        m_bundle.ScanFileResources(m_rootFolder);
        m_bundle.LoadAllFileResources();

        Event::EventSystem::Get()->Trigger<Event::EAllResourcesLoaded>(Event::EAllResourcesLoaded {});
        ResourceManager::ResetProgress();
    }

    void ResourceManager::OnRequestResourceReload(const Event::ERequestResourceReload& ev)
    {
        m_bundle.LoadSingleFile(ev.m_tid, ev.m_fullPath);
        m_eventSys->Trigger<Event::EResourceReloaded>(Event::EResourceReloaded{ev.m_tid, ev.m_sid});
    }

    void ResourceManager::Shutdown()
    {
        // Make sure we don't have any packing/unpacking going on.
        if (m_future.valid())
        {
            m_future.cancel();
            m_future.get();
        }

        delete m_rootFolder;

        LINA_TRACE("[Shutdown] -> Resource Manager {0}", typeid(*this).name());
    }

    void ResourceManager::ResetProgress()
    {
        s_currentProgressData.m_currentProcessedFiles = s_currentProgressData.m_currentTotalFiles = 0;
        s_currentProgressData.m_currentResourceName = s_currentProgressData.m_progressTitle = "";
        s_currentProgressData.m_state                                                       = ResourceProgressState::None;
        s_currentProgressData.m_currentProgress                                             = 0.0f;
    }

    void ResourceManager::TriggerResourceUpdatedEvent()
    {
        Event::EventSystem::Get()->Trigger<Event::EResourceLoadUpdated>(Event::EResourceLoadUpdated{ResourceManager::s_currentProgressData.m_currentResourceName, ResourceManager::s_currentProgressData.m_currentProgress});
    }

    void ResourceManager::PackageProject(const std::string& path, const std::string& name)
    {
        // Find out which resources to export.
        std::vector<std::string> filesToPack;
        AddAllResourcesToPack(filesToPack, m_rootFolder);

        // Export resources.
        m_packager.PackageFileset(filesToPack, path + "/" + name + RESOURCEPACKAGE_EXTENSION, m_appInfo.m_packagePass);
    }

    void ResourceManager::Initialize(ApplicationInfo& appInfo)
    {
        m_appInfo    = appInfo;
        m_eventSys   = Event::EventSystem::Get();
        m_eventSys->Connect<Event::ERequestResourceReload, &ResourceManager::OnRequestResourceReload>(this);
    }

    void ResourceManager::AddAllResourcesToPack(std::vector<std::string>& resources, Utility::Folder* folder)
    {
        for (auto& childFolder : folder->m_folders)
            AddAllResourcesToPack(resources, childFolder);

        for (auto& file : folder->m_files)
            resources.push_back(file->m_fullPath);
    }

    void ResourceManager::ImportResourceBundle(const std::string& path, const std::string& name)
    {
        const std::string fullPath = path + name + RESOURCEPACKAGE_EXTENSION;
        if (!Utility::FileExists(fullPath))
        {
            LINA_ERR("Package does not exist, aborting import. {0}", fullPath);
            return;
        }

        std::string fullBundlePath                  = fullPath;
        s_currentProgressData.m_progressTitle       = "Unpacking level resources...";
        s_currentProgressData.m_currentResourceName = fullBundlePath;

        // Start unpacking process, preceeded & followed by an event dispatch.
        // m_eventSys->Trigger<Event::EResourceProgressStarted>();

        // Start unpacking.
        m_packager.Unpack(fullBundlePath, m_appInfo.m_packagePass, &m_bundle);
        m_bundle.LoadAllMemoryResources();

        // Set progress end.
        ResetProgress();
    }

} // namespace Lina::Resources
