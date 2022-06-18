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

#include "World/Level.hpp"

#include "Core/Application.hpp"
#include "Core/RenderEngine.hpp"
#include "ECS/Components/EntityDataComponent.hpp"
#include "ECS/Components/MeshRendererComponent.hpp"
#include "ECS/Components/ModelRendererComponent.hpp"
#include "ECS/Components/SpriteRendererComponent.hpp"
#include "ECS/Registry.hpp"
#include "Log/Log.hpp"
#include "Utility/UtilityFunctions.hpp"
#include "Resources/ResourceStorage.hpp"
#include "EventSystem/LevelEvents.hpp"
#include <cereal/archives/portable_binary.hpp>
#include <fstream>
#include <stdio.h>

namespace Lina::World
{
    Level* Level::s_currentLevel = nullptr;

    void Level::Install()
    {
        if (s_currentLevel != nullptr)
            s_currentLevel->Uninstall();

        ECS::Registry::s_ecs = &m_registry;
        s_currentLevel       = this;
        SetupData();
    }

    void Level::Uninstall()
    {
        Event::EventSystem::Get()->Trigger<Event::ELevelUninstalled>(Event::ELevelUninstalled{});
        m_registry.clear();
    }

    void Level::SaveToFile(const std::string& path)
    {
        Event::EventSystem::Get()->Trigger<Event::EPreSerializingLevel>(Event::EPreSerializingLevel{});

        std::ofstream stream(path, std::ios::binary);
        {
            cereal::PortableBinaryOutputArchive oarchive(stream);
            oarchive(*this);
            m_registry.SerializeComponentsInRegistry(oarchive);
        }
        Event::EventSystem::Get()->Trigger<Event::ESerializedLevel>(Event::ESerializedLevel{});
    }

    void Level::InstallFromFile(const std::string& path)
    {
        if (s_currentLevel != nullptr)
            s_currentLevel->Uninstall();

        std::ifstream stream(path, std::ios::binary);
        {
            cereal::PortableBinaryInputArchive iarchive(stream);
            iarchive(*this);
            m_registry.DeserializeComponentsInRegistry(iarchive);
        }

        ECS::Registry::s_ecs = &m_registry;
        s_currentLevel       = this;
        SetupData();
    }

    void Level::SetupData()
    {
        Graphics::RenderEngine::Get()->SetSkyboxMaterial(m_skyboxMaterial.m_value);
        Graphics::RenderEngine::Get()->GetLightingSystem()->SetAmbientColor(m_ambientColor);
        Event::EventSystem::Get()->Trigger<Event::ELevelInstalled>(Event::ELevelInstalled{});
    };

} // namespace Lina::World
