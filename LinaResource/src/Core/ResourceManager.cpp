/*
This file is a part of: Lina Engine
https://github.com/inanevin/Lina

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

#include "Core/ResourceManager.hpp"
#include "Resources/ResourceStorage.hpp"
#include "EventSystem/EventSystem.hpp"
#include "EventSystem/ResourceEvents.hpp"
#include "Log/Log.hpp"
#include "Utility/UtilityFunctions.hpp"

namespace Lina::Resources
{

#define RESOURCEPACKAGE_EXTENSION String(".linapackage")

    ResourceManager*     ResourceManager::s_resourceManager = nullptr;
    ResourceProgressData ResourceManager::s_currentProgressData;

    void ResourceManager::Initialize(ApplicationInfo& appInfo)
    {
        LINA_TRACE("[Initialization] -> Resource Manager {0}", typeid(*this).name());

        m_appInfo  = appInfo;
        m_eventSys = Event::EventSystem::Get();
        m_eventSys->Connect<Event::ERequestResourceReload, &ResourceManager::OnRequestResourceReload>(this);
        m_workingDirectory         = std::filesystem::current_path().string().c_str();
        m_workingDirectoryReplaced = m_workingDirectory;
        std::replace(m_workingDirectoryReplaced.begin(), m_workingDirectoryReplaced.end(), '\\', '/');

        // Fill the folder structure.
        if (appInfo.m_appMode == ApplicationMode::Editor)
            ScanRootFolder();
    }

    void ResourceManager::Shutdown()
    {
        // TODO: Make sure we don't have any packing/unpacking going on.
        delete m_rootFolder;
        LINA_TRACE("[Shutdown] -> Resource Manager {0}", typeid(*this).name());
    }

    bool ResourceManager::LoadSingleResource(TypeID tid, const String& relativePath)
    {
        const String path = m_workingDirectoryReplaced + "/" + relativePath;
        return m_bundle.LoadSingleResource(tid, relativePath);
    }

    void ResourceManager::LoadResourcesInFolder(Utility::Folder* folder)
    {
        // Set progress & fill.
        s_currentProgressData.m_state                 = ResourceProgressState::Pending;
        s_currentProgressData.m_progressTitle         = "Loading resources in folder [" + folder->m_name + "]";
        s_currentProgressData.m_state                 = ResourceProgressState::InProgress;
        s_currentProgressData.m_currentProcessedFiles = 0;

        m_bundle.ScanResourcesInFolder(m_rootFolder);
        m_bundle.LoadAllScannedResources();
        ResourceManager::ResetProgress();
    }

    void ResourceManager::PackageProject(const String& path, const Vector<String>& levelResources, const HashMap<TypeID, HashSet<StringIDType>>& resourceMap)
    {
        ScanRootFolder();
        auto*          storage = ResourceStorage::Get();
        Vector<String> filesToPack;
        auto&          allTypes = storage->GetResourceTypes();

        // Pack the static resources, all static resources should be loaded into the memory during the lifetime of the editor.
        // So we can find them from the resource cache.
        for (const auto& [tid, typeData] : allTypes)
        {
            if (typeData.packageType == PackageType::Static)
            {
                const Cache& cache = storage->GetCache(tid);

                for (auto& pair : cache)
                {
                    String fullpath = SearchFolderForSID(m_rootFolder, pair.first);
                    if (fullpath.compare("") != 0)
                        filesToPack.push_back(fullpath);
                }
            }
        }

        const String workingPath = path.compare("") == 0 ? "Packages/" : (path + "/Packages/");
        if (Utility::FileExists(workingPath))
            Utility::DeleteDirectory(workingPath);
        Utility::CreateFolderInPath(workingPath);

        m_packager.PackageFileset(filesToPack, workingPath + + "static" + RESOURCEPACKAGE_EXTENSION, m_appInfo.m_packagePass);

        // Pack the level resources, level resources might not be alive in the resource storage.
        // Thus use the table sent by the engine to figure out what to pack.
        m_packager.PackageFileset(levelResources, workingPath + "levels" + RESOURCEPACKAGE_EXTENSION, m_appInfo.m_packagePass);

        // Now for all resource types that are marked as non-static, package them into their respective packages.
        // We do not use the storage to get the resources here, we merely use it to check the type data, which is guaranteed to be registered.
        // We package the files based on the resource table sent by the engine here.
        for (const auto& [tid, sidVector] : resourceMap)
        {
            ResourceTypeData& typeData = storage->GetTypeData(tid);

            if (typeData.packageType != PackageType::Static)
            {
                filesToPack.clear();

                // Find the full paths & add them based on sids.
                for (auto sid : sidVector)
                {
                    String fullpath = SearchFolderForSID(m_rootFolder, sid);
                    if (fullpath.compare("") != 0)
                        filesToPack.push_back(fullpath);
                }

                m_packager.PackageFileset(filesToPack, workingPath  + storage->PackageTypeToString(typeData.packageType) + RESOURCEPACKAGE_EXTENSION, m_appInfo.m_packagePass);
            }
        }
    }

    void ResourceManager::LoadPackage(const String& packageName)
    {
        const String fullPath = "Packages/" + packageName + RESOURCEPACKAGE_EXTENSION;
        if (!Utility::FileExists(fullPath))
        {
            LINA_ERR("Package does not exist, aborting import. {0}", fullPath);
            return;
        }

        String fullBundlePath                       = fullPath;
        s_currentProgressData.m_progressTitle       = "Unpacking resources...";
        s_currentProgressData.m_currentResourceName = fullBundlePath;

        // Start unpacking.
        m_packager.UnpackAndLoad(fullBundlePath, m_appInfo.m_packagePass, &m_bundle);
        m_bundle.LoadAllMemoryResources();

        // Set progress end.
        ResetProgress();
    }

    void ResourceManager::LoadFilesFromPackage(const String& packageName, const Vector<String>& filesToLoad)
    {
        const String fullPath = "Packages/" + packageName + RESOURCEPACKAGE_EXTENSION;
        if (!Utility::FileExists(fullPath))
        {
            LINA_ERR("Package does not exist, aborting import. {0}", fullPath);
            return;
        }

        String fullBundlePath                       = fullPath;
        s_currentProgressData.m_progressTitle       = "Unpacking resources...";
        s_currentProgressData.m_currentResourceName = fullBundlePath;

        // Start unpacking.
        m_packager.UnpackAndLoad(fullBundlePath, filesToLoad, m_appInfo.m_packagePass, &m_bundle);
        m_bundle.LoadAllMemoryResources();

        // Set progress end.
        ResetProgress();
    }

    void ResourceManager::OnRequestResourceReload(const Event::ERequestResourceReload& ev)
    {
        m_bundle.LoadSingleResource(ev.m_tid, ev.m_fullPath);
        m_eventSys->Trigger<Event::EResourceReloaded>(Event::EResourceReloaded{ev.m_tid, ev.m_sid});
    }

    void ResourceManager::TriggerResourceUpdatedEvent()
    {
        Event::EventSystem::Get()->Trigger<Event::EResourceLoadUpdated>(Event::EResourceLoadUpdated{ResourceManager::s_currentProgressData.m_currentResourceName, ResourceManager::s_currentProgressData.m_currentProgress});
    }

    void ResourceManager::ResetProgress()
    {
        s_currentProgressData.m_currentProcessedFiles = s_currentProgressData.m_currentTotalFiles = 0;
        s_currentProgressData.m_currentResourceName = s_currentProgressData.m_progressTitle = "";
        s_currentProgressData.m_state                                                       = ResourceProgressState::None;
        s_currentProgressData.m_currentProgress                                             = 0.0f;
    }

    void ResourceManager::ScanRootFolder()
    {
        if (m_rootFolder != nullptr)
            delete m_rootFolder;

        m_rootFolder             = new Utility::Folder();
        m_rootFolder->m_fullPath = "Resources/";
        m_rootFolder->m_name     = "Resources";
        Utility::ScanFolder(m_rootFolder, true, &s_currentProgressData.m_currentTotalFiles);
    }

    String ResourceManager::SearchFolderForSID(Utility::Folder* folder, StringIDType sid)
    {
        String foundFile = "";
        bool   found     = false;
        for (Utility::File* file : folder->m_files)
        {
            if (file->m_sid == sid)
            {
                foundFile = file->m_fullPath;
                found     = true;
            }
        }

        if (found)
            return foundFile;

        for (Utility::Folder* sub : folder->m_folders)
        {
            String found = SearchFolderForSID(sub, sid);

            if (found.compare("") != 0)
                return found;
        }

        return "";
    }

} // namespace Lina::Resources
