#include "World/LevelManager.hpp"
#include "Core/Engine.hpp"
#include "World/Level.hpp"
#include "Core/EngineSettings.hpp"
#include "Core/ResourceManager.hpp"
#include "Core/ResourceStorage.hpp"
#include "Log/Log.hpp"
#include "EventSystem/LevelEvents.hpp"

namespace Lina::World
{
    LevelManager* LevelManager::s_instance = nullptr;

    void LevelManager::Initialize(ApplicationInfo appInfo)
    {
        m_appInfo = appInfo;
        LINA_TRACE("[Initialization] -> Level Manager {0}", typeid(*this).name());
    }

    void LevelManager::Shutdown()
    {
        LINA_TRACE("[Shutdown] -> Level Manager {0}", typeid(*this).name());
    }

    void LevelManager::InstallLevel(const String& path)
    {
        HashMap<TypeID, HashSet<StringIDType>> previousLevelResources;

        const bool sameLevel = m_currentLevel->GetSID() == StringID(path.c_str()).value();

        // Uninstall current level if exists.
        if (m_currentLevel != nullptr)
        {
            // Store the current levels resources for cross-checking with the target level to load
            if (!sameLevel)
                previousLevelResources = m_currentLevel->GetResources();

            // Uninstall current level.
            Event::EventSystem::Get()->Trigger<Event::ELevelUninstalled>(Event::ELevelUninstalled{});
            m_currentLevel->Uninstall();

            Resources::ResourceStorage::Get()->Unload<Level>(path);
        }

        if (m_appInfo.m_appMode == ApplicationMode::Editor)
        {
            // Create the level & add it to the resource storage.
            m_currentLevel = new Level();
            Resources::ResourceStorage::Get()->Add(m_currentLevel, GetTypeID<Level>(), StringID(path.c_str()).value());

            // Load level data from the file, this will also load the ECS registry.
            m_currentLevel->LoadFromFile(path);
        }
        else
        {
            // In standalone builds, level data would be packed into the respective levels package.
            // We first need to load the target file from the package into memory, this will create the level resource managed by the resource storage.
            HashSet<StringIDType> v;
            v.insert(StringID(path.c_str()).value());
            Resources::ResourceManager::Get()->LoadFilesFromPackage(Resources::ResourceStorage::Get()->PackageTypeToString(Resources::PackageType::Level), v);

            // Now simply retrieve the pointer to the level.
            m_currentLevel = Resources::ResourceStorage::Get()->GetResource<Level>(path);
        }

        // Notify that we are starting to load the resources for this level.
        Event::EventSystem::Get()->Trigger<Event::ELevelInstallStarted>(Event::ELevelInstallStarted{});

        // Don't change any resources if we are loading the same level.
        if (!sameLevel)
        {
            auto& resources = m_currentLevel->GetResources();
            auto* storage = Resources::ResourceStorage::Get();
            auto* manager = Resources::ResourceManager::Get();

            for (auto& pair : previousLevelResources)
            {
                const HashSet<StringIDType> set = resources.at(pair.first);
                
                // For every type id, search all the resources referenced by the previous level.
                // If the same resource is not reference by the level we are loading right now, unload them.
                for (auto sid : pair.second)
                {
                    if (set.find(sid) == set.end())
                        storage->Unload(pair.first, sid);
                }
            }

            for (auto& pair : resources)
            {
                Resources::ResourceTypeData& typeData = storage->GetTypeData(pair.first);

                if(m_appInfo.m_appMode == ApplicationMode::Standalone)
                manager->LoadFilesFromPackage(storage->PackageTypeToString(typeData.packageType), pair.second);
                else
                {
                   
                }
            }
        }

        // After all resources are loaded, activate the level, actively switching the ECS registry & then notify.
        m_currentLevel->Install();
        Event::EventSystem::Get()->Trigger<Event::ELevelInstalled>(Event::ELevelInstalled{});
    }

    void LevelManager::SaveCurrentLevel()
    {
        if (m_currentLevel == nullptr)
            return;

        if (m_appInfo.m_appMode == ApplicationMode::Standalone)
            return;

        const String path = m_currentLevel->GetPath();
        m_currentLevel->SaveToFile(path);
    }

    void LevelManager::CreateLevel(const String& path)
    {
        Level lvl;
        lvl.SaveToFile(path);
        lvl.SetSID(path);
    }

} // namespace Lina::World
