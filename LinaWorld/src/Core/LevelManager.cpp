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

#include "Core/LevelManager.hpp"
#include "Core/Level.hpp"
#include "Core/ResourceManager.hpp"
#include "Log/Log.hpp"
#include "EventSystem/LevelEvents.hpp"
#include "Core/ResourceLoader.hpp"

namespace Lina::World
{
    LevelManager* LevelManager::s_instance = nullptr;

    void LevelManager::Initialize()
    {
        LINA_TRACE("[Initialization] -> Level Manager {0}", typeid(*this).name());
    }

    void LevelManager::Shutdown()
    {
        LINA_TRACE("[Shutdown] -> Level Manager {0}", typeid(*this).name());
    }

    void LevelManager::InstallLevel(const String& path, bool loadAsync)
    {
        const bool sameLevel = m_currentLevel != nullptr ? m_currentLevel->GetSID() == HashedString(path.c_str()).value() : false;

        // Uninstall current level if exists.
        // We don't use UninstallCurrent() here as it'll uninstall all resources that level contains.
        // We want to keep the shared resources.
        if (m_currentLevel != nullptr)
        {
            // Uninstall current level.
            m_currentLevel->Uninstall();
            Event::EventSystem::Get()->Trigger<Event::ELevelUninstalled>(Event::ELevelUninstalled{});
            Resources::ResourceManager::Get()->Unload<Level>(m_currentLevel->GetSID());
        }

        // Load level file itself.
        Resources::ResourceManager* storage = Resources::ResourceManager::Get();
        Event::EventSystem::Get()->Trigger<Event::EResourceProgressStarted>(Event::EResourceProgressStarted{.title = "Loading level file.", .totalFiles = 1});
        storage->GetLoader()->LoadResource(GetTypeID<Level>(), path, false);
        Event::EventSystem::Get()->Trigger<Event::EResourceProgressEnded>();
        m_currentLevel = storage->GetResource<Level>(path);

        // Notify that we are starting to load the resources for this level.
        Event::EventSystem::Get()->Trigger<Event::ELevelInstallStarted>(Event::ELevelInstallStarted{});

        // Load level resources
        const auto& load = [storage, this]() {
            storage->GetLoader()->LoadLevelResources(m_currentLevel->GetResources());
            m_currentLevel->ResourcesLoaded();
            Event::EventSystem::Get()->Trigger<Event::ELevelResourcesLoaded>(Event::ELevelResourcesLoaded{.path = m_currentLevel->GetPath()});
        };

        if (loadAsync)
            JobSystem::Get()->GetResourceExecutor().SilentAsync(load);
        else
            load();

        // Activate the level, actively switching the ECS registry & then notify.
        // Level might still be unplayable if the resources are not loaded (level loaded async).
        m_currentLevel->Install();
        Event::EventSystem::Get()->Trigger<Event::ELevelInstalled>(Event::ELevelInstalled{.path = m_currentLevel->GetPath()});
    }

    void LevelManager::UninstallCurrent()
    {
        if (m_currentLevel == nullptr)
        {
            LINA_WARN("[Level Manager] -> No level to uninstall.");
            return;
        }

        // Uninstall current level.
        m_currentLevel->Uninstall();
        Event::EventSystem::Get()->Trigger<Event::ELevelUninstalled>(Event::ELevelUninstalled{});

        // Unload resources
        auto& resources = m_currentLevel->GetResources();
        auto* storage   = Resources::ResourceManager::Get();
        for (const auto& pair : resources)
        {
            const StringID sid = TO_SID(pair.second);
            if (!g_defaultResources.IsEngineResource(pair.first, sid) && storage->Exists(pair.first, sid))
                storage->Unload(pair.first, pair.second);
        }

        Resources::ResourceManager::Get()->Unload<Level>(m_currentLevel->GetSID());
        m_currentLevel = nullptr;
    }

    void LevelManager::SaveCurrentLevel()
    {
        if (m_currentLevel == nullptr)
            return;

        if (g_appInfo.GetAppMode() == ApplicationMode::Standalone)
            return;

        const String path = m_currentLevel->GetPath();
        m_currentLevel->SaveToFile(path);

        LINA_TRACE("[Level Manager] -> Level saved: {0}", m_currentLevel->GetPath());
    }

    void LevelManager::CreateLevel(const String& path)
    {
        Level lvl;
        lvl.SaveToFile(path);
    }

} // namespace Lina::World
