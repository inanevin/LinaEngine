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

#include "World/LevelManager.hpp"
#include "Core/Engine.hpp"
#include "World/Level.hpp"
#include "Core/EngineSettings.hpp"
#include "Core/ResourceManager.hpp"
#include "Resources/ResourceStorage.hpp"
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
        // Uninstall current level if exists.
        if (m_currentLevel != nullptr)
        {
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
            Vector<String> v;
            v.push_back(path);
            Resources::ResourceManager::Get()->LoadFilesFromPackage(Resources::ResourceStorage::Get()->PackageTypeToString(Resources::PackageType::Level), v);

            // Now simply retrieve the pointer to the level.
            m_currentLevel = Resources::ResourceStorage::Get()->GetResource<Level>(path);
        }

        // Notify that we are starting to load the resources for this level.
        Event::EventSystem::Get()->Trigger<Event::ELevelInstallStarted>(Event::ELevelInstallStarted{});

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
